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

   File:       NCPackageSelector.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "NCurses.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCPkgTable.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPkgPopupSearch.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCMenuButton.h"
#include "NCPkgPopupSelection.h"
#include "NCPkgPopupRepo.h"
#include "NCPkgPopupDeps.h"
#include "NCPkgPopupDiskspace.h"
#include "NCPkgPopupTable.h"
#include "NCPkgPopupDescr.h"
#include "NCPackageSelector.h"
#include "YSelectionBox.h"
#include "YNCursesUI.h"
#include "NCi18n.h"

#include <boost/bind.hpp>
#include <fstream>
#include <iomanip>
#include <list>
#include <string>
#include <set>

#include "NCZypp.h"		// tryCastToZyppPkg(), tryCastToZyppPatch()
#include <zypp/ui/Selectable.h>
#include <zypp/ui/PatchContents.h>
#include <zypp/SysContent.h>

typedef zypp::ui::PatchContents			ZyppPatchContents;
typedef zypp::ui::PatchContents::const_iterator	ZyppPatchContentsIterator;
typedef zypp::syscontent::Reader::Entry		ZyppReaderEntry;
typedef std::pair<string, ZyppReaderEntry>	importMapPair; 

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>

#define DEFAULT_EXPORT_FILE_NAME "user-packages.xml"

using namespace std;
using std::string;
using std::wstring;
using std::map;
using std::pair;

/*
  Textdomain "packages"
*/

///////////////////////////////////////////////////////////////////
//
// CompFunc 
//
bool sortByName( ZyppSel ptr1, ZyppSel ptr2 )
{
    if ( ptr1->name() < ptr2->name() )
    {
	return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////
//
// ignore case compare  
//
bool ic_compare ( char c1, char c2 )
{
    return ( toupper( c1 ) == toupper( c2 ) );
}


///////////////////////////////////////////////////////////////////
//
// Constructor
//
NCPackageSelector::NCPackageSelector( YNCursesUI * ui, const YWidgetOpt & opt )
    : y2ui( ui )
      , visibleInfo( YCPNull() )
      , filterPopup( 0 )
      , depsPopup( 0 )
      , selectionPopup( 0 )
      , patternPopup( 0 )
      , languagePopup( 0 )
      , repoPopup( 0 )
      , diskspacePopup( 0 )
      , searchPopup( 0 )
      , youMode( false )
      , updateMode( false )
      , testMode ( false )
      , autoCheck( true )
      , _rpmGroupsTree (0)
{
    // Fill the handler map
    eventHandlerMap[ NCPkgNames::Packages()->toString() ]	= &NCPackageSelector::PackageListHandler;
    eventHandlerMap[ NCPkgNames::Cancel()->toString() ] 	= &NCPackageSelector::CancelHandler;
    eventHandlerMap[ NCPkgNames::OkButton()->toString() ]	= &NCPackageSelector::OkButtonHandler;
    eventHandlerMap[ NCPkgNames::Search()->toString() ] 	= &NCPackageSelector::SearchHandler;
    eventHandlerMap[ NCPkgNames::Diskinfo()->toString() ] = &NCPackageSelector::DiskinfoHandler;
    // Filter menu
    eventHandlerMap[ NCPkgNames::RpmGroups()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Selections()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Patterns()->toString() ] = &NCPackageSelector::FilterHandler;    
    eventHandlerMap[ NCPkgNames::Languages()->toString() ] = &NCPackageSelector::FilterHandler;    
    eventHandlerMap[ NCPkgNames::Repositories()->toString() ] = &NCPackageSelector::FilterHandler;    
    eventHandlerMap[ NCPkgNames::UpdateList()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Installed()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Whatif()->toString() ] = &NCPackageSelector::FilterHandler;

    // YOU filter
    eventHandlerMap[ NCPkgNames::Recommended()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Security()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::Optional()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::InstalledPatches()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::AllPatches()->toString() ] = &NCPackageSelector::FilterHandler; 
    eventHandlerMap[ NCPkgNames::NewPatches()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::InstalledPatches()->toString() ] = &NCPackageSelector::FilterHandler;
    eventHandlerMap[ NCPkgNames::InstallablePatches()->toString() ] = &NCPackageSelector::FilterHandler;    
    eventHandlerMap[ NCPkgNames::YaST2Patches()->toString() ] = &NCPackageSelector::FilterHandler;

    // Information menu
    eventHandlerMap[ NCPkgNames::Files()->toString() ]   	= &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::PkgInfo()->toString() ] 	= &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::LongDescr()->toString() ]= &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::Versions()->toString() ] = &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::Relations()->toString() ] = &NCPackageSelector::InformationHandler;

    // YOU information 
    eventHandlerMap[ NCPkgNames::PatchDescr()->toString() ] = &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::PatchPackages()->toString() ] = &NCPackageSelector::InformationHandler;
    eventHandlerMap[ NCPkgNames::PatchPackagesVersions()->toString() ] = &NCPackageSelector::InformationHandler;

    // Action menu
    eventHandlerMap[ NCPkgNames::Toggle()->toString() ] 	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::Select()->toString() ] 	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::Delete()->toString() ] 	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::Update()->toString() ] 	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::TabooOn()->toString() ]	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::TabooOff()->toString() ]	= &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::SourceYes()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::SourceNo()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::InstallAll()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::DontInstall()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::DeleteAll()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::DontDelete()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::UpdateNewer()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::UpdateAll()->toString() ] = &NCPackageSelector::StatusHandler;
    eventHandlerMap[ NCPkgNames::DontUpdate()->toString() ] = &NCPackageSelector::StatusHandler;
    
    // Etc. menu
    eventHandlerMap[ NCPkgNames::ShowDeps()->toString() ] = &NCPackageSelector::DependencyHandler;
    eventHandlerMap[ NCPkgNames::AutoDeps()->toString() ] = &NCPackageSelector::DependencyHandler;
    eventHandlerMap[ NCPkgNames::VerifySystem()->toString() ] = &NCPackageSelector::DependencyHandler;
    eventHandlerMap[ NCPkgNames::ExportToFile()->toString() ] = &NCPackageSelector::FileHandler;
    eventHandlerMap[ NCPkgNames::ImportFromFile()->toString() ] = &NCPackageSelector::FileHandler;
    eventHandlerMap[ NCPkgNames::Testcase()->toString() ] = &NCPackageSelector::TestcaseHandler;
    
    // Help menu
    eventHandlerMap[ NCPkgNames::GeneralHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::StatusHelp()->toString() ]  = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::FilterHelp()->toString() ]  = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::UpdateHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::SearchHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::PatchHelp()->toString() ]  = &NCPackageSelector::YouHelpHandler;

    if ( opt.youMode.value() )
	youMode = true;

    if ( opt.updateMode.value() )
	updateMode = true;

    // read test source information
    if ( opt.testMode.value() )
	testMode = true;

    saveState ();

    if ( !youMode )
    {
	// create the selections popup
	selectionPopup = new NCPkgPopupSelection( wpos( 1, 1 ), this, NCPkgPopupSelection::S_Selection );
	// create the patterns popup
	patternPopup =  new NCPkgPopupSelection( wpos( 1, 1 ), this, NCPkgPopupSelection::S_Pattern );
	
	_rpmGroupsTree = new YRpmGroupsTree ();
	// get the rpm groups
	ZyppPoolIterator b = zyppPkgBegin ();
	ZyppPoolIterator e = zyppPkgEnd ();
	ZyppPoolIterator i;
	for ( i = b; i != e;  ++i )    
	{
	    ZyppPkg zyppPkg = tryCastToZyppPkg( (*i)->theObj() );
	    if ( zyppPkg )
	    {
		_rpmGroupsTree->addRpmGroup (zyppPkg->group ());
	    }
	}

	// create language popup
	languagePopup = new NCPkgPopupSelection( wpos( 1,1 ), this, NCPkgPopupSelection::S_Language );

        // create repositories popup
        repoPopup = new NCPkgPopupRepo( wpos( 1,1), this );

	// create the filter popup
	filterPopup = new NCPkgPopupTree( wpos( 1, 1 ),  this );	 

    }

    // create the search popup
    searchPopup = new NCPkgPopupSearch( wpos( 1, 1 ), this );
	
    // the dependency popup
    depsPopup = new NCPkgPopupDeps( wpos( 3, 8 ), this );

    // the disk space popup
    diskspacePopup = new NCPkgPopupDiskspace( wpos( (NCurses::lines() - 15)/2, NCurses::cols()/6  ), testMode );

}

///////////////////////////////////////////////////////////////////
//
// Destructor
//
NCPackageSelector::~NCPackageSelector()
{
    if ( filterPopup )
    {
	delete filterPopup;
    }
    if ( selectionPopup )
    {
	delete selectionPopup;
    }
    if ( patternPopup )
    {
	delete patternPopup;
    }
    if ( languagePopup )
    {
	delete languagePopup;
    }
    if ( repoPopup )
    {
	delete repoPopup;
    }
    if ( depsPopup )
    {
	delete depsPopup;	
    }
    if ( diskspacePopup )
    {
	delete diskspacePopup;
    }
    if ( searchPopup )
    {
	delete searchPopup;
    }
    if ( _rpmGroupsTree )
    {
	delete _rpmGroupsTree;
    }
}

