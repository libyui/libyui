/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA


  File:       NCPkgMenuExtras.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#include <fstream>
#include <iomanip>
#include <boost/bind.hpp>

#include <zypp/SysContent.h>

#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include <yui/ncurses/NCPopupInfo.h>

#include "NCPackageSelector.h"
#include "NCPkgMenuExtras.h"


using std::endl;

typedef zypp::syscontent::Reader::Entry		ZyppReaderEntry;
typedef std::pair<std::string, ZyppReaderEntry>	importMapPair;

#define DEFAULT_EXPORT_FILE_NAME "user-packages.xml"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuExtras::NCPkgMenuExtras (YWidget *parent, std::string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label)
	, pkg (pkger)
{
    createLayout();
}

NCPkgMenuExtras::~NCPkgMenuExtras()
{

}

void NCPkgMenuExtras::createLayout()
{
    exportFile = new YMenuItem( _( "&Export Package List to File" ) );
    items.push_back( exportFile );

    importFile = new YMenuItem( _( "&Import Package List from File" ) );
    items.push_back( importFile );

    diskSpace = new YMenuItem( _( "&Show Available Disk Space" ) );
    items.push_back( diskSpace );

    if ( pkg->isOnlineSearchEnabled() ) {
      onlineSearch = new YMenuItem(_("Search &Online"));
      items.push_back(onlineSearch);
    }

    addItems( items );
}

bool NCPkgMenuExtras::handleEvent ( const NCursesEvent & event)
{
    if (!event.selection)
	return false;

    if ( event.selection == exportFile )
    	exportToFile();
    else if ( event.selection == importFile )
    	importFromFile();
    else if ( event.selection == diskSpace )
    	showDiskSpace();
    else if ( event.selection == onlineSearch ) {
    	const_cast<NCursesEvent &>(event).result = "online_search";
        yuiMilestone() << "Launching online search " << endl;

        //and close the main loop
    	return false;
    }

    /*else if ( event.selection == repoManager )
    {
	// return :repo_mgr symbol to the Ruby module (FaTE #302517)
	const_cast<NCursesEvent &>(event).result = "repo_mgr";
	yuiMilestone() << "Launching repository manager " << endl;

        // and close the main loop
	return false;
    }*/
    return true;
}

void NCPkgMenuExtras::importSelectable( ZyppSel selectable, bool isWanted, const char*kind )
{
    ZyppStatus oldStatus = selectable->status();
    ZyppStatus newStatus = oldStatus;

    // Package/Pattern is on the list
    if (isWanted)
    {
	switch (oldStatus)
	{
	    // Keep status for installed ones
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:
	    case S_KeepInstalled:
	    case S_Protected:
		newStatus = oldStatus;
		break;

	    // Keep also those marked for deletion
	    case S_Del:
	    case S_AutoDel:
		newStatus = S_KeepInstalled;
		yuiDebug() << "Keeping " << kind << " " << selectable->name().c_str() << endl;
		break;

	    // Add not yet installed pkgs (if they have candidate available)
	    case S_NoInst:
	    case S_Taboo:
		if ( selectable->hasCandidateObj() )
		{
		    newStatus = S_Install;
		    yuiDebug() << "Adding " << kind << " " << selectable->name().c_str() << endl;
		}
		else
		{
		    yuiDebug() << "Cannot add " << kind << " " << selectable->name().c_str() <<
			" " << " - no candidate." << endl;
		}
		break;
	}
    }
    // Package/Pattern is not on the list
    else
    {
	switch (oldStatus)
	{
	    // Mark installed ones for deletion
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:
	    case S_KeepInstalled:
	    case S_Protected:
		newStatus = S_Del;
		yuiDebug() << "Deleting " << kind << " " << selectable->name().c_str() << endl;
		break;

            // Keep status for not installed, taboo and to-be-deleted
	    case S_Del:
	    case S_AutoDel:
	    case S_NoInst:
	    case S_Taboo:
		newStatus = oldStatus;
		break;
	}
    }

    if (oldStatus != newStatus)
	selectable->setStatus( newStatus );
}


