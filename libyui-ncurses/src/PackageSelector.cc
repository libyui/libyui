/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       PackageSelector.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCPkgTable.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPopupSearch.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCPopupTree.h"
#include "NCPopupSelection.h"
#include "PackageSelector.h"
#include "YSelectionBox.h"

#include <iomanip>
#include <list>
#include <string>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPParser.h>
#include <ycp/YCPBlock.h>

using namespace std;


///////////////////////////////////////////////////////////////////
//
// CompFunc 
//
bool sortByName( PMSelectablePtr ptr1, PMSelectablePtr ptr2 )
{
    if ( ptr1->name() < ptr2->name() )
    {
	return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
// Constructor
//
PackageSelector::PackageSelector( Y2NCursesUI * ui )
    : y2ui( ui )
      , visibleInfo( YCPNull() )
      , filterPopup( 0 )
{
    // Fill the handler map
    eventHandlerMap[ PkgNames::Packages()->toString() ]	= &PackageSelector::PackageListHandler;
    eventHandlerMap[ PkgNames::Cancel()->toString() ] 	= &PackageSelector::CancelHandler;
    eventHandlerMap[ PkgNames::OkButton()->toString() ]	= &PackageSelector::OkButtonHandler;
    eventHandlerMap[ PkgNames::Search()->toString() ] 	= &PackageSelector::SearchHandler;
    eventHandlerMap[ PkgNames::Diskspace()->toString() ] = &PackageSelector::DiskspaceHandler;
    eventHandlerMap[ PkgNames::Diskinfo()->toString() ]	= &PackageSelector::DiskinfoHandler;
    // Filter menu
    eventHandlerMap[ PkgNames::RpmGroups()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Selections()->toString() ] = &PackageSelector::FilterHandler;
    // Information menu
    eventHandlerMap[ PkgNames::Files()->toString() ]   	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PkgInfo()->toString() ] 	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::LongDescr()->toString() ]= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Versions()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::RequRel()->toString() ] = &PackageSelector::InformationHandler;
    // Action menu
    eventHandlerMap[ PkgNames::Toggle()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Select()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Deselect()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Delete()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Replace()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Update()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Installed()->toString() ]= &PackageSelector::StatusHandler; 
    // help menu
    eventHandlerMap[ PkgNames::GeneralHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::StatusHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::FilterHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::InstSourceHelp()->toString() ] = &PackageSelector::HelpHandler;
    // FIXME: add handler for all `id s

    // create the filter popup
    filterPopup = new NCPopupTree( wpos( 1, 1 ),	// position
				   false );		// without a description 

    // get the available selections
    getSelections();
    // create the selections popup
    selectionPopup = new NCPopupSelection( wpos( 1, 1 ), this );

    NCstring text( "This is for UI testing only.");
    NCPopupInfo info( wpos( 1, 1 ), YCPString( "Warning" ), text.YCPstr() );
    info.showInfoPopup( );
}



///////////////////////////////////////////////////////////////////
//
// Destructor
//
PackageSelector::~PackageSelector()
{
    if ( filterPopup )
    {
	delete filterPopup;
    }
    if ( selectionPopup )
    {
	delete selectionPopup;
    }
}

///////////////////////////////////////////////////////////////////
//
// handleEvent
//
// Handle event, calls corresponding handler-function
//
bool PackageSelector::handleEvent ( const NCursesEvent&   event )
{
    bool retVal = false;
    YCPValue currentId = YCPNull();

    if ( event == NCursesEvent::handled )
	return false;
    
    // Get the id of the widget which is affected
    if ( event == NCursesEvent::button )
    {
	currentId =  dynamic_cast<YWidget *>(event.widget)->id();
	UIMIL <<  "Selected widget id: " << currentId->toString() << endl;

    }
    else if ( event == NCursesEvent::menu )
    {
	currentId =  event.selection;
	UIMIL <<  "Selected menu item: " << currentId->toString() << endl;
    }
    
    // Find the handler-function for the given widget-nameId
    if ( ! currentId.isNull() )
    {
	tHandlerMap::iterator it = eventHandlerMap.find ( currentId->toString() );
    
	if (it != eventHandlerMap.end())    // if currentId found in map
	{ 
	    tHandlerFctPtr pFct = (*it).second;
	    retVal = (this->*pFct) ( event );    // call handler
	}
	else
	{
	    UIERR <<  "Unhandled event for widget-Id: " << currentId->toString() << endl;
	    
	    // return true means: don't leave the event loop in runPkgSelection  
	    retVal = true;
	}
    }
    else
    {
	UIERR << "Unknown event or id not valid" << endl;
    }
	
    return retVal;
}