void NCPackageSelector::createFilterMenu()
{
    bool selections;
    bool patterns;
	
    // create the pattern popup	
    if ( ! zyppPool().empty<zypp::Pattern>() )
    {

	patterns = true;
    }
    else
    {
	patterns = false;
    }
    // create the selections popup
    if ( ! zyppPool().empty<zypp::Selection>() )
    {

	selections = true;
    }
    else
    {
	selections = false;
    }
	
    if ( selections && !patterns )
    {
	// ReplaceWidget and show menu entry Selections instead of Patterns
	char menu[4000];
	snprintf ( menu, sizeof(menu) - 1,
		  "`MenuButton( `opt(`key_F4), \"%s\", "
		  "[`item( `id(\"groups\"), \"%s\" ), `item( `id(\"selections\"), \"%s\" ), "
		  " `item( `id(\"search\"), \"%s\" ), `item( `id(\"installed\"), \"%s\" ), "
		  " `item( `id(\"whatif\"), \"%s\" ), `item( `id(\"updatelist\"), \"%s\" ) ] ) ",
		  NCPkgNames::MenuFilter().c_str(),
		  NCPkgNames::MenuEntryRPMGroups().c_str(),
		  NCPkgNames::MenuEntrySelections().c_str(),
		  NCPkgNames::MenuEntrySearch().c_str(),
		  NCPkgNames::MenuEntryInstPkg().c_str() ,
		  NCPkgNames::MenuEntryInstSummary().c_str(),
		  NCPkgNames::MenuEntryUpdateList().c_str()
		  );

	Parser parser( menu );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();
	if ( !layout.isNull() )
	    y2ui->evaluateReplaceWidget( YCPSymbol("replacefilter"), layout->asTerm() ); 

    }
    else if ( patterns && selections )
    {
	// ReplaceWidget and show menu entries Patterns AND Selections
	char menu[4000];
	snprintf ( menu, sizeof(menu) - 1,
		  "`MenuButton(`opt(`key_F4), \"%s\", "
		  "[`item( `id(\"groups\"), \"%s\" ), `item( `id(\"patterns\"), \"%s\" ), "
		  " `item( `id(\"selections\"), \"%s\" ), "
		  " `item( `id(\"search\"), \"%s\" ), `item( `id(\"installed\"), \"%s\" ), "
		  " `item( `id(\"whatif\"), \"%s\" ), `item( `id(\"updatelist\"), \"%s\" ) ] ) ",
		  NCPkgNames::MenuFilter().c_str(),
		  NCPkgNames::MenuEntryRPMGroups().c_str(),
		  NCPkgNames::MenuEntryPatterns().c_str(),
		  NCPkgNames::MenuEntrySelections().c_str(),
		  NCPkgNames::MenuEntrySearch().c_str(),
		  NCPkgNames::MenuEntryInstPkg().c_str() ,
		  NCPkgNames::MenuEntryInstSummary().c_str(),
		  NCPkgNames::MenuEntryUpdateList().c_str()
		  );

	Parser parser( menu );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();
	if ( !layout.isNull() )
	    y2ui->evaluateReplaceWidget( YCPSymbol("replacefilter"), layout->asTerm() ); 

    }
}

///////////////////////////////////////////////////////////////////
//
// detection whether the user has made any changes
//
 
void NCPackageSelector::saveState ()
{
    ZyppPool p = zyppPool ();

    p.saveState<zypp::Package> ();
    p.saveState<zypp::SrcPackage> ();
    
    p.saveState<zypp::Patch> ();
    // some future proofing
    p.saveState<zypp::Message> ();
    p.saveState<zypp::Script> ();

    p.saveState<zypp::Selection> ();
    p.saveState<zypp::Pattern> ();
    p.saveState<zypp::Language> ();
}

void NCPackageSelector::restoreState ()
{
    ZyppPool p = zyppPool ();

    p.restoreState<zypp::Package> ();
    p.restoreState<zypp::SrcPackage> ();
    
    p.restoreState<zypp::Patch> ();
    // some future proofing
    p.restoreState<zypp::Message> ();
    p.restoreState<zypp::Script> ();
    
    p.restoreState<zypp::Selection> ();
    p.restoreState<zypp::Pattern> ();
    p.restoreState<zypp::Language> ();
}

bool NCPackageSelector::diffState ()
{
    ZyppPool p = zyppPool ();

    bool diff = false;

    ostream & log = UIMIL;
    log << "diffState" << endl;
    diff = diff || p.diffState<zypp::Package> ();
    log << diff << endl;
    diff = diff || p.diffState<zypp::SrcPackage> ();
    log << diff << endl;
    
    diff = diff || p.diffState<zypp::Patch> ();
    log << diff << endl;
    // some future proofing
    diff = diff || p.diffState<zypp::Message> ();
    log << diff << endl;
    diff = diff || p.diffState<zypp::Script> ();
    log << diff << endl;
    
    diff = diff || p.diffState<zypp::Selection> ();
    log << diff << endl;
    diff = diff || p.diffState<zypp::Pattern> ();
    log << diff << endl;
    diff = diff || p.diffState<zypp::Language> ();
    log << diff << endl;
    return diff;
}

///////////////////////////////////////////////////////////////////
//
// handleEvent
//
// Handle event, calls corresponding handler-function
//
bool NCPackageSelector::handleEvent ( const NCursesEvent&   event )
{
    bool retVal = false;
    YCPValue currentId = YCPNull();

    if ( event == NCursesEvent::handled )
	return false;
    
    // Get the id of the widget which is affected
    if ( event == NCursesEvent::button )
    {
	currentId =  dynamic_cast<YWidget *>(event.widget)->id();
    }
    else if ( event == NCursesEvent::menu )
    {
	currentId =  event.selection;
    }
    
    // Find the handler-function for the given widget-nameId
    if ( ! currentId.isNull() )
    {
	UIMIL <<  "Selected widget id: " << currentId->toString() << endl;
	// hyperlink 
        if ( currentId->isString()
	     && currentId->asString()->value().substr(0, 4) == "pkg:" )
	{
	    LinkHandler( currentId->asString()->value() );
	    return true;
	}
	
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
void NCPackageSelector::setVisibleInfo( const YCPValue & info )
{
    visibleInfo = info;
}

///////////////////////////////////////////////////////////////////
//
// fillAvailableList
//
// Fills the package table (on bottom ) with the list of available packages 
//
bool NCPackageSelector::fillAvailableList( NCPkgTable * pkgTable, ZyppSel selectable )
{
    bool addInstalled = true;
    
    if ( !pkgTable )
    {
	NCERR << "No table widget for available packages existing" << endl;
	return false;
    }

    if ( !selectable )
    {
	NCERR << "Package pointer not valid" << endl;
	return false;
    }

    // clear the package table
    pkgTable->itemsCleared ();

    NCDBG << "Number of available packages: " << selectable->availableObjs() << endl;

    // show all availables
    zypp::ui::Selectable::available_iterator
	b = selectable->availableBegin (),
	e = selectable->availableEnd (),
	it;
    for (it = b; it != e; ++it)
    {
	if ( selectable->installedObj() &&
	     selectable->installedObj()->edition() == (*it)->edition() &&
	     selectable->installedObj()->arch()    == (*it)->arch()      )
	    // FIXME: In future releases, also the vendor will make a difference
	{
	    addInstalled = false;
	}
	pkgTable->createListEntry( tryCastToZyppPkg (*it), selectable );
    }
    if ( selectable->hasInstalledObj() && addInstalled )
    {
	pkgTable->createListEntry( tryCastToZyppPkg (selectable->installedObj()), selectable );
    }
    
    // show the package list
    pkgTable->drawList();
    
    if ( pkgTable->getNumLines() > 0 )
    {
	pkgTable->setCurrentItem( 0 );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// showSelPackages
//
// Fills the package table with the list of packages of the given selection
//
bool NCPackageSelector::showSelPackages( const YCPString & label, const set<string> & wanted )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();
    
    set<string>::iterator not_found = wanted.end ();

    std::vector<ZyppSel> sorted;
    sorted.reserve (wanted.size ());

    // find the objects for the names
    ZyppPoolIterator
	b = zyppPkgBegin(),
	e = zyppPkgEnd(),
	it;
    for ( it = b; it != e; ++it )
    {
	string name = (*it)->name(); // omitted theObj

	if ( wanted.find (name) != not_found )
	{
	    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
	    if ( zyppPkg )
	    {
		sorted.push_back (*it);
	    }
	}
    }

    // sort it and insert it to the list
    sort (sorted.begin (), sorted.end (), sortByName);
    {
	std::vector<ZyppSel>::iterator
	    b = sorted.begin (),
	    e = sorted.end (),
	    it;
	for ( it = b; it != e; ++it )
	{
	    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
	    // already know that it is valid
	    packageList->createListEntry( zyppPkg, *it );
	}
    }

    // show the package table
    packageList->drawList();
    
    if ( !label.isNull() )
    {
	NCDBG << "Filter: " << label->toString() << endl;

        // show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }
   
    return true;

}

///////////////////////////////////////////////////////////////////
//
// fillSearchList
//
// Fills the package table
//
bool NCPackageSelector::fillSearchList( const YCPString & expr,
				      bool ignoreCase,
				      bool checkName,
				      bool checkSummary,
				      bool checkDescr,
				      bool checkProvides,
				      bool checkRequires )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList
	 || expr.isNull() )
    {
	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

// FIXME filter it inside zypp
    ZyppPoolIterator b = zyppPkgBegin ();
    ZyppPoolIterator e = zyppPkgEnd ();
    ZyppPoolIterator i;

    // get the package list and sort it
    list<ZyppSel> pkgList( b, e );
    pkgList.sort( sortByName );

    // fill the package table
    list<ZyppSel>::iterator listIt = pkgList.begin();
    ZyppPkg pkg;
    string description = "";
    string provides = "";
    string requires = "";
    
    while ( listIt != pkgList.end() )
    {
	if ( (*listIt)->installedObj() )
	   pkg = tryCastToZyppPkg ((*listIt)->installedObj());
	else
	   pkg = tryCastToZyppPkg ((*listIt)->theObj());

	if ( pkg )
	{
	    if ( checkDescr )
	    {
		zypp::Text value = pkg->description();
		description = createDescrText( value );    
	    }
	    if ( checkProvides )
	    {
		zypp::CapSet value = pkg->dep (zypp::Dep::PROVIDES);
		provides = createRelLine( value );
	    }
	    if ( checkRequires )
	    {
		zypp::CapSet value = pkg->dep (zypp::Dep::REQUIRES);
		requires = createRelLine( value );    
	    }
	    if ( ( checkName && match( pkg->name(), expr->value(), ignoreCase )) ||
		 ( checkSummary && match( pkg->summary(), expr->value(), ignoreCase) ) ||
		 ( checkDescr && match( description, expr->value(), ignoreCase) ) ||
		 ( checkProvides && match( provides, expr->value(), ignoreCase) ) ||
		 ( checkRequires && match( requires,  expr->value(), ignoreCase) )
		 )
	    {
		// search sucessful
		packageList->createListEntry( pkg, *listIt );
	    }
	}	

	++listIt;
    }

    // show the package list
    packageList->drawList();
    
    // set filter label to 'Search'
    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::SearchResults()) );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchSearchList
//
// Fills the patch list with search results
//
bool NCPackageSelector::fillPatchSearchList( const YCPString & expr )
{
   NCPkgTable * packageList = getPackageList();
    
    if ( !packageList
       || expr.isNull() )
    {
      return false;
    }

    // clear the patch list
    packageList->itemsCleared ();
    // get the patch list and sort it
    list<ZyppSel> patchList( zyppPatchesBegin (), zyppPatchesEnd () );
    patchList.sort( sortByName );
    list<ZyppSel>::iterator listIt = patchList.begin(); 

    while ( listIt != patchList.end() )
    {
      ZyppPatch patchPtr  = tryCastToZyppPatch( ( *listIt)->theObj() );

      if ( patchPtr )
      {
          if ( match( (*listIt)->name(), expr->value(), true ) )
          {
              // search sucessful
              packageList->createPatchEntry( patchPtr, *listIt );
          }
      }
      ++listIt;
    }
    // show the patch list with search result
    packageList->drawList();
    
    // set filter label to 'Search'
    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::SearchResults()) );
    }

    return true;
}