bool NCPkgMenuExtras::exportToFile()
{	// Ask for file to save into
    std::string filename = YUI::app()->askForSaveFileName( DEFAULT_EXPORT_FILE_NAME,
    						  "*.xml",
    						  _( "Export List of All Packages and Patterns to File" ));

    if ( ! filename.empty() )
    {
        zypp::syscontent::Writer writer;
        const zypp::ResPool & pool = zypp::getZYpp()->pool();

        // some strange C++ magic (c) by ma
        for_each( pool.begin(), pool.end(),
    	boost::bind( &zypp::syscontent::Writer::addIf,
    		     boost::ref(writer),
                     boost::placeholders::_1));

        try
        {
    	    // open file for writing and try to dump syscontent into it
    	    std::ofstream exportFile(  filename.c_str() );
    	    exportFile.exceptions(std::ios_base::badbit | std::ios_base::failbit );
    	    exportFile << writer;

            yuiMilestone() << "Exported list of packages and patterns to " << filename << endl;
        }

        catch (std::exception & exception)
        {
    	    yuiWarning() << "Error exporting list of packages and patterns to " << filename << endl;

    	    // delete partially written file (don't care if it doesn't exist)
    	    (void) unlink( filename.c_str() );

    	    // present error popup to the user
    	    NCPopupInfo * errorMsg = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2 ),
    	    					  NCPkgStrings::ErrorLabel(),
    	    					  _( "Error exporting list of packages and patterns to " )
    	    					  // FIXME: String addition is evil for translators!
    	    					  + filename,
    	    					  NCPkgStrings::OKLabel(),
    	    					  "");
            errorMsg->setPreferredSize(40, 5);
    	    NCursesEvent input = errorMsg->showInfoPopup();

    	    YDialog::deleteTopmostDialog();
        }

        return true;
    }

    return false;
}

bool NCPkgMenuExtras::importFromFile()
{
    // ask for file to open
    std::string filename = YUI::app()->askForExistingFile( DEFAULT_EXPORT_FILE_NAME,
    						  "*.xml",
    						  _( "Import List of All Packages and Patterns from File" ));
    if ( ! filename.empty() )
    {
        NCPkgTable * packageList = pkg->PackageList();
        yuiMilestone() << "Importing list of packages and patterns from " << filename << endl;

        try
        {
    	std::ifstream importFile ( filename.c_str() );
    	zypp::syscontent::Reader reader (importFile);

    	// maps to store package/pattern data into
    	std::map<std::string, ZyppReaderEntry> importPkgs;
    	std::map<std::string, ZyppReaderEntry> importPatterns;

    	// Import syscontent reader to a map $[ "package_name" : pointer_to_data]
    	for (zypp::syscontent::Reader::const_iterator it = reader.begin();
    	     it != reader.end();
    	     it ++ )
    	{
    	    std::string kind = it->kind();

    	    // importMapPair =>	std::pair<std::string, ZyppReaderEntry>
    	    if ( kind == "package" )
    		importPkgs.insert( importMapPair( it->name(), *it ) );
    	    else if ( kind == "pattern" )
    		importPatterns.insert( importMapPair( it->name(), *it ) );
    	}

    	yuiMilestone() << "Found " << importPkgs.size() << " packages and " << importPatterns.size() << " patterns." << endl;

    	// Change status of appropriate packages and patterns
    	for (ZyppPoolIterator it = zyppPkgBegin();
    	     it != zyppPkgEnd();
    	     it++ )
    	{
    	    ZyppSel selectable = *it;
    	    // isWanted => package name found in importPkgs map
    	    importSelectable ( *it, importPkgs.find( selectable->name() ) != importPkgs.end(), "package" );
    	}

    	for (ZyppPoolIterator it = zyppPatternsBegin();
    	     it != zyppPatternsEnd();
    	     it++ )
    	{
    	    ZyppSel selectable = *it;
    	    importSelectable ( *it, importPatterns.find( selectable->name() ) != importPatterns.end(), "pattern" );
    	}

    	// Switch to installation summary filter
    	packageList->fillSummaryList(NCPkgTable::L_Changes);

    	//... and finally display the result
    	packageList->showInformation();
    	packageList->setKeyboardFocus();

            return true;
        }
        catch ( const zypp::Exception & exception )
        {
    	yuiWarning() << "Error importing list of packages and patterns from" << filename << endl;

    	NCPopupInfo * errorMsg = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2) ,
    						  NCPkgStrings::ErrorLabel(),
    						  _( "Error importing list of packages and patterns from " )
    						  // FIXME: String addition is evil for translators!
    						  + filename,
    						  NCPkgStrings::OKLabel(),
    						  "");
        errorMsg->setPreferredSize(40, 5);
    	NCursesEvent input = errorMsg->showInfoPopup();

    	YDialog::deleteTopmostDialog();
        }
    }
    return true;

}

bool NCPkgMenuExtras::showDiskSpace()
{
      pkg->diskSpacePopup()->showInfoPopup( NCPkgStrings::DiskspaceLabel() );
    // FIXME: move focus back to pkg table?

    return true;
}