///////////////////////////////////////////////////////////////////
//
// setVisibleInfo
//
// currently displayed package information
//
void PackageSelector::setVisibleInfo( const YCPValue & info )
{
    visibleInfo = info;
}

///////////////////////////////////////////////////////////////////
//
// fillHeader
//
// Fillup the column headers of the package table 
//
void PackageSelector::fillHeader( NCPkgTable *pkgTable )
{
    vector<NCstring> header(5);

    header[0] = PkgNames::PkgStatus();
    header[1] = PkgNames::PkgName();
    header[2] = PkgNames::PkgSummary();
    header[3] = PkgNames::PkgSize();

    if ( pkgTable )
    {
	pkgTable->setHeader( header );
    }
    else
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
// fillAvailableList
//
// Fills the package table (on bottom ) with the list of available packages 
//
bool PackageSelector::fillAvailableList( NCPkgTable * pkgTable, PMObjectPtr pkgPtr )
{
    vector<string> pkgLine (4);
    unsigned int i = 0;

    if ( !pkgTable )
    {
	return false;
    }
    
    // get the selectable
    PMSelectablePtr selectable = pkgPtr->getSelectable();
    NCMIL << "Number of available packages: " << selectable->availableObjs() << endl;
    
    std::list<PMObjectPtr>::const_iterator it = selectable->av_begin();

    // show all availables
    while ( it != selectable->av_end() )
    {
	pkgLine[0] = selectable->name();	// package name
	pkgLine[1] = (*it)->summary();  	// short description
	FSize size = (*it)->size();     	// installed size
	pkgLine[2] = size.asString();

	pkgTable->addLine( PkgNoInstall, // FIXME: get the package status
			   pkgLine,
			   i,		 // the index
			   (*it) );	 // the corresponding package pointer
	i++;
	++it;
    }
    
    return true;
}
///////////////////////////////////////////////////////////////////
//
// showSelectionPackages
//
// Fills the package table with the list of packages matching
// the selected filter
//
bool PackageSelector::showSelectionPackages( NCPkgTable *pkgTable,
					     const YCPString & label,
					     int index )
{
    list<PMPackagePtr> pkgList;
    list<PMPackagePtr>::iterator listIt;
    unsigned int i;
    
    if ( !pkgTable )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    pkgTable->itemsCleared ();
    
    NCMIL << "INDEX: " << index << "  Label: " << label->toString() << endl;

    PMSelectionPtr sel;

    if ( (unsigned int)index < selections.size() )
    {
	sel = selections[index];
    }

    if ( sel )
    {
	pkgList = sel->inspacks_ptrs ( );
    }

    for ( i = 0, listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt, i++ )    
    {
	createListEntry( pkgTable, (*listIt), i );
    }
      
    if ( !label.isNull() && label->compare( YCPString("default") ) != YO_EQUAL )
    {
	// show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }
   
    return true;

}

///////////////////////////////////////////////////////////////////
//
// fillPackageList
//
// Fills the package table with the list of packages matching
// the selected filter
//
bool PackageSelector::fillPackageList( NCPkgTable *pkgTable, const YCPString & label, string filter )
{
    if ( !pkgTable )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    NCMIL <<  "Filter: " <<  label->toString() << " is selected" << endl;

    string::size_type len = filter.length();

      // clear the package table
    pkgTable->itemsCleared ();

    // get the package list and sort it
    list<PMSelectablePtr> pkgList( Y2PM::packageManager().begin(), Y2PM::packageManager().end() );
    pkgList.sort( sortByName );

    // fill the package table
    list<PMSelectablePtr>::iterator listIt;
    unsigned int i;
    PMPackagePtr pkgPtr;

    //for ( i = 0, listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt, i++ )    
    for ( i = 0, listIt = pkgList.begin(); i < 100;  ++listIt, i++ )    
    {
	PMPackagePtr instPtr = (*listIt)->installedObj();
	PMPackagePtr candPtr = (*listIt)->candidateObj();

	if ( candPtr )
	{
	    pkgPtr = candPtr;		// if exists: show the candidate
	}
	else if ( instPtr )
	{
	    pkgPtr = instPtr; 		// else if exists: the installed package
	}
	else
	{
	    pkgPtr = (*listIt)->theObject();	// else: any package 
	}

	// filter the packages
	if ( pkgPtr->group().compare( 0, len, filter ) == 0 )
	{
	     createListEntry( pkgTable, pkgPtr, i );
	}
    }
  
    if ( !label.isNull() && label->compare( YCPString("default") ) != YO_EQUAL )
    {
	// show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// createListEntry
//
//
bool PackageSelector::createListEntry ( NCPkgTable *pkgTable,
					PMPackagePtr pkgPtr,
					unsigned int index )
{
    vector<string> pkgLine (4);

    if ( !pkgPtr )
    {
	NCERR << "No valid package available" << endl;
	return false;
    }

    pkgLine[0] = pkgPtr->getSelectable()->name();	// package name
    pkgLine[1] = pkgPtr->summary();  	// short description
    FSize size = pkgPtr->size();     	// installed size
    pkgLine[2] = size.asString();

    pkgTable->addLine( PkgInstalled,	// FIXME: get the package status
		       pkgLine,
		       index,		// the index
		       pkgPtr );	// the corresponding package pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// SearchHandler
//
// Opens popup for package search and asks the package manager
//
bool PackageSelector::SearchHandler( const NCursesEvent& event)
{
    if ( !packageList )
    {
	return false;
    }
    
    // open the search poup and get the search expression
    NCPopupSearch pkgSearch( wpos( 1, 1 ) );

    YCPString search = pkgSearch.showSearchPopup();

    if ( !search.isNull() )
    {
	// FIXME: get the package list from package manager
	NCMIL << "Searching for: " <<  search->toString() << endl;

	// FIXME: use enum (or whatever) for the filter
	fillPackageList ( packageList, YCPString ( "Search" ), "" );

	showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    }
    else
    {
	NCDBG << "Search is canceled"  << endl;
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// InformationHandler
//
// Sets the member variable visibleInfo. If required,
// replaces the currently active package information widget
// ( e.g. by the list of available packages ).
//
bool PackageSelector::InformationHandler( const NCursesEvent&  event )
{
    if ( !packageList )
    {
	return false;
    }
    
    if ( visibleInfo->compare( event.selection ) == YO_EQUAL )
    {
	// information selection has not changed
	return true;
    }

    // set visibleInfo
    visibleInfo = event.selection;

    if ( visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	// show the package table
	const char * tableLayout = "`ReplacePoint( `id(`replaceinfo), `PkgSpecial( `id(`availpkgs), `opt(`notify), \"pkgTable\" ) )"; 
	YCPParser parser( tableLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );

	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));

	if ( pkgAvail )
	{
	    fillHeader( pkgAvail );
	    fillAvailableList( pkgAvail, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}
    }
    else
    {
	// show the rich text widget
	const char * textLayout = "`ReplacePoint( `id(`replaceinfo), `RichText( `id(`description), \" \") )"; 
	YCPParser parser( textLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );
    
	showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    }

    packageList->setKeyboardFocus();
    
    UIMIL << "Change package info to: " << visibleInfo->toString() << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
//
// FilterHandler
//
// Opens the popup with rpm group tree, selections ... and display the
// corresponding package list
//
bool PackageSelector::FilterHandler( const NCursesEvent&  event )
{
    YCPString item = YCPNull();
    string filter = "";

    if ( !packageList || event.selection.isNull() )
    {
	return false;
    }

    
    if ( event.selection->compare( PkgNames::RpmGroups() ) == YO_EQUAL )
    {
	// the filter popup returns the selected item 
	NCursesEvent event = filterPopup->showFilterPopup( );
	
	// get the currently selected RPM group (the label)
	item = event.item;

	// get the complete path of the RPM group
	vector<string> itemList = event.itemList;
	if ( !itemList.empty() && !item.isNull() )
	{
	    filter = itemList[0];
	    NCMIL << "RPM group in PackageSelector: " << filter << endl;
	    // fill the package list with packages belonging to the filter
	    fillPackageList( packageList, item, filter );
	}
    }
    else if ( event.selection->compare( PkgNames::Selections() ) == YO_EQUAL )
    {
	// the selection popup returns the NCursesEvent 
	NCursesEvent event = selectionPopup->showSelectionPopup( );

	// get the currently selected list entry
	item = event.item;
	// get all selected categories
	vector<string> itemList = event.itemList;

	NCMIL << "selection INDEX: " << event.detail << endl;
	
	if ( !itemList.empty() ) 
	{
	    NCMIL << "First selected: " << itemList[0] << endl;

	    // FIXME: inform the packagemanager about the selected categories
	}

	if ( !item.isNull() )
	{
	    // show packages belonging to selected list entry
	    showSelectionPackages( packageList, item, event.detail );
	}
    }

    showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    packageList->setKeyboardFocus();	
    
    // return true means: don't leave the event loop in runPkgSelection 
    return true;
}

///////////////////////////////////////////////////////////////////
//
// StatusHandler
//
// Sets the package status
//
bool PackageSelector::StatusHandler( const NCursesEvent&  event )
{
    if ( !packageList )
    {
	return false;
    }

    // call the corresponding method of NCPkgTable
    if ( event.selection->compare( PkgNames::Toggle() ) == YO_EQUAL )
    {
	packageList->toggleStatus( packageList->getDataPointer( packageList->getCurrentItem() ));
    }
    else if ( event.selection->compare( PkgNames::Select() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToInstall );
    }
    else if ( event.selection->compare( PkgNames::Deselect() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgNoInstall );
    }
    else if ( event.selection->compare( PkgNames::Delete() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToDelete );
    }
    else if ( event.selection->compare( PkgNames::Replace() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToReplace );
    }
    else if ( event.selection->compare( PkgNames::Update() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToUpdate );
    }
    else if ( event.selection->compare( PkgNames::Installed() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgInstalled );	
    }
    packageList->setKeyboardFocus();
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// PackageListHandler
//
// Handles actions not directly related to the package list.
// 
bool PackageSelector::PackageListHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskinfoHandler
//
// Opens a popup with disk space information.
// 
bool PackageSelector::DiskinfoHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskspaceHandler
// 
// Show the required disk space
//
bool PackageSelector::DiskspaceHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// HelpHandler
// 
// Show the help popup
//
bool PackageSelector::HelpHandler( const NCursesEvent&  event )
{
    NCstring text ( "" );
    
    if ( event.selection->compare( PkgNames::GeneralHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpPkgInst1();
	text += PkgNames::HelpPkgInst12();
	text += PkgNames::HelpPkgInst13();
	text += PkgNames::HelpPkgInst2();
	text += PkgNames::HelpPkgInst3();
	text += PkgNames::HelpPkgInst4();
	text += PkgNames::HelpPkgInst5();		
    }
    else if ( event.selection->compare( PkgNames::StatusHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnStatus1();
	text += PkgNames::HelpOnStatus2();
	text += PkgNames::HelpOnStatus3();
	// FIXME: text depends on flag isNew/isPostInstallation 

	// text += PkgNames::HelpOnStatusNew();
	text += PkgNames::HelpOnStatusPost();
    }
    
    // open the popup with the help text
    NCPopupInfo pkgHelp( wpos( 1, 1 ), PkgNames::PackageHelp(), text.YCPstr() );
    pkgHelp.showInfoPopup( );
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// CancelHandler
//
// Cancel button handler.
// 
bool PackageSelector::CancelHandler( const NCursesEvent&  event )
{
    // FIXME - show a popup and ask the user

    NCMIL <<  "Cancel button pressed - leaving package selection" << endl;
    
    // return false, which means stop the event loop (see runPkgSelection)
    return false;
}

///////////////////////////////////////////////////////////////////
//
// OkButtonHandler
//
// OK button handler.
// 
bool PackageSelector::OkButtonHandler( const NCursesEvent&  event )
{
    // FIXME - do a final dependency check
    //	     - check diskspace
    // 	     - inform packagemanager (or installation manager)

    NCMIL <<  "OK button pressed - leaving package selection, starting installation" << endl;
    
    // return false, leave the package selection
    return false;
}


///////////////////////////////////////////////////////////////////
//
// showPackageInformation
//
// Shows the package information
//
bool PackageSelector::showPackageInformation ( PMObjectPtr pkgPtr )
{
    if ( !pkgPtr )
    {
	NCERR << "Package not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }
    
    if ( visibleInfo->compare( PkgNames::LongDescr() ) == YO_EQUAL )
    {
	// ask the package manager for the description of this package
	list<string> value = pkgPtr->description();
	string descr = createDescrText( value );

        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(descr) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Files() ) == YO_EQUAL )
    {
	// get the file list from the package manager/show the list
	list<string> fileList = ((PMPackagePtr)pkgPtr)->filenames();
	
	// get the widget id 
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );  
	if ( descrInfo  )
	{
	    NCMIL <<  "Size of the file list: " << fileList.size() << endl;
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(createText(fileList, false)) );
	}
    }
    else if ( visibleInfo->compare( PkgNames::PkgInfo() ) == YO_EQUAL )
    {
	string instVersion = "";
	string version = "";
	NCstring text ( "" );
	
	if ( pkgPtr->hasSelectable() )
	{
	    text += NCstring( pkgPtr->getSelectable()->name() );
	    text += NCstring( " - " );
	}

	text += NCstring( pkgPtr->summary() );		// the summary
	text += NCstring( "<br>" );

	if ( pkgPtr->hasInstalledObj() && pkgPtr->hasCandidateObj() )
	{
	    instVersion = pkgPtr->getInstalledObj()->version();
	    version = pkgPtr->getCandidateObj()->version();
	}
	else
	{
	    version = pkgPtr->version();
	}

	text += NCstring( PkgNames::Version() );
	text += NCstring( version );
	if ( instVersion != "" )
	{
	    text += NCstring( "  " );
	    text += NCstring( PkgNames::InstVersion() );
	    text += NCstring( instVersion );
	}

	text += NCstring( "  " );
	
	// show the size
	text += NCstring( PkgNames::Size() );
	text += NCstring( pkgPtr->size().asString() );
	
	text += NCstring( "<br>" );

	// show Provides:
	text += NCstring( PkgNames::Provides() );
	list<PkgRelation> provides = pkgPtr->provides();	// PMSolvable
	text += NCstring( createRelLine(provides) );
	text += NCstring( "<br>" );

	// show the authors
	text += NCstring( PkgNames::Authors() );
	list<string> authors = ((PMPackagePtr)pkgPtr)->authors(); // PMPackage	
	text += NCstring( createText( authors, true ) );
	
        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( text.YCPstr() );
	}
    }

    NCDBG <<  "Showing package information: " << visibleInfo->toString() << endl;
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// setPackageStatus
//
// Informs the package manager about status change
//
bool PackageSelector::setPackageStatus ( int index, int key )
{

    // FIXME: get the package handle and inform the package manager
    return true;
}

///////////////////////////////////////////////////////////////////
//
// createDescrText
//
string PackageSelector::createDescrText( list<string> value )
{
    string html_text = "";
    
    bool auto_format = true;
    list<string>::const_iterator it = value.begin();

    while ( it != value.end() )
    {
	string line = (*it);

	if ( line.substr( 0, 7 ) ==  "Authors" )
	{
	    line = "<b>" + line + "</b>";
	    auto_format = false;
	}

	if ( auto_format )
	{
	    if ( line.length() == 0 )	// Empty lines mean new paragraph
		html_text += "</p><p>";
	    else
		html_text += line;
	}
	else
	{
	    html_text += line + "<br>";
	}	
	
	++it;
    }

    html_text += "</p>";

    return html_text;
}

///////////////////////////////////////////////////////////////////
//
// createProvides
//
string PackageSelector::createRelLine( list<PkgRelation> info )
{
    list<PkgRelation>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it).asString();
	if ( i < info.size()-1 )
	{
	    text = text + ", ";
	}
    }

    return text;
}


///////////////////////////////////////////////////////////////////
//
// createText
//
string PackageSelector::createText( list<string> info, bool oneline )
{
    list<string>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it);
	if ( i < info.size()-1 )
	{
	    if ( oneline )
	    {
	    text = text + ", ";
	    }
	    else
	    {
		text = text + "<br>";	
	    }
	}
    }

    return text;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PackageSelector::getSelections
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
bool PackageSelector::getSelections( )
{
    InstSrcPtr nsrc;
    unsigned int i;
    
    // FIXME: remove this part (is done before in YCP or ...)
    string media_url = "dir:///";
    string product_dir = "/develop/yast2/source/packagemanager/testsuite/media/";

    system ( "rm -rf /tmp/tcache" );	// remove it first ????????
    
    Pathname cache   ( "/tmp/tcache" ); // cachedir (must not exist)

    PMError err = InstSrc::vconstruct( nsrc,
				       cache,
				       media_url,
				       product_dir,
				       InstSrc::T_UnitedLinux );

    if ( err != InstSrcError::E_ok )
    {
	NCERR << "Failed to enableSource: "<< err << endl;
	return false;
    }
    
    err = nsrc->enableSource();

    if ( err != InstSrcError::E_ok )
    {
	NCERR << "Failed to enableSource" << endl;
	return false;
    }
    // end FIXME: remove this part

    
    // get selections
    const list<PMSelectionPtr> sels = nsrc->getSelections ();

    if ( sels.empty() )
    {
	NCERR << "Selections are NOT available" << endl;
	return false;
    }
    else
    {
	list<PMSelectionPtr>::const_iterator p_it;	
	for ( i = 0, p_it = sels.begin(); p_it != sels.end(); ++p_it, i++ )
	{
	    // show the available add-ons
	    if ( (*p_it)->visible() && !(*p_it)->isBase() )
	    {
		NCMIL << "Add-on " <<  (*p_it)->name() << " available" << endl;
		selections.push_back( (*p_it) );	// store the PMSelectionPtr 
	    }
	}
    }

    return true;
}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PackageSelector::selectionNames
//	METHOD TYPE : list <string>
//
//	DESCRIPTION :
//
vector<string> PackageSelector::selectionNames( )
{
    vector<string> selList;
    vector<PMSelectionPtr>::iterator it = selections.begin();

    while ( it != selections.end() )
    {
	selList.push_back( (*it)->name() );
	++it;
    }

    return selList;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PackageSelector::selectionId
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PackageSelector::selectionId( unsigned int index )
{
    string id  = "";
    if ( index < selections.size() )
    {
	id = selections[index]->name();
    }
    
    return id;
}