///////////////////////////////////////////////////////////////////
//
// fillPatchList
//
// Fills the package table with the list of YOU patches
//
bool NCPackageSelector::fillPatchList( string filter )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear list of patches
    packageList->itemsCleared ();

    // get the patch list and sort it
    list<ZyppSel> patchList( zyppPatchesBegin (), zyppPatchesEnd () );
    patchList.sort( sortByName );
    list<ZyppSel>::iterator listIt = patchList.begin(); 

    while ( listIt != patchList.end() )
    {
	ZyppPatch patchPtr  = tryCastToZyppPatch( ( *listIt)->theObj() );

	if ( patchPtr )
	{
	    checkPatch( patchPtr, *listIt, filter );
	}
	++listIt;		
    }

    if ( filter == "installable"
	 && packageList->getNumLines() == 0 )
    {
	packageList->createInfoEntry( NCPkgNames::NoPatches() );	
    }

    // show the patches
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    if ( filterLabel )
    {
	if ( filter == "installable" )
	{
	    // show common label "Online Update Patches"
	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::YOUPatches()) );
	}
	else if ( filter == "installed" )
	{
  	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::InstPatches()) );  
	}
	else
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::Patches()) );  
	}
    }
    
    return true;
}


///////////////////////////////////////////////////////////////////
//
// fillUpdateList
//
//
bool NCPackageSelector::fillUpdateList( )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    list<zypp::PoolItem_Ref> problemList = zypp::getZYpp()->resolver()->problematicUpdateItems();
    
    for ( list<zypp::PoolItem_Ref>::const_iterator it = problemList.begin();
	  it != problemList.end();
	  ++it )
    {
	ZyppPkg pkg = tryCastToZyppPkg( (*it).resolvable() );

	if ( pkg )
	{
	    ZyppSel slb = selMapper.findZyppSel( pkg );

	    if ( slb )
	    {
		NCMIL << "Problematic package: " <<  pkg->name().c_str() << " " <<
		    pkg->edition().asString().c_str() << endl;
		packageList->createListEntry( pkg, slb );
	    }
	}
	
    }
    
    // show the list
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(NCPkgNames::UpdateProblem()) );
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchPackages
//
//
bool NCPackageSelector::fillPatchPackages ( NCPkgTable * pkgTable, ZyppObj objPtr, bool versions )
{
    if ( !pkgTable || !objPtr )
	return false;

    pkgTable->itemsCleared ();

    std::set<ZyppSel> patchSelectables;
    ZyppPatch patchPtr  = tryCastToZyppPatch( objPtr ); 
    
    if ( !patchPtr )
	return false;

    ZyppPatchContents patchContents( patchPtr );

    zypp::Patch::AtomList atomList = patchPtr->atoms();
    NCMIL <<  "Filtering for patch: " << patchPtr->name().c_str() << " number of atoms: " << atomList.size() << endl ;

    for ( ZyppPatchContentsIterator it = patchContents.begin();
	  it != patchContents.end();
	  ++it )
    {
	ZyppPkg pkg = tryCastToZyppPkg( *it );

	if ( pkg )
	{
	    NCMIL << "Patch package found: " <<  (*it)->name().c_str() << endl;
	    ZyppSel sel = selMapper.findZyppSel( pkg );

	    if ( sel )
	    {
		if ( contains( patchSelectables, sel ) )
		{
		    NCMIL << "Suppressing duplicate selectable: " << (*it)->name().c_str() << "-" <<
			(*it)->edition().asString().c_str() << " " <<
			(*it)->arch().asString().c_str() << endl;
		}
		else
		{
		    patchSelectables.insert( sel );
		    NCDBG << (*it)->name().c_str() << ": Version: " <<  pkg->edition().asString() << endl;

		    pkgTable->createListEntry( pkg, sel );

		    if ( versions )	// additionally show all availables
		    {
			zypp::ui::Selectable::available_iterator
			    b = sel->availableBegin (),
			    e = sel->availableEnd (),
			    it;
			for (it = b; it != e; ++it)
			{
			    ZyppPkg pkgAvail =  tryCastToZyppPkg (*it);
			    if ( pkgAvail )
			    {
				if ( pkg->edition() != pkgAvail->edition() ||
				     pkg->arch() != pkgAvail->arch() )
				{
				    pkgTable->createListEntry( pkgAvail, sel );
				}
			    }
			}

		    } // if ( versions )
		}
	    }
	}
	else  // No ZyppPkg - some other kind of object (script, message)
	{
	    if ( zypp::isKind<zypp::Script> ( *it ) )
	    {
		vector<string> pkgLine;
		pkgLine.reserve(4);

		pkgLine.push_back( (*it)->name() );
		pkgLine.push_back( "   " );	// versions empty
		pkgLine.push_back( "   " );	
		pkgLine.push_back( NCPkgNames::Script() );
	
		pkgTable->addLine( S_NoInst,
				   pkgLine,
				   ZyppObj(),
				   ZyppSel()
				   );
	    }
	    else
	    {
		y2debug( "Found unknown atom of kind %s: %s",
			 (*it)->kind().asString().c_str(),
			 (*it)->name().c_str() );
	    }
	}
    }

    // show the list
    pkgTable->drawList();
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillSummaryList
//
// Shows the installation summary or lists all installed packages.
//
bool NCPackageSelector::fillSummaryList( NCPkgTable::NCPkgTableListType type )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    // get the package list and sort it
    list<ZyppSel> pkgList( zyppPkgBegin (), zyppPkgEnd () );
    pkgList.sort( sortByName );

    // fill the package table
    list<ZyppSel>::iterator listIt;
    ZyppPkg pkgPtr;

    // If the dependency check is off, the dependencies will not be solved for 
    // the installation summary.
    // This is not necessary because the dependencies will be solved and the
    // "Automatic Changes" list will be shown if the OK button is pressed. 
    if ( !autoCheck )
    {
	// showPackageDependencies( true );
    }
    
    for ( listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt )
    {
	ZyppSel selectable = *listIt;
	ZyppPkg pkg = tryCastToZyppPkg (selectable->theObj ());
	// show all matching packages 
	switch ( type )
	{
	    case NCPkgTable::L_Changes: {
		if ( selectable->status() != S_NoInst
		     && selectable->status() != S_KeepInstalled )  
		{
		    packageList->createListEntry( pkg, selectable );
		}
		break;
	    }
	    case NCPkgTable::L_Installed: {
		if ( selectable->status() == S_KeepInstalled ) 
		{
		    packageList->createListEntry( pkg, selectable );
		}
		break;
	    }
	    default:
		break;
	}
    }

    // show the package list
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( NCPkgNames::InstSummary() );
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
bool NCPackageSelector::fillPackageList( const YCPString & label, YStringTreeItem * rpmGroup )
{
     NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    // get the package list and sort it
    list<ZyppSel> pkgList( zyppPkgBegin (), zyppPkgEnd () );
    pkgList.sort( sortByName );

    // fill the package table
    list<ZyppSel>::iterator listIt;
    ZyppPkg pkgPtr;


    for ( listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt )
    {
	ZyppSel selectable = *listIt;
	    
	// Multiple instances of this package may or may not be in the same
	// RPM group, so let's check both the installed version (if there
	// is any) and the candidate version.
	//
	// Make sure we emit only one filterMatch() signal if both exist
	// and both are in the same RPM group. We don't want multiple list
	// entries for the same package!
	    
	bool match =
	    checkPackage( selectable->candidateObj(), selectable, rpmGroup ) ||
	    checkPackage( selectable->installedObj(), selectable, rpmGroup ); 

	// If there is neither an installed nor a candidate package, check
	// any other instance.  
	    
	if ( ! match			&&
	     ! selectable->installedObj()	&&
	     ! selectable->candidateObj()     )
	    checkPackage( selectable->theObj(), selectable, rpmGroup );

    }

    // show the package list
    packageList->drawList();
    
    if ( !label.isNull() )
    {
	// show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillRepoFilterList
//
// Fills the package table with packages from selected repository
//

bool NCPackageSelector::fillRepoFilterList( ZyppRepo repo) 
{
    NCMIL << "Collecting packages in selected repository" << endl;

    NCPkgTable *pkgList = getPackageList();
    //clean the pkg table first
    pkgList->itemsCleared ();

    //sets to store matching packages
    set <ZyppSel> exactMatch;
    set <ZyppSel> nearMatch;
    
    //iterate through the package pool
    for ( ZyppPoolIterator it = zyppPkgBegin();
          it != zyppPkgEnd();
          ++it )
    {
	//we have candidate object in this repository
        if ( (*it)->candidateObj() &&
             (*it)->candidateObj()->repository() == repo )
        {
            exactMatch.insert( *it );
        }
	//something else (?)
        else
        {
            zypp::ui::Selectable::available_iterator pkg_it = (*it)->availableBegin();
    
            while ( pkg_it != (*it)->availableEnd() )
            {
                if ( (*pkg_it)->repository() == repo )
                    nearMatch.insert( *it );
    
                ++pkg_it;
            }
        }
    
      }

    //finally create pkg table list entries
    set<ZyppSel>::const_iterator e_it = exactMatch.begin();
    while ( e_it != exactMatch.end() )
    {
        ZyppPkg pkg = tryCastToZyppPkg( (*e_it)->theObj() );
        pkgList->createListEntry ( pkg, *e_it);
	e_it++;
    }
    
    set<ZyppSel>::const_iterator n_it = nearMatch.begin();
    while ( n_it != nearMatch.end() )
    {
        ZyppPkg pkg = tryCastToZyppPkg( (*n_it)->theObj() );
        pkgList->createListEntry ( pkg, *n_it);
	n_it++;
    }

    //and show the whole stuff to the user
    pkgList->drawList();

    YWidget * filterLabel = y2ui->widgetWithId( NCPkgNames::Filter(), true );
    
    if ( repoPopup && filterLabel )
    {
	ZyppProduct product = repoPopup->findProductForRepo( repo );
	if ( product )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( product->summary() ); 
	}
    }
    
    return true;

}

///////////////////////////////////////////////////////////////////
//
// match
//

bool NCPackageSelector::match ( string s1, string s2, bool ignoreCase )
{
    string::iterator pos;
    
    if ( ignoreCase )
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end(),
		      ic_compare );
    }
    else
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end() );
    }

    return ( pos != s1.end() );
}


///////////////////////////////////////////////////////////////////
//
// checkPackage
//
//
bool NCPackageSelector::checkPackage( ZyppObj opkg, ZyppSel slb,
				    YStringTreeItem * rpmGroup )
{
    ZyppPkg pkg = tryCastToZyppPkg (opkg);
    if ( ! pkg || ! rpmGroup )
	return false;

    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }
    
    string group_str = _rpmGroupsTree->rpmGroup (rpmGroup);
    // is the requested rpm group a prefix of this package's group?
    if ( pkg->group ().find (group_str) == 0 )
    {
	packageList->createListEntry( pkg, slb );
	
	return true;
    }
    else
    {
	return false;
    }
}


// patches

///////////////////////////////////////////////////////////////////
//
// checkPatch
//
//
bool NCPackageSelector::checkPatch( ZyppPatch 	patchPtr,
				  ZyppSel	selectable,
				  string 	filter )
				   
{
    NCPkgTable * packageList = getPackageList();
    bool displayPatch = false;
    
    if ( !packageList || !patchPtr
	 || !selectable )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    if ( filter == "all" )
    {
	displayPatch = true;
    }
    else if ( filter == "installed" )
    {
	if ( selectable->hasInstalledObj() )
		displayPatch = true;
    }
    else if ( filter == "installable" )
    {
	if ( selectable->hasInstalledObj() ) // patch installed?
	{
	    // display only if broken
	    if ( selectable->installedPoolItem().status().isIncomplete() )
	    {
		displayPatch = true;
		NCWAR << "Installed patch is broken: " << patchPtr->name().c_str() << " - "
		      << patchPtr->summary().c_str() << endl;
	    }
	}
	else // patch not installed

	{ 
	    if (selectable->hasCandidateObj() && 
		selectable->candidatePoolItem().status().isSatisfied() )
	    {
		//patch not installed, but it is satisfied (updated to the version patch requires)
		//all that is missing are patch metadata, so let's display the patch
		
		displayPatch = true;

		NCMIL << "Patch satisfied, but not installed yet: " << patchPtr->name().c_str() << " - "
		      << patchPtr->summary().c_str() << endl;
	    }
	}

	if (selectable->hasCandidateObj()) {

            // isNeeded(): this patch is relevant (contains updates for installed packages)
	    // isSatisfied(): all packages are installed, therefore the isNeeded() flag
	    // isn't set. BUT the patch meta data aren't installed and therefore it makes
	    // sense to install the patch
	    if ( selectable->candidatePoolItem().status().isNeeded()) 
	    {
		displayPatch = true;
	    }
	    else
	    {
		NCMIL << "Patch not needed: " << patchPtr->name().c_str() << " - "
		      << patchPtr->summary().c_str() << endl;
	    }
	}
    }
    else if ( filter == "new" )
    {
	    if ( !selectable->hasInstalledObj() )
		displayPatch = true;
    }
    else if ( filter == "security" )
    {
	    if ( patchPtr->category() == "security" )
    		displayPatch = true;
    }

    else if ( filter == "recommended" )
    {
	    if ( patchPtr->category() == "recommended" )
		displayPatch = true;
    }
    else if ( filter == "optional" )
    {
	    if (  patchPtr->category() == "optional" )
			displayPatch = true;
    }
    else if ( filter == "YaST2" )
    {
	    if ( patchPtr->category() == "yast" )
			displayPatch = true;
    }
    
    if ( displayPatch )
	packageList->createPatchEntry( patchPtr, selectable );

    return displayPatch;
}

void NCPackageSelector::importSelectable( ZyppSel selectable, bool isWanted, const char* kind )
{
    ZyppStatus oldStatus = selectable->status();
    ZyppStatus newStatus = oldStatus;

    //Package/Pattern is on the list
    if (isWanted)
    {
	switch (oldStatus)
	{
	    //Keep status for installed ones
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:
	    case S_KeepInstalled:
	    case S_Protected:
		newStatus = oldStatus;
		break;

	    //Keep also those marked for deletion 
	    case S_Del:
	    case S_AutoDel:
		newStatus = S_KeepInstalled;
		NCDBG << "Keeping " << kind << " " << selectable->name().c_str() << endl;
		break;
	    
	    //Add not yet installed pkgs (if they have candidate available)
	    case S_NoInst:
	    case S_Taboo:
		if ( selectable->hasCandidateObj() )
		{
		    newStatus = S_Install;
		    NCDBG << "Adding " << kind << " " << selectable->name().c_str() << endl;
		}
		else
		{
		    NCDBG << "Cannot add " << kind << " " << selectable->name().c_str() << 
			" " << " - no candidate." << endl;	
		}
		break;	
	} 
    }	
    //Package/Pattern is not on the list
    else
    {
	switch (oldStatus)
	{
	    //Mark installed ones for deletion
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:
	    case S_KeepInstalled:
	    case S_Protected:
		newStatus = S_Del;
		NCDBG << "Deleting " << kind << " " << selectable->name().c_str() << endl;
		break;

            //Keep status for not installed, taboo and to-be-deleted	
	    case S_Del:
	    case S_AutoDel:
	    case S_NoInst:
	    case S_Taboo:
		newStatus = oldStatus;
		break;
	}
    }
 
    if (oldStatus != newStatus) 
	selectable->set_status( newStatus );
}

///////////////////////////////////////////////////////////////////
//
// SearchHandler
//
// Opens popup for package search and asks the package manager
//
bool NCPackageSelector::SearchHandler( const NCursesEvent& event)
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList || !searchPopup )
    {
	return false;
    }
    
    // open the search popup
    NCursesEvent retEvent = searchPopup->showSearchPopup();

    if ( retEvent == NCursesEvent::button )
    {
	NCMIL << "Searching for: " <<  (!retEvent.result.isNull()?retEvent.result->toString():"") << endl;
	packageList->showInformation( );
    }
    else
    {
	NCMIL << "Search is canceled"  << endl;
    }

    packageList->setKeyboardFocus();
    
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
bool NCPackageSelector::InformationHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();

    if ( !packageList
	 || event.selection.isNull()
	 || visibleInfo.isNull() )
    {
	NCERR << "*** InformationHandler RETURN false ***" << endl;
	return false;
    }
        
    if ( visibleInfo->compare( event.selection ) == YO_EQUAL )
    {
	// information selection has not changed
	return true;
    }

    // set visibleInfo
    visibleInfo = event.selection;

    if ( visibleInfo->compare( NCPkgNames::Versions() ) == YO_EQUAL )
    {
	// show the package table
	const char * tableLayout = "`PkgSpecial( `id(\"availpkgs\"), `opt(`notify), \"pkgTable\" )"; 
	Parser parser( tableLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();
	if (!layout.isNull() )
	    y2ui->evaluateReplaceWidget( YCPSymbol("replaceinfo"), layout->asTerm() );

	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::AvailPkgs(), true));

	if ( pkgAvail )
	{
	    // set the connection to the NCPackageSelector !!!!
	    pkgAvail->setPackager( this );
	    // set status strategy
	    NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	    pkgAvail->setTableType( NCPkgTable::T_Availables, strategy );

	    pkgAvail->fillHeader( );
	    fillAvailableList( pkgAvail, packageList->getSelPointer( packageList->getCurrentItem() ) );
	}
    }
// patches
    else if ( visibleInfo->compare( NCPkgNames::PatchPackages() ) == YO_EQUAL )
    {
        // show the package table
	const char * tableLayout = "`PkgSpecial( `id(\"patchpkgs\"), `opt(`notify), \"pkgTable\" )"; 
	Parser parser( tableLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();
	if (!layout.isNull())
	    y2ui->evaluateReplaceWidget( YCPSymbol ("replaceinfo"), layout->asTerm() );

	NCPkgTable * patchPkgs = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::PatchPkgs(), true));

	if ( patchPkgs )
	{
	    // set the connection to the NCPackageSelector !!!!
	    patchPkgs->setPackager( this );
	    // set status strategy - don't set extra strategy, use 'normal' package strategy
	    NCPkgStatusStrategy * strategy = new PackageStatStrategy();
	    patchPkgs->setTableType( NCPkgTable::T_PatchPkgs, strategy );

	    patchPkgs->fillHeader( );
	    fillPatchPackages( patchPkgs, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}	
    }
    else if  ( visibleInfo->compare( NCPkgNames::PatchPackagesVersions() ) == YO_EQUAL )
    {
        // show the package table
	const char * tableLayout = "`PkgSpecial( `id(\"pkgsversions\"), `opt(`notify), \"pkgTable\" )"; 
	Parser parser( tableLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();
	if (!layout.isNull())
	    y2ui->evaluateReplaceWidget( YCPSymbol ("replaceinfo"), layout->asTerm() );

	NCPkgTable * patchPkgsVersions = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::PatchPkgsVersions(), true));

	if ( patchPkgsVersions )
	{
	    // set the connection to the NCPackageSelector !!!!
	    patchPkgsVersions->setPackager( this );
	    // set status strategy
	    NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	    patchPkgsVersions->setTableType( NCPkgTable::T_Availables, strategy );

	    patchPkgsVersions->fillHeader( );
	    fillPatchPackages( patchPkgsVersions, packageList->getDataPointer( packageList->getCurrentItem() ), true );
	}	
    }	
    else
    {
	// show the rich text widget
	const char * textLayout = "`RichText( `id(\"description\"), \" \")"; 
	Parser parser( textLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate ();
	if (!layout.isNull())
	    y2ui->evaluateReplaceWidget( YCPSymbol ("replaceinfo"), layout->asTerm() );
    
	packageList->showInformation( );
    }

    packageList->setKeyboardFocus();
    
    UIMIL << "Change package info to: " << visibleInfo->toString() << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
//
// Dependency Handler
//
//
bool NCPackageSelector::DependencyHandler( const NCursesEvent&  event )
{
    if ( event.selection.isNull() )
    {
	return false;
    }

    NCPopupInfo info( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-35)/2 ),  YCPString( "" ),
		      YCPString(_( "All package dependencies are OK." )),
		      NCPkgNames::OKLabel() );
    info.setNiceSize( 35, 5 );
    
    if ( event.selection->compare( NCPkgNames::ShowDeps() ) == YO_EQUAL )
    {
	bool ok = false;

	if ( depsPopup )
	{
	    NCMIL << "Checking dependencies" << endl;
	    depsPopup->showDependencies( NCPkgPopupDeps::S_Solve, &ok );
	}

	if ( ok )		
	    info.showInfoPopup();

	// update the package list and the disk space info
	updatePackageList();
	showDiskSpace();	
    }
    else if ( event.selection->compare( NCPkgNames::VerifySystem() ) == YO_EQUAL )
    {
	verifyPackageDependencies();
	updatePackageList();
	showDiskSpace();
    }
    else if ( event.selection->compare( NCPkgNames::AutoDeps() ) == YO_EQUAL )
    {
	char menu[2000];
	
	if ( autoCheck )
	{
	    snprintf ( menu, sizeof(menu) - 1,
		      "`MenuButton( \"%s\", ["
		       "`menu( \"%s\", [`item( `id(\"showdeps\"), \"%s\" ), `item( `id(\"autodeps\"), \"%s\" ), `item ( `id(\"verifysystem\"), \"%s\" ) ] ),"
		       "`item( `id(\"testcase\"), \"%s\" )"
		      ","
		      "`menu( \"%s\", [`item( `id(\"export\"), \"%s\" ), `item( `id(\"import\"), \"%s\" ) ] )"
		      "] )",
		      NCPkgNames::MenuEtc().c_str(),
		      NCPkgNames::MenuDeps().c_str(),
		      NCPkgNames::MenuCheckDeps().c_str(),
		      NCPkgNames::MenuNoAutoDeps().c_str(),
		      NCPkgNames::MenuVerifySystem().c_str(),
		      NCPkgNames::MenuTestCase().c_str(),
		      NCPkgNames::MenuList().c_str(),
		      NCPkgNames::MenuExportList().c_str(),
		      NCPkgNames::MenuImportList().c_str()
		);


	    Parser parser( menu );
	    YCodePtr parsed_code = parser.parse ();
	    YCPValue layout = YCPNull ();
	    
	    if ( parsed_code != NULL )
		layout = parsed_code->evaluate();

	    if ( !layout.isNull() )
	    {
		y2ui->evaluateReplaceWidget( YCPSymbol ("replacemenu"), layout->asTerm() );
		autoCheck = false;
	    }
	}
	else
	{
	    snprintf ( menu, sizeof(menu) - 1,
		      "`MenuButton( \"%s\", ["
		      "`menu( \"%s\", [`item( `id(\"showdeps\"), \"%s\" ), `item( `id(\"autodeps\"), \"%s\" ), `item ( `id(\"verifysystem\"), \"%s\" ) ] ),"
		     "`item( `id(\"testcase\"), \"%s\" )"  
		      ","
		      "`menu( \"%s\", [`item( `id(\"export\"), \"%s\" ), `item( `id(\"import\"), \"%s\" ) ] )"
		      "] )",
		      NCPkgNames::MenuEtc().c_str(),
		      NCPkgNames::MenuDeps().c_str(),
		      NCPkgNames::MenuCheckDeps().c_str(),
		      NCPkgNames::MenuAutoDeps().c_str(),
		      NCPkgNames::MenuVerifySystem().c_str(),
		      NCPkgNames::MenuTestCase().c_str(),
		      NCPkgNames::MenuList().c_str(),
		      NCPkgNames::MenuExportList().c_str(),
		      NCPkgNames::MenuImportList().c_str()
		);


	    Parser parser( menu );
	    YCodePtr parsed_code = parser.parse ();
	    YCPValue layout = YCPNull ();
	    
	    if ( parsed_code != NULL )
		layout = parsed_code->evaluate();

	    if ( !layout.isNull() )
	    {
		y2ui->evaluateReplaceWidget( YCPSymbol ("replacemenu"), layout->asTerm() );	
		autoCheck = true;
	    }
	}
    }

    NCPkgTable * packageList = getPackageList();
    
    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// FilterHandler
//
// Opens the popup with rpm group tree, selections ... and display the
// corresponding package list
//
bool NCPackageSelector::FilterHandler( const NCursesEvent&  event )
{
    NCursesEvent retEvent;
    NCPkgTable * packageList = getPackageList();
 
    if ( !packageList
	 || event.selection.isNull() )
    {
	return false;
    }

    if ( event.selection->compare( NCPkgNames::RpmGroups() ) == YO_EQUAL )
    {
	if ( filterPopup )
	{
	    // show the filter popup (fills the package list) 
	    retEvent = filterPopup->showFilterPopup( );
	}
    }
    else if ( event.selection->compare( NCPkgNames::Selections() ) == YO_EQUAL )
    {
	if ( selectionPopup )
	{
	    // show the selection popup
	    retEvent = selectionPopup->showSelectionPopup( );
	}
    }
    else if ( event.selection->compare( NCPkgNames::Patterns() ) == YO_EQUAL )
    {
	if ( patternPopup )
	{
	    // show the selection popup
	    retEvent = patternPopup->showSelectionPopup( );
	}
    }
    else if ( event.selection->compare( NCPkgNames::Languages() ) == YO_EQUAL )
    {
	if ( languagePopup )
	{
	    // show the selection popup
	    retEvent = languagePopup->showSelectionPopup( );
	}
    }
    else if ( event.selection->compare( NCPkgNames::Repositories() ) == YO_EQUAL )
    {
	if ( repoPopup )
	{
	    // show the selection popup
	    retEvent = repoPopup->showRepoPopup( );
	}
    }

// patches
    else if ( event.selection->compare( NCPkgNames::Recommended() ) ==  YO_EQUAL )
    {
	fillPatchList( "recommended" );	// patch kind
    }
    else if ( event.selection->compare( NCPkgNames::Security() )  ==  YO_EQUAL )
    {
	fillPatchList( "security" );		// patch kind
    }
    else if ( event.selection->compare( NCPkgNames::Optional() )  ==  YO_EQUAL )
    {
	fillPatchList( "optional" );		// patch kind
    }
    else if (  event.selection->compare( NCPkgNames::YaST2Patches() ) ==  YO_EQUAL )
    {
	fillPatchList( "YaST2" );		// patch kind
    } 
    else if ( event.selection->compare( NCPkgNames::AllPatches() )  ==  YO_EQUAL )
    {
	fillPatchList( "all" );			// show all patches
    }
    else if (  event.selection->compare( NCPkgNames::InstalledPatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "installed" );		// show installed patches
    }
    else if (  event.selection->compare( NCPkgNames::InstallablePatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "installable" );		// show installed patches
    }
    else if (  event.selection->compare( NCPkgNames::NewPatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "new" );			// show new patches
    }
    else if (  event.selection->compare( NCPkgNames::UpdateList() ) ==  YO_EQUAL )
    {
	fillUpdateList();
    }
    else if (  event.selection->compare( NCPkgNames::Whatif() ) ==  YO_EQUAL )
    {
	fillSummaryList( NCPkgTable::L_Changes );
    }
    else if (  event.selection->compare( NCPkgNames::Installed() ) ==  YO_EQUAL )
    {
	fillSummaryList( NCPkgTable::L_Installed );
    }
    
    packageList->showInformation();
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
bool NCPackageSelector::StatusHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList
	 || event.selection.isNull() )
    {
	return false;
    }
    
    if (  packageList->getNumLines() == 0 )
    {
	// nothing to do
	return true;
    }

    // call the corresponding method of NCPkgTable
    if ( event.selection->compare( NCPkgNames::Toggle() ) == YO_EQUAL )
    {
	packageList->toggleObjStatus( );
    }
    else if ( event.selection->compare( NCPkgNames::Select() ) == YO_EQUAL )
    {
	if ( testMode )
	    diskspacePopup->setDiskSpace( '+' );
	else
	    packageList->changeObjStatus( '+' );
    }
    else if ( event.selection->compare( NCPkgNames::Delete() ) == YO_EQUAL )
    {
	if ( testMode )
	    diskspacePopup->setDiskSpace( '-' );
	else
	    packageList->changeObjStatus( '-' );
    }
    else if ( event.selection->compare( NCPkgNames::Update() ) == YO_EQUAL )
    {
	if ( testMode )
	    diskspacePopup->checkDiskSpaceRange();
	else
	    packageList->changeObjStatus( '>' );
    }
    else if ( event.selection->compare( NCPkgNames::TabooOn() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '!' );	
    }
    else if ( event.selection->compare( NCPkgNames::TabooOff() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '%' );	
    } 
    else if ( event.selection->compare( NCPkgNames::SourceYes() ) == YO_EQUAL )
    {
#ifdef FIXME
	packageList->SourceInstall( true );	
#endif
    }
    else if ( event.selection->compare( NCPkgNames::SourceNo() ) == YO_EQUAL )
    {
#ifdef FIXME
	packageList->SourceInstall( false );	
#endif
    }	
    else if ( event.selection->compare( NCPkgNames::InstallAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Install );
    }
    else if ( event.selection->compare( NCPkgNames::DontInstall() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontInstall );
    }
        else if ( event.selection->compare( NCPkgNames::DeleteAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Delete );
    }
    else if ( event.selection->compare( NCPkgNames::DontDelete() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontDelete );
    }
    else if ( event.selection->compare( NCPkgNames::UpdateNewer() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_UpdateNewer );
    }
    else if ( event.selection->compare( NCPkgNames::UpdateAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Update );
    }
    else if ( event.selection->compare( NCPkgNames::DontUpdate() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontUpdate );
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
bool NCPackageSelector::PackageListHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskinfoHandler
//
// Opens a popup with disk space information.
// 
bool NCPackageSelector::DiskinfoHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();

    if ( diskspacePopup )
    {
	diskspacePopup->showInfoPopup( NCPkgNames::DiskspaceLabel() );
    }
    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// LinkHandler
//
// Handles hyperlinks in package description.
//
bool NCPackageSelector::LinkHandler ( string link )
{
    bool found = false;
    // e.g. link is pkg://hp-officeJet
    string pkgName = link.substr(6);
    
    ZyppPoolIterator
	b = zyppPkgBegin(),
	e = zyppPkgEnd(),
	i;
    for (i = b; i != e; ++i)
    {
	ZyppPkg pkgPtr = tryCastToZyppPkg ((*i)->theObj());
	if ( pkgPtr && pkgPtr->name() == pkgName )
	{
	    NCERR << "Package " << pkgName << " found" << endl;
	    // open popup with package info
	    NCPkgPopupDescr popupDescr( wpos(1,1), this );
	    popupDescr.showInfoPopup( pkgPtr, *i );
	    found = true;
	}
    }

    if ( !found )
    {
	NCERR << "Package " << pkgName << " NOT found" << endl;
	// open error popup
    }
    
    return found;
}

///////////////////////////////////////////////////////////////////
//
// testcase handler
// 
// generate solver testcase 
//
bool NCPackageSelector::TestcaseHandler ( const NCursesEvent&  event )
{
    string testCaseDir = "/var/log/YaST2/solverTestcase";

    NCMIL << "Generating solver test case START" << endl;
    bool success = zypp::getZYpp()->resolver()->createSolverTestcase( testCaseDir );
    NCMIL <<  "Generating solver test case END" << endl;

    if ( success )
    {
	NCPopupInfo info( wpos( (NCurses::lines()-8)/2, (NCurses::cols()-40)/2 ),
			  YCPString( "" ),
			  YCPString( _("Dependency resolver test case written to ") + "<br>" + testCaseDir ) );
	info.setNiceSize( 40, 8 );
	info.showInfoPopup( );	
    }
    return success;
}

///////////////////////////////////////////////////////////////////
//
// File handler
// 
// export/import all packages and patterns list to/from file
//
bool NCPackageSelector::FileHandler( const NCursesEvent& event )
{
    if ( event.selection.isNull() )
    {
 	return false;
    }

    //Export package list into file	
    if ( event.selection->compare( NCPkgNames::ExportToFile() ) == YO_EQUAL ) 
    {
 	//Ask for file to save into
 	YCPValue filename = YNCursesUI::ui()->askForSaveFileName( YCPString(DEFAULT_EXPORT_FILE_NAME), 
 					     YCPString("*.xml"), 
 					     YCPString(_("Export List of All Packages and Patterns to File" )));  
 
 	if ( filename->isString() && !filename->asString()->value().empty() ) 
 	{   
	    zypp::syscontent::Writer writer;
	    const zypp::ResPool & pool = zypp::getZYpp()->pool();

	    //some strange C++ magic (c) by ma
	    for_each( pool.begin(), pool.end(),
		boost::bind( &zypp::syscontent::Writer::addIf,
			     boost::ref(writer),
			     _1));

	    try 
	    {
		//open file for writing and try to dump syscontent into it
		std::ofstream exportFile(  filename ->asString()->value_cstr() );
		exportFile.exceptions(std::ios_base::badbit | std::ios_base::failbit );
		exportFile << writer;
		
	        NCMIL << "Exported list of packages and patterns to " << filename->toString() << endl;
	    }		    
	
	    catch (std::exception & exception)
	    {
		NCWAR << "Error exporting list of packages and patterns to " << filename->toString() << endl;

		//delete partially written file (don't care if it doesn't exist) 
		(void) unlink( filename->asString()->value_cstr() );

		//present error popup to the user
		NCPopupInfo errorMsg ( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2 ),
				YCPString( NCPkgNames::ErrorLabel()),
				YCPString( _("Error exporting list of packages and patterns to ") + filename->toString() ),
				NCPkgNames::OKLabel(),
				"");
	        errorMsg.setNiceSize(40,5);
		NCursesEvent input = errorMsg.showInfoPopup();

	    }
	
	    return true;
        } 
    }

    //Import package list from file
    else if ( event.selection->compare( NCPkgNames::ImportFromFile() ) == YO_EQUAL )
    {
	//ask for file to open
 	YCPValue filename = YNCursesUI::ui()->askForExistingFile( YCPString(DEFAULT_EXPORT_FILE_NAME),
					     YCPString("*.xml"),
					     YCPString(_("Import List of All Packages and Patterns from File")));

	if ( filename->isString()  && !filename->asString()->value().empty() )
	{ 
    	    NCPkgTable * packageList = getPackageList();
	    NCMIL << "Importing list of packages and patterns from " << filename->toString() << endl;

	    try
	    {	
		std::ifstream importFile ( filename->asString()->value_cstr() );
		zypp::syscontent::Reader reader (importFile);

		//maps to store package/pattern data into
		map<string, ZyppReaderEntry> importPkgs;
		map<string, ZyppReaderEntry> importPatterns;

		//Import syscontent reader to a map $[ "package_name" : pointer_to_data]
		for (zypp::syscontent::Reader::const_iterator it = reader.begin();
		     it != reader.end();
		     it ++ )
		{
		    string kind = it->kind();
	
		    // importMapPair =>	std::pair<string, ZyppReaderEntry>
		    if ( kind == "package" ) 
			importPkgs.insert( importMapPair( it->name(), *it ) );
		    else if ( kind == "pattern" ) 
			importPatterns.insert( importMapPair( it->name(), *it ) );
		}  

		NCMIL << "Found " << importPkgs.size() << " packages and " << importPatterns.size() << " patterns." << endl;		

		//Change status of appropriate packages and patterns
		for (ZyppPoolIterator it = zyppPkgBegin();
		     it != zyppPkgEnd(); 
		     it++ )
		{
		    ZyppSel selectable = *it;
		    //isWanted => package name found in importPkgs map
		    importSelectable ( *it, importPkgs.find( selectable->name() ) != importPkgs.end(), "package" ); 
		}

		for (ZyppPoolIterator it = zyppPatternsBegin();
		     it != zyppPatternsEnd(); 
		     it++ )
		{
		    ZyppSel selectable = *it;
		    importSelectable ( *it, importPatterns.find( selectable->name() ) != importPatterns.end(), "pattern" ); 
		}

		//Switch to installation summary filter
		fillSummaryList(NCPkgTable::L_Changes);

		//... and finally display the result
		packageList->showInformation();
		packageList->setKeyboardFocus();

	        return true;
	    }
	    catch ( const zypp::Exception & exception )
	    {
		NCWAR << "Error importing list of packages and patterns from" << filename->toString() << endl;

		NCPopupInfo errorMsg ( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2) ,
				YCPString( NCPkgNames::ErrorLabel()),
				YCPString( _("Error importing list of packages and patterns from ") + filename->toString() ),
				NCPkgNames::OKLabel(),
				"");
	        errorMsg.setNiceSize(40,5);
		NCursesEvent input = errorMsg.showInfoPopup();

	    }
	}
    }
    return true; 	
}
///////////////////////////////////////////////////////////////////
//
// HelpHandler
// 
// Show the help popup
//
bool NCPackageSelector::HelpHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
    string text = "";
    YCPString headline = YCPString(NCPkgNames::PackageHelp());
    
    if ( event.selection.isNull() )
    {
	return false;
    }
    
    if ( event.selection->compare( NCPkgNames::GeneralHelp() ) == YO_EQUAL )
    {
	text += NCPkgNames::HelpPkgInst1();
	text += NCPkgNames::HelpPkgInst12();
	text += NCPkgNames::HelpPkgInst13();
	text += NCPkgNames::HelpPkgInst2();
	text += NCPkgNames::HelpPkgInst3();
	text += NCPkgNames::HelpPkgInst4();
	text += NCPkgNames::HelpPkgInst5();
	text += NCPkgNames::HelpPkgInst6();
    }
    else if ( event.selection->compare( NCPkgNames::StatusHelp() ) == YO_EQUAL )
    {
	text += NCPkgNames::HelpOnStatus1();
	text += NCPkgNames::HelpOnStatus2();
	text += NCPkgNames::HelpOnStatus3();
	text += NCPkgNames::HelpOnStatus4();
	text += NCPkgNames::HelpOnStatus5();
	text += NCPkgNames::HelpOnStatus6();
	text += NCPkgNames::HelpOnStatus7();
    }
    else if ( event.selection->compare( NCPkgNames::UpdateHelp() ) == YO_EQUAL )
    {
	text += NCPkgNames::HelpOnUpdate();
    }
    else if ( event.selection->compare( NCPkgNames::SearchHelp() ) == YO_EQUAL )
    {
	headline = YCPString(NCPkgNames::SearchHeadline());
	text += NCPkgNames::HelpOnSearch();
    }
    
    // open the popup with the help text
    NCPopupInfo pkgHelp( wpos( (NCurses::lines()*8)/100, (NCurses::cols()*18)/100 ), headline, YCPString( text ) );
    pkgHelp.setNiceSize( (NCurses::cols()*65)/100, (NCurses::lines()*85)/100 );
    pkgHelp.showInfoPopup( );

    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }
	
    return true;
}

///////////////////////////////////////////////////////////////////
//
// YOUHelpHandler
// 
// Show the Online Update Help
//
bool NCPackageSelector::YouHelpHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
    string text  = "";

    text += NCPkgNames::YouHelp1();
    text += NCPkgNames::YouHelp2();
    text += NCPkgNames::YouHelp3();

    // open the popup with the help text
    NCPopupInfo youHelp( wpos( NCurses::lines()/3, NCurses::cols()/6 ), YCPString(NCPkgNames::YouHelp()), YCPString(text) );
    youHelp.setNiceSize( (NCurses::cols()*2)/3, NCurses::lines()/3 );
    youHelp.showInfoPopup( );

    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }
       
    return true;
}

///////////////////////////////////////////////////////////////////
//
// CancelHandler
//
// Cancel button handler.
// 
bool NCPackageSelector::CancelHandler( const NCursesEvent&  event )
{
    bool changes = diffState ();

    if (changes) {
	// show a popup and ask the user
	NCPopupInfo cancelMsg( wpos( (NCurses::lines()-8)/2, (NCurses::cols()-45)/2 ),
			   YCPString( NCPkgNames::NotifyLabel() ),
			   YCPString( NCPkgNames::CancelText() ),
			   NCPkgNames::YesLabel(),
			   NCPkgNames::NoLabel() );
	cancelMsg.setNiceSize( 45, 8 ); 
	NCursesEvent input = cancelMsg.showInfoPopup( );
	if ( input == NCursesEvent::cancel ) {
	    // don't leave the package installation dialog
	    return true;
	}
    }

    restoreState ();

    NCMIL <<  "Cancel button pressed - leaving package selection" << endl;
    const_cast<NCursesEvent &>(event).result = YCPSymbol("cancel");
    
    // return false, which means stop the event loop (see runPkgSelection)
    return false;
}

///////////////////////////////////////////////////////////////////
//
// OkButtonHandler
//
// OK button handler.
// 
bool NCPackageSelector::OkButtonHandler( const NCursesEvent&  event )
{
    bool closeDialog = true;
    bool confirmedAllLicenses = false;

    // check/show dependencies also if youMode == true
    do
    {
	// show the dependency popup
	if ( showPackageDependencies( true ) )
	{
	    // don't leave the package installation if the user has clicked on Cancel
	    // in dependency popup because maybe he wants to change his choices
	    closeDialog = false;
	}

	confirmedAllLicenses = showPendingLicenseAgreements();
	
    } while ( !confirmedAllLicenses && closeDialog );

    if ( !youMode )	// don't show automatic changes if YOU mode
    {
	// show the automatic changes list
	NCPkgPopupTable autoChangePopup( wpos( 3, 8), this );
	NCursesEvent input = autoChangePopup.showInfoPopup();

	if ( input == NCursesEvent::cancel )
	{
	    // user clicked on Cancel
	    closeDialog = false;
	}
    }
    
    if ( diskspacePopup )
    {
	string message = "";
	message = diskspacePopup->checkDiskSpace();
	if ( message != "" )
	{
	    // open the popup e.g. with the text "/usr needs 50 MB more disk space"
	    NCPopupInfo spaceMsg( wpos( (NCurses::lines()-10)/2, (NCurses::cols()-50)/2 ),
				  YCPString( NCPkgNames::ErrorLabel() ),
				  YCPString( NCPkgNames::DiskSpaceError() + "<br>" + message ),
				  NCPkgNames::OKLabel(),
				  NCPkgNames::CancelLabel() );
	    
	    spaceMsg.setNiceSize( 50, 10 ); 
	    NCursesEvent input = spaceMsg.showInfoPopup( );

	    if ( input == NCursesEvent::cancel )
	    {
		// disk space error warning returned `cancel
		closeDialog = false;
	    }
	}
    }

    if ( closeDialog )
    {
	// clear the saved states
	// could free some memory?
	// clearSaveState ();

	const_cast<NCursesEvent &>(event).result = YCPSymbol("accept"); 
	NCMIL <<  "OK button pressed - leaving package selection, starting installation" << endl;

        // return false, leave the package selection	
	return false;
    }
    else
    {
	NCPkgTable * packageList = getPackageList();
	if ( packageList )
	{
	    packageList->setKeyboardFocus();
	}
	// don't leave the dialog
	return true;
    }
}

bool NCPackageSelector::showPendingLicenseAgreements()
{
    bool allConfirmed = true;

    if ( youMode )
	allConfirmed = showPendingLicenseAgreements( zyppPatchesBegin(), zyppPatchesEnd() );

    allConfirmed = showPendingLicenseAgreements( zyppPkgBegin(), zyppPkgEnd() ) && allConfirmed;

    return allConfirmed;
}

bool NCPackageSelector::showPendingLicenseAgreements( ZyppPoolIterator begin, ZyppPoolIterator end )
{
    y2milestone( "Showing all pending license agreements" );

    bool allConfirmed = true;

    for ( ZyppPoolIterator it = begin; it != end; ++it )
    {
	ZyppSel sel = (*it);

	switch ( sel->status() )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:

		if ( sel->candidateObj() )
		{
		    string licenseText = sel->candidateObj()->licenseToConfirm();

		    if ( ! licenseText.empty() )
		    {
			y2milestone( "Package/Patch %s has a license agreement", sel->name().c_str() );

			if( ! sel->hasLicenceConfirmed() )
			{
			    allConfirmed = showLicenseAgreement( sel, licenseText ) && allConfirmed;
			}
			else
			{
			    y2milestone( "Package/Patch %s's  license is already confirmed", sel->name().c_str() );
			}
		    }
		}
		break;

	    default:
		break;
	}
    }

    return allConfirmed;
}

bool NCPackageSelector::showLicenseAgreement( ZyppSel & slbPtr , string licenseText )
{
    if ( !slbPtr )
	return false;

    bool license_confirmed = true;
    bool ok = true;
    string pkgName = slbPtr->name();

    NCPopupInfo info( wpos( NCurses::lines()/10, NCurses::cols()/10),
		      NCPkgNames::NotifyLabel(),
		      YCPString( "<i>" + pkgName + "</i><br><br>" + createDescrText( licenseText ) ),
		      NCPkgNames::AcceptLabel(),
		      NCPkgNames::CancelLabel(),
		      NCPkgNames::PrintLicenseText() );
    info.setNiceSize( (NCurses::cols() * 80)/100, (NCurses::lines()*80)/100);
    license_confirmed = info.showInfoPopup( ) != NCursesEvent::cancel;


    if ( !license_confirmed )
    {
	// make sure the package won't be installed
	switch ( slbPtr->status() )
	{
	    case S_Install:
	    case S_AutoInstall:
		slbPtr->set_status( S_Taboo );
		break;
		    
	    case S_Update:
	    case S_AutoUpdate:
		slbPtr->set_status(  S_Protected );
		break;

	    default:
		break;
	}
	    
	ok = false;
    } else {
	NCMIL << "User confirmed license agreement for " << pkgName << endl;
	slbPtr->setLicenceConfirmed (true);
	ok = true;
    }

    return ok;
}


///////////////////////////////////////////////////////////////////
//
// showPatchInformation
//
// Shows the patch information
//
bool NCPackageSelector::showPatchInformation ( ZyppObj objPtr, ZyppSel selectable )
{
    ZyppPatch patchPtr = tryCastToZyppPatch( objPtr );

    if ( !patchPtr || !selectable )
    {
	NCERR << "Patch not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }    

    if (  visibleInfo->compare( NCPkgNames::PatchDescr() ) == YO_EQUAL )
    {
	string descr;
	
	descr += NCPkgNames::Patch();
	descr += selectable->name();
	descr += "&nbsp;";
	// the patch size is not available
        // descr += NCPkgNames::Size();
	// descr += patchPtr->size().asString( 8 );
	descr += "<b>";
	descr += NCPkgNames::PatchKind();
	descr += ": </b>";
	descr += patchPtr->category();
	descr += "&nbsp;";
	descr += NCPkgNames::Version();
	descr += patchPtr->edition().asString();
	descr += "<br>";

	if ( selectable->hasInstalledObj()
	     && selectable->installedPoolItem().status().isIncomplete() )
	{
	    descr += _( "----- this patch is broken !!! -----" );
	    descr += "<br>";
	}
	// get and format the patch description
	zypp::Text value = patchPtr->description();
	descr += createDescrText( value );
	
	// show the description	
	YWidget * descrInfo = y2ui->widgetWithId( NCPkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString( descr ) );
	}	
    }
    else if (  visibleInfo->compare( NCPkgNames::PatchPackages() ) == YO_EQUAL )
    {
	NCPkgTable *patchPkgList  = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::PatchPkgs(), true));
	if ( patchPkgList )
	{
	    fillPatchPackages ( patchPkgList, objPtr);
	}
    }
    else if (  visibleInfo->compare( NCPkgNames::PatchPackagesVersions() ) == YO_EQUAL )
    {
	NCPkgTable *patchPkgsVersions  = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::PatchPkgsVersions(), true));
	if ( patchPkgsVersions )
	{
	    fillPatchPackages ( patchPkgsVersions, objPtr, true);
	}
    }
    
    return true;
}


///////////////////////////////////////////////////////////////////
//
// showDependencies
//
// Checks and shows the dependencies
//
bool NCPackageSelector::showPackageDependencies ( bool doit )
{
    bool ok = false;
    bool cancel = false;
    
    if ( depsPopup
	 && (doit || autoCheck) )
    {
	NCMIL << "Checking dependencies" << endl;
	cancel = depsPopup->showDependencies( NCPkgPopupDeps::S_Solve, &ok );
    }

    return cancel;
}

bool NCPackageSelector::verifyPackageDependencies ()
{
    bool ok = false;
    bool cancel = false;
    
    NCPopupInfo info( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-30)/2 ),  YCPString( "" ),
		      YCPString(_( "System dependencies verify OK." )),
		      NCPkgNames::OKLabel() );
    info.setNiceSize( 30, 5 );
    
    NCMIL << "Verifying system" << endl;

    if ( depsPopup )
    {
	saveState();
	//call the solver (with S_Verify it displays no popup)
	cancel = depsPopup->showDependencies( NCPkgPopupDeps::S_Verify, &ok );
        //display the popup with automatic changes
	NCPkgPopupTable autoChangePopup( wpos( 3, 8 ), this );
        NCursesEvent input = autoChangePopup.showInfoPopup();

        if ( input == NCursesEvent::cancel )
        {
            // user clicked on Cancel
	    restoreState();
            cancel = true;
        }
	if ( ok && input == NCursesEvent::button )
	{
	    // dependencies OK, no automatic changes/the user has accepted the changes
	    info.showInfoPopup();
	}
    }
    return cancel;
}

///////////////////////////////////////////////////////////////////
//
// showDependencies
//
// Checks and shows the dependencies
//
void NCPackageSelector::showSelectionDependencies ( )
{
    showPackageDependencies (true);
}

///////////////////////////////////////////////////////////////////
//
// showPackageInformation
//
// Shows the package information
//
bool NCPackageSelector::showPackageInformation ( ZyppObj pkgPtr, ZyppSel slbPtr )
{
    if ( !pkgPtr )
    {
	NCERR << "Package not valid" << endl;
	return false;
    }
    if ( !slbPtr )
    {
	NCERR << "Selectable not valid" << endl;
	return false;
    }
    
    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }

    if ( visibleInfo->compare( NCPkgNames::LongDescr() ) == YO_EQUAL )
    {
	// ask the package manager for the description of this package
	zypp::Text value = pkgPtr->description();
	string descr = createDescrText( value );

        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( NCPkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(descr) );
	}
    }
    else if (  visibleInfo->compare( NCPkgNames::Files() ) == YO_EQUAL )
    {
	string text = NCPkgNames::ListOfFiles();
	// the file list is available only for installed packages
	ZyppPkg package = tryCastToZyppPkg (slbPtr->installedObj());

	if ( package )
	{
	    // get the file list from the package manager/show the list
	    list<string> fileList = package->filenames();
	    text += createText( fileList, false ) ;
	}
	
	// get the widget id 
	YWidget * descrInfo = y2ui->widgetWithId( NCPkgNames::Description(), true );  
	if ( descrInfo  )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(text) );
	}	
    }
    else if ( visibleInfo->compare( NCPkgNames::PkgInfo() ) == YO_EQUAL )
    {
	string instVersion = "";
	string version = "";
	string text = "";
	
	text += slbPtr->name();
	text += " - ";
	
	text += pkgPtr->summary();
	text += "<br>";

	if ( slbPtr->hasBothObjects () )
	{
	    ZyppObj io = slbPtr->installedObj ();
	    instVersion = io->edition().version();
	    instVersion += "-";
	    instVersion += io->edition().release();
	    ZyppObj co = slbPtr->candidateObj ();
	    version = co->edition().version();
	    version += "-";
	    version += co->edition().release();
	}
	else
	{
	    version = pkgPtr->edition().version();
	    version += "-";
	    version += pkgPtr->edition().release(); 
	}
	
	text += NCPkgNames::Version();
	text +=  version;
	if ( instVersion != "" )
	{
	    text += "  ";
	    text += NCPkgNames::InstVersion();
	    text += instVersion;
	}
	text +=  "  ";
	
	// show the size
	text += NCPkgNames::Size();
	text += pkgPtr->size().asString();
	text +=  "  ";
	
	ZyppPkg package = tryCastToZyppPkg (pkgPtr);
	if ( package )
	{
	    // add the media nr
	    text += NCPkgNames::MediaNo();
	    char num[5];
	    int medianr = package->mediaNr ();
	    sprintf( num, "%d", medianr );
	    text += num;
	    text += "<br>";	    

	    // the license
	    text += NCPkgNames::License();
	    text += package->license();
	    text += "  ";
	    text += "<br>";

	    // the rpm group
	    text += NCPkgNames::RpmGroup();
	    text += package->group ();
	    text += "<br>";
	}
	
	// show Provides:
	text += NCPkgNames::Provides();
	zypp::CapSet provides = package->dep (zypp::Dep::PROVIDES);
	text += createRelLine(provides);
	text += "<br>";

	// show the authors
	if ( package )
	{
	    text += NCPkgNames::Authors();
	    list<string> authors = package->authors(); // zypp::Package	
	    text += createText( authors, true );
	}
	
        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( NCPkgNames::Description(), true );

	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(text) );
	}
    }
    else if (  visibleInfo->compare( NCPkgNames::Versions() ) == YO_EQUAL )
    {
	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(NCPkgNames::AvailPkgs(), true));
	if ( pkgAvail )
	{
	    fillAvailableList( pkgAvail, slbPtr );
	}
    }
    else if (  visibleInfo->compare( NCPkgNames::Relations() ) == YO_EQUAL )
    {
	string text = "";
	text += slbPtr->name();
	text += " - ";

	text += pkgPtr->summary();		// the summary
	text += "<br>";

	// show the relations, all of them except provides which is above
	zypp::Dep deptypes[] = {
	    zypp::Dep::PREREQUIRES,
	    zypp::Dep::REQUIRES,
	    zypp::Dep::CONFLICTS,
	    zypp::Dep::OBSOLETES,
	    zypp::Dep::RECOMMENDS,
	    zypp::Dep::SUGGESTS,
	    zypp::Dep::FRESHENS,
	    zypp::Dep::ENHANCES,
	    zypp::Dep::SUPPLEMENTS,
	};
	for (size_t i = 0; i < sizeof (deptypes)/sizeof(deptypes[0]); ++i)
	{
	    zypp::Dep deptype = deptypes[i];
	    zypp::CapSet relations = pkgPtr->dep (deptype);
	    string relline = createRelLine (relations);
	    if (!relline.empty ())
	    {
		// FIXME: translate
		text += "<b>" + deptype.asString () + ": </b>"
		    + relline + "<br>";
	    }
	}
	
        // show the package relations	
	YWidget * descrInfo = y2ui->widgetWithId( NCPkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString( text ) );
	}
    }
 
    NCDBG <<  "Showing package information: " << visibleInfo->toString() << endl;
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// createDescrText
//
#define DOCTYPETAG "<!-- DT:Rich -->"

string NCPackageSelector::createDescrText( zypp::Text value )
{
    string html_text = "";
    
#ifdef FIXME
    bool author_format = false;
    bool htmlFormat = false;     /* Is the description coming in html? */
    /* By default, this is false and the text is plain text. But if the
     * description contains DOCTYPETAG in the first line, it is considered
     * to be formatted in html. The yast (this method here) needs not to
     * do further formatting for the text part.
     */
    
    list<string>::const_iterator it = value.begin();

    string line;

    /* Check if the first line is the html tag */
    if( it != value.end() )
    {
	line = (*it);
	const string htmlIdent(DOCTYPETAG);

	if ( line.length() >= htmlIdent.length() &&  /* Avoid exception if stringlen < len of tag */
	     line.substr( 0,  htmlIdent.length() ) ==  htmlIdent ) /* first line == DOCTYPETAG ? */
	{
	    htmlFormat = true;  /* indicate that the text is already html formatted */
	}
	else
	{
	    html_text += line + " ";
	}
	++it;
    }

    /** Loop over the remaining text. **/
    const string authors("Authors:");
    while ( it != value.end() )
    {
	line = (*it);

	/* Check if authors-line starts */
	if ( line.length() >= authors.length() &&  /* Avoid exception if stringlen < len of Authors */
	     line.substr( 0, authors.length() ) ==  authors )
	{
	    line = "<br><b>" + line + "</b>";
	    author_format = true;
	}
	if ( author_format )
	{
	    /* every author in his own line */
	    html_text += line + "<br>";
	}
	else
	{
	    html_text += " " + line;
	    if( (! htmlFormat) && (line.length() == 0) )
	    {
		html_text += "<br>";
	    }
	    else
	    {
		html_text += " ";
	    }	
	}
	
	++it;
    }
#else
    html_text = value;
#endif
    
    return html_text;
}

///////////////////////////////////////////////////////////////////
//
// updatePackageList
//
void NCPackageSelector::updatePackageList()
{
    NCPkgTable * packageList = getPackageList();

    if ( packageList )
    {
	packageList->updateTable();
    }
}

///////////////////////////////////////////////////////////////////
//
// createProvides
//
string NCPackageSelector::createRelLine( const zypp::CapSet & info )
{
    string text = "";
    zypp::CapSet::const_iterator
	b = info.begin (),
	e = info.end (),
	it;
    unsigned int i, n = info.size();
    
    for ( it = b, i = 0; it != e; ++it, ++i )
    {
	text = text + (*it).asString();
	if ( i < n - 1 )
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
string NCPackageSelector::createText( list<string> info, bool oneline )
{
    list<string>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end() && i < 1000; ++it, i++ )
    {
	text += (*it);
	if ( i < info.size()-1 )
	{
	    if ( oneline && i < 999 )
	    {
		text += ", ";
	    }
	    else
	    {
		text += "<br>";	
	    }
	}
	if ( i == 999 )
	{
	    text += "...";
	}
    }

    return text;
}

static
ostream & operator << (ostream & s, zypp::DiskUsageCounter::MountPoint mp)
{
    return s << mp.dir
	     << " t:" << mp.total_size
	     << " u:" << mp.used_size
	     << " p:" << mp.pkg_size;
}

///////////////////////////////////////////////////////////////////
//
// showDiskSpace()
//
void NCPackageSelector::showDiskSpace()
{
    zypp::ZYpp::Ptr z = zypp::getZYpp();
    zypp::DiskUsageCounter::MountPointSet du = z->diskUsage ();
    zypp::DiskUsageCounter::MountPointSet::iterator
	b = du.begin (),
	e = du.end (),
	it;
    if (b == e)
    {
	// retry after detecting from the target
	z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
	du = z->diskUsage();
	b = du.begin ();
	e = du.end ();
    }

    zypp::ByteCount diff = 0;
    for (it = b; it != e; ++it)
    {
	UIMIL << *it << endl;
	diff += (it->pkg_size - it->used_size) * 1024;
    }

    // show pkg_diff, i.e. total difference of disk space (can be negative in installed system
    // if packages are deleted)
    YCPString label( diff.asString () );
    
    // show the required diskspace
    YWidget * diskSpace = y2ui->widgetWithId( NCPkgNames::Diskspace(), true );
    if ( diskSpace )
    {
	static_cast<NCLabel *>(diskSpace)->setLabel( label );
    }

    // check whether required diskspace enters the warning range
    if ( diskspacePopup )
	diskspacePopup->checkDiskSpaceRange( );
}


///////////////////////////////////////////////////////////////////
//
// showDownloadSize()
//
// total download size of YOU patches
//
void NCPackageSelector::showDownloadSize()
{
    set<ZyppSel> selectablesToInstall;

    for ( ZyppPoolIterator patches_it = zyppPatchesBegin();
	  patches_it != zyppPatchesEnd();
	  ++patches_it )
    {
	ZyppPatch patch = tryCastToZyppPatch( (*patches_it)->theObj() );

	if ( patch )
	{
	    ZyppPatchContents patchContents( patch );

	    for ( ZyppPatchContentsIterator contents_it = patchContents.begin();
		  contents_it != patchContents.end();
		  ++contents_it )
	    {
		ZyppPkg pkg = tryCastToZyppPkg( *contents_it );
		ZyppSel sel;
		
		if ( pkg )
		    sel = selMapper.findZyppSel( pkg );
		

		if ( sel )
		{
		    switch ( sel->status() )
		    {
			case S_Install:
			case S_AutoInstall:
			case S_Update:
			case S_AutoUpdate:
			    // Insert the patch contents selectables into a set,
			    // don't immediately sum up their sizes: The same
			    // package could be in more than one patch, but of
			    // course it will be downloaded only once.

			    selectablesToInstall.insert( sel );
			    break;

			case S_Del:
			case S_AutoDel:
			case S_NoInst:
			case S_KeepInstalled:
			case S_Taboo:
			case S_Protected:
			    break;

			    // intentionally omitting 'default' branch so the compiler can
			    // catch unhandled enum states
		    }

		}
	    }
	}
    }

    FSize totalSize = 0;

    for ( set<ZyppSel>::iterator it = selectablesToInstall.begin();
	  it != selectablesToInstall.end();
	  ++it )
    {
	if ( (*it)->candidateObj() )
	    totalSize += (*it)->candidateObj()->size();
    }

    YCPString label( totalSize.asString() );

    // show the download size
    YWidget * diskSpace = y2ui->widgetWithId( NCPkgNames::Diskspace(), true );
    if ( diskSpace )
    {
	static_cast<NCLabel *>(diskSpace)->setLabel( label );
    }
}


///////////////////////////////////////////////////////////////////
//
// getPackageList()
//
NCPkgTable * NCPackageSelector::getPackageList()
{
	YWidget * packages = y2ui->widgetWithId( NCPkgNames::Packages(), true );
	
	return ( dynamic_cast<NCPkgTable *>(packages) );
}
