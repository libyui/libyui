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
#include "NCWidgetFactory.h"
#include "NCPushButton.h"
#include "NCMenuButton.h"
#include "NCTable.h"
#include "NCPkgTable.h"
#include "NCSpacing.h"
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
#include "NCLayoutBox.h"
#include "YSelectionBox.h"
#include "YAlignment.h"
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

#include "YWidgetID.h"
#include "YCPDialogParser.h"
#include "YCPValueWidgetID.h"
#include "YPackageSelector.h"

typedef zypp::ui::PatchContents			ZyppPatchContents;
typedef zypp::ui::PatchContents::const_iterator	ZyppPatchContentsIterator;
typedef zypp::syscontent::Reader::Entry		ZyppReaderEntry;
typedef std::pair<string, ZyppReaderEntry>	importMapPair; 

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>
#include "YEvent.h"

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
NCPackageSelector::NCPackageSelector( YNCursesUI * ui, YWidget * wRoot, long modeFlags )
    : y2ui( ui )
      , widgetRoot ( wRoot )
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
      , pkgList ( 0 )
      , filterMenu( 0 )
      , groupsItem( 0 )
      , patternsItem( 0 )
      , languagesItem( 0 )
      , reposItem( 0 )
      , searchItem( 0 )
      , installedItem( 0 )
      , whatifItem( 0 )
      , updatelistItem( 0 )
      , actionMenu( 0 )
      , toggleItem( 0 )
      , selectItem( 0 )
      , deleteItem( 0 )
      , updateItem( 0 )
      , tabooItem( 0 )
      , notabooItem( 0 )
      , allItem( 0 )
      , selallItem( 0 )
      , delallItem( 0 )
      , deselallItem( 0 )
      , dontdelItem( 0 )
      , updnewerItem( 0 )
      , updallItem( 0 )
      , dontupdItem( 0 )
      , infoMenu( 0 )
      , pkginfoItem( 0 )
      , longdescrItem( 0 )
      , versionsItem( 0 )
      , filesItem( 0 )
      , relationsItem( 0 )
      , patchdescrItem( 0 )
      , patchpkgsItem( 0 )
      , pkgversionsItem( 0 )
      , etcMenu( 0 )
      , depsItem( 0 )
      , showdepsItem( 0 )
      , autodepsItem( 0 )
      , noautodepsItem( 0 )
      , verifyItem( 0 )
      , allpksItem( 0 )
      , exportItem( 0 )
      , importItem( 0 )
      , testcaseItem( 0 )
      , filterLabel( 0 )
      , diskspaceLabel( 0 )
      , infoText( 0 )
      , replacePoint( 0 )
      , versionsList( 0 )
      , patchPkgs( 0 )
      , patchPkgsVersions( 0 )
      , okButton( 0 )
      , cancelButton( 0 )
      , visibleInfo( 0 )	

{
    // FIXME - remove this code when everthiong is handled by handleEvent() !
    //         (the eventHandlerMap is obsolete)
    
    // Fill the handler map
    eventHandlerMap[ NCPkgNames::Search()->toString() ]   = &NCPackageSelector::SearchHandler;
    eventHandlerMap[ NCPkgNames::Diskinfo()->toString() ] = &NCPackageSelector::DiskinfoHandler;

    // Filter - add selectionsItem, if really needed
    // eventHandlerMap[ NCPkgNames::Selections()->toString() ] = &NCPackageSelector::FilterHandler;

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

    // Information menu -> DONE

    // Action menu -> DONE
    
    // Etc. menu -> DONE
    
    // Help menu
    eventHandlerMap[ NCPkgNames::GeneralHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::StatusHelp()->toString() ]  = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::FilterHelp()->toString() ]  = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::UpdateHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::SearchHelp()->toString() ] = &NCPackageSelector::HelpHandler;
    eventHandlerMap[ NCPkgNames::PatchHelp()->toString() ]  = &NCPackageSelector::YouHelpHandler;


    if ( modeFlags & YPkg_OnlineUpdateMode )
	youMode = true;

    if ( modeFlags & YPkg_UpdateMode )
	updateMode = true;

    // read test source information
    if (modeFlags & YPkg_TestMode )
	testMode = true;

    saveState ();
    
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
	    NCDBG << "Adding group: " << zyppPkg->group() << endl;
	}
    }
	
    // The creation of the popups is moved to createPopups()
}


///////////////////////////////////////////////////////////////////
//
// Destructor
//
NCPackageSelector::~NCPackageSelector()
{
    // Changed because of new libyui: don't call delete for the popups;
    // call YDialog::deleteTopmostDialog() instead at the end of
    // NCPackageSelectorPlugin::runPkgSelection
}

///////////////////////////////////////////////////////////////////
//
// 	createPopups()
//
// 	Create all popups
//
// 	Don't create the popups any longer in constructur because the current dialog
// 	wouldn't be the PackageSelector dialog then but a PopupDialog and
// 	NCPackageSelectorPlugin::runPkgSelection wouldn't find the selector widget.
// 	Call NCPackageSelector::createPopups() instead after the PackageSelector dialog
// 	is created (see NCPackageSelectorStart::showDefaultList()).
//
void NCPackageSelector::createPopups()
{
    if ( !youMode )
    {
	// create the selections popup
	selectionPopup = new NCPkgPopupSelection( wpos( 1, 1 ), this, NCPkgPopupSelection::S_Selection );

	// create the patterns popup
	patternPopup =  new NCPkgPopupSelection( wpos( 1, 1 ), this, NCPkgPopupSelection::S_Pattern );

	// create language popup
	languagePopup = new NCPkgPopupSelection( wpos( 1,1 ), this, NCPkgPopupSelection::S_Language );

	// create repository popup
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
//
//	getDefaultRpmGroup()
//
//	returns the default RPM group, the packages of this group
//	are displayed on PackageSelector start
//
YStringTreeItem * NCPackageSelector::getDefaultRpmGroup()
{
    if ( _rpmGroupsTree &&
	 _rpmGroupsTree->root() )
	return _rpmGroupsTree->root()->firstChild();
    else
	return 0;
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

string NCPackageSelector::getButtonId( YWidget * button )
{
    YCPValueWidgetID * ycpId;
    string id;

    if ( !button )
	return id;
    
    YWidgetID * widgetId = button->id();
    if ( widgetId )
    {
        ycpId = dynamic_cast<YCPValueWidgetID *>(widgetId);
	if ( ycpId )
	{
	    id = ycpId->toString();
	}
    }
    return id;
}

string NCPackageSelector::getMenuId( YMenuItem *menuItem )
{
    string id;
    YMenuEvent * menuEvent;
    
    if ( !menuItem )
	return id;

    menuEvent = new YMenuEvent( menuItem );
    if ( menuEvent )
    {
	YCPValue selectionId = menuEvent->id();
    	id = selectionId->toString();

	delete menuEvent;
    }
    
    return id;
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

    if ( event == NCursesEvent::handled )
	return false;
    
    // Call the appropriate handler
    if ( event == NCursesEvent::button )
    {
	if ( event.widget == okButton )
	    retVal = OkButtonHandler( event );
	else if ( event.widget == cancelButton )
	    retVal = CancelHandler( event );
	    
    }
    else if ( event == NCursesEvent::menu )
    {
	if ( event.widget == filterMenu )
	    // filter out packages/patches
	    retVal = FilterHandler( event );
	else if ( event.widget == actionMenu )
	    // change package/patch status
	    retVal = StatusHandler( event );
	else if ( event.widget == infoMenu )
	    // show package/patch information
	    retVal = InformationHandler( event );
	else if ( event.widget == etcMenu )
	{
	    if ( event.selection == testcaseItem )
		// generate testcase
		retVal = TestcaseHandler( event );
	    else if ( (event.selection == exportItem) || (event.selection == importItem) )
		// import/export file list
		retVal = FileHandler( event );
	    else
		// check package dependencies
		retVal = DependencyHandler( event );
	}
	else if ( event.selection->label().substr(0,4) == "pkg:" )
	    // handle hyper links
	    retVal = LinkHandler( event.selection->label() );
	    
    }

    return retVal;
}


///////////////////////////////////////////////////////////////////
//
// setVisibleInfo
//
// currently displayed package information
//
void NCPackageSelector::setVisibleInfo( YMenuItem * info )
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
	if ( filterLabel )
	{
	    filterLabel->setLabel( label->toString() );
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
bool NCPackageSelector::fillSearchList( const string & expr,
					bool ignoreCase,
					bool checkName,
					bool checkSummary,
					bool checkDescr,
					bool checkProvides,
					bool checkRequires )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
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
	    if ( ( checkName && match( pkg->name(), expr, ignoreCase )) ||
		 ( checkSummary && match( pkg->summary(), expr, ignoreCase) ) ||
		 ( checkDescr && match( description, expr, ignoreCase) ) ||
		 ( checkProvides && match( provides, expr, ignoreCase) ) ||
		 ( checkRequires && match( requires,  expr, ignoreCase) )
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
    if ( filterLabel )
    {
	filterLabel->setText( NCPkgNames::SearchResults() );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchSearchList
//
// Fills the patch list with search results
//
bool NCPackageSelector::fillPatchSearchList( const string & expr )
{
   NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
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
          if ( match( (*listIt)->name(), expr, true ) )
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
    if ( filterLabel )
    {
	filterLabel->setLabel( NCPkgNames::SearchResults() );
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
    if ( filterLabel )
    {
	if ( filter == "installable" )
	{
	    // show common label "Online Update Patches"
	    filterLabel->setLabel( NCPkgNames::YOUPatches() );
	}
	else if ( filter == "installed" )
	{
  	    filterLabel->setLabel( NCPkgNames::InstPatches() );  
	}
	else
	{
	    filterLabel->setLabel( NCPkgNames::Patches() );  
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
    if ( filterLabel )
    {
	filterLabel->setLabel( NCPkgNames::UpdateProblem() );
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
		if ( inContainer( patchSelectables, sel ) )
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
    if ( filterLabel )
    {
	filterLabel->setLabel( NCPkgNames::InstSummary() );
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

    NCMIL << "Fill package list" << endl;

    if ( !label.isNull() )
    {
	// show the selected filter label
	if ( filterLabel )
	{
	   filterLabel->setText( label->toString() );
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

    if ( repoPopup && filterLabel )
    {
	ZyppProduct product = repoPopup->findProductForRepo( repo );
	if ( product )
	{
	    filterLabel->setLabel( product->summary() ); 
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
	NCMIL << "Searching for: " <<  retEvent.result << endl;
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
	 || !event.selection
	 || !replacePoint )
    {
	NCERR << "*** InformationHandler RETURN false ***" << endl;
	return false;
    }

    YMenuItem * info = event.selection;

    NCMIL << "Show info: " << info->label() << endl;

    if ( visibleInfo == info )
    {
	// information selection has not changed
	return true;
    }

    // set visibleInfo
    visibleInfo = info;

    if ( infoText )
    {
	// RichText widget remains in place
	if ( ( visibleInfo == longdescrItem ) ||
	     ( visibleInfo == filesItem ) ||
	     ( visibleInfo == pkginfoItem ) )
	{
	    packageList->showInformation( ); 
	    return true;
	}
    }
	     
    // delete current child of the ReplacePoint
    YWidget * replaceChild = replacePoint->firstChild();
    wrect oldSize;
    
    if ( replaceChild )
    {
	oldSize = dynamic_cast<NCWidget *>(replaceChild)->wGetSize();
	
	delete replaceChild;
	// reset all info widgets
	infoText = 0;
	versionsList = 0;
	patchPkgs = 0;
	patchPkgsVersions = 0;
    }

    if ( visibleInfo == versionsItem )
    {
	// show a package table with all available package versions
	YTableHeader * tableHeader = new YTableHeader();
	versionsList = new NCPkgTable( replacePoint, tableHeader );
        // YDialog::currentDialog()->setInitialSize(); -> doesn't work
	// call versionsList->setSize() and versionsList->Redraw() instead

	if ( versionsList )
	{
	    versionsList->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    versionsList->Redraw();

	    // set the connection to the NCPackageSelector !!!!
	    versionsList->setPackager( this );
	    // set status strategy
	    NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	    versionsList->setTableType( NCPkgTable::T_Availables, strategy );
	    versionsList->fillHeader( );

	    fillAvailableList( versionsList, packageList->getSelPointer( packageList->getCurrentItem() ) );
	}
    }
// patches
    else if ( visibleInfo == patchpkgsItem )
    {
	// show a package table with packages belonging to a patch
	YTableHeader * tableHeader = new YTableHeader();
	patchPkgs =  new NCPkgTable( replacePoint, tableHeader );

	if ( patchPkgs )
	{
	    patchPkgs->setSize( patchPkgs->preferredWidth(), patchPkgs->preferredHeight() );
	    patchPkgs->Redraw();
	
	    // set the connection to the NCPackageSelector !!!!
	    patchPkgs->setPackager( this );
	    // set status strategy - don't set extra strategy, use 'normal' package strategy
	    NCPkgStatusStrategy * strategy = new PackageStatStrategy();
	    patchPkgs->setTableType( NCPkgTable::T_PatchPkgs, strategy );
	    patchPkgs->fillHeader( );

	    fillPatchPackages( patchPkgs, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}
    }
    else if ( visibleInfo == pkgversionsItem ) 
    {
	// show a package table with versions of the packages beloning to a patch
	YTableHeader * tableHeader = new YTableHeader();
	patchPkgsVersions =  new NCPkgTable( replacePoint, tableHeader );

	if ( patchPkgsVersions )
	{
	    patchPkgsVersions->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    patchPkgsVersions->Redraw();

	    patchPkgsVersions->setPackager( this );
	    // set status strategy and table type
	    NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	    patchPkgsVersions->setTableType( NCPkgTable::T_Availables, strategy );

	    patchPkgsVersions->fillHeader( );
	    
	    fillPatchPackages( patchPkgsVersions, packageList->getDataPointer( packageList->getCurrentItem() ), true );
	}
    }	
    else
    {
	// show the rich text widget	
	infoText = new NCRichText( replacePoint, " ");

	if ( infoText )
	{
	    infoText->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    infoText->Redraw();

	    packageList->showInformation( );
	}
    }

    packageList->setKeyboardFocus();

    UIMIL << "Change package info to: " << visibleInfo->label() << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
//
// Dependency Handler
//
//
bool NCPackageSelector::DependencyHandler( const NCursesEvent&  event )
{
    if ( !event.selection )
    {
	return false;
    }

    NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-35)/2 ), "",
					  _( "All package dependencies are OK." ),
					  NCPkgNames::OKLabel() );
    info->setNiceSize( 35, 5 );

    if ( event.selection == showdepsItem )
    {
	bool ok = false;

	if ( depsPopup )
	{
	    NCMIL << "Checking dependencies" << endl;
	    depsPopup->showDependencies( NCPkgPopupDeps::S_Solve, &ok );
	}

	if ( ok )
	{
	    info->showInfoPopup();
	    YDialog::deleteTopmostDialog();
    	}

	// update the package list and the disk space info
	updatePackageList();
	showDiskSpace();	
    }
    else if ( event.selection == verifyItem )
    {
	verifyPackageDependencies();
	updatePackageList();
	showDiskSpace();
    }
    else if ( event.selection == autodepsItem )
    {
	etcMenu->deleteAllItems();

	autoCheck = false;	// reset autoCheck and create new items
	createEtcMenu();
    }
    else if ( event.selection == noautodepsItem )
    {
	etcMenu->deleteAllItems();

	autoCheck = true;	// set autoCheck and create new items
	createEtcMenu();
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
	 || !event.selection )
    {
	return false;
    }

    string selId = getMenuId( event.selection );

    if ( event.selection == groupsItem )
    {
	if ( filterPopup )
	{
	    NCMIL << "Showing RPM groups" << endl;
	    // show the filter popup (fills the package list) 
	    retEvent = filterPopup->showFilterPopup( );
	}
    }
    // FIXME - add selectionsItem if really needed
    // else if ( event.selection == selectionsItem )
    else if ( event.selection == patternsItem )
    {
	if ( patternPopup )
	{
	    // show the selection popup
	    retEvent = patternPopup->showSelectionPopup( );
	}
    }
    else if ( event.selection == languagesItem )
    {
	if ( languagePopup )
	{
	    // show the selection popup
	    retEvent = languagePopup->showSelectionPopup( );
	}
    }
    else if ( event.selection == reposItem ) 
    {
       if ( repoPopup )
       {
           // show the selection popup
           retEvent = repoPopup->showRepoPopup( );
       }
    }
    else if ( event.selection == searchItem ) 
    {
        // start package search
	SearchHandler( event );
    }
// patches
// FIXME - compare event.selection with YMenuItem (see above)
    else if ( selId == NCPkgNames::Recommended()->toString() )
    {
	fillPatchList( "recommended" );	// patch kind
    }
    else if ( selId == NCPkgNames::Security()->toString() )
    {
	fillPatchList( "security" );		// patch kind
    }
    else if ( selId ==  NCPkgNames::Optional()->toString() )
    {
	fillPatchList( "optional" );		// patch kind
    }
    else if ( selId ==  NCPkgNames::YaST2Patches()->toString() )
    {
	fillPatchList( "YaST2" );		// patch kind
    } 
    else if ( selId ==  NCPkgNames::AllPatches()->toString() )
    {
	fillPatchList( "all" );			// show all patches
    }
    else if ( selId == NCPkgNames::InstalledPatches()->toString() )
    {
	fillPatchList( "installed" );		// show installed patches
    }
    else if ( selId == NCPkgNames::InstallablePatches()->toString() )
    {
	fillPatchList( "installable" );		// show installed patches
    }
    else if ( selId == NCPkgNames::NewPatches()->toString() )
    {
	fillPatchList( "new" );			// show new patches
    }
 // patches end
    else if ( event.selection == updatelistItem )
    {
	fillUpdateList();
    }
    else if ( event.selection == whatifItem )
    {
	fillSummaryList( NCPkgTable::L_Changes );
    }
    else if ( event.selection == installedItem )
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
	 || !event.selection )
    {
	return false;
    }
    
    if (  packageList->getNumLines() == 0 )
    {
	// nothing to do
	return true;
    }

    string selId = getMenuId( event.selection );
    
    // call the corresponding method of NCPkgTable
    if ( event.selection == toggleItem )
    {
	packageList->toggleObjStatus( );
    }
    else if ( event.selection == selectItem )
    {
	if ( testMode )
	    diskspacePopup->setDiskSpace( '+' );
	else
	    packageList->changeObjStatus( '+' );
    }
    else if ( event.selection == deleteItem )
    {
	if ( testMode )
	    diskspacePopup->setDiskSpace( '-' );
	else
	    packageList->changeObjStatus( '-' );
    }
    else if ( event.selection == updateItem )
    {
	if ( testMode )
	    diskspacePopup->checkDiskSpaceRange();
	else
	    packageList->changeObjStatus( '>' );
    }
    else if ( event.selection == tabooItem )
    {
	packageList->changeObjStatus( '!' );	
    }
    else if ( event.selection ==   notabooItem )
    {
	packageList->changeObjStatus( '%' );	
    }
#if 0
    // no source install available
    else if (event.selection ==  NCPkgNames::SourceYes()->toString() )
    {
	packageList->SourceInstall( true );	
    }
    else if (event.selection ==  NCPkgNames::SourceNo()->toString() )
    {
	packageList->SourceInstall( false );	
    }
#endif
    else if ( event.selection == selallItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Install );
    }
    else if (event.selection == deselallItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontInstall );
    }
    else if (event.selection == delallItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Delete );
    }
    else if (event.selection == dontdelItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontDelete );
    }
    else if ( event.selection == updnewerItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_UpdateNewer );
    }
    else if (event.selection == updallItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Update );
    }
    else if (event.selection ==  dontupdItem )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontUpdate );
    }
    
    packageList->setKeyboardFocus();
    
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
	    NCMIL << "Package " << pkgName << " found" << endl;
	    // open popup with package info
	    NCPkgPopupDescr * popupDescr = new NCPkgPopupDescr( wpos(1,1), this );
	    popupDescr->showInfoPopup( pkgPtr, *i );

	    YDialog::deleteTopmostDialog();

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
	NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-8)/2, (NCurses::cols()-40)/2 ),
					      "",
					      _("Dependency resolver test case written to ") + "<br>"
					      + testCaseDir
					      );
	info->setNiceSize( 40, 8 );
	info->showInfoPopup( );

	YDialog::deleteTopmostDialog();
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
    if ( !event.selection )
    {
 	return false;
    }

    //Export package list into file	
    if ( event.selection == exportItem )
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
		NCPopupInfo * errorMsg = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2 ),
							  NCPkgNames::ErrorLabel(),
							  _("Error exporting list of packages and patterns to ")
							  + filename->toString(),
							  NCPkgNames::OKLabel(),
							  "");
	        errorMsg->setNiceSize(40,5);
		NCursesEvent input = errorMsg->showInfoPopup();

		YDialog::deleteTopmostDialog();
	    }
	
	    return true;
        } 
    }

    //Import package list from file
    else if ( event.selection == importItem )
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

		NCPopupInfo * errorMsg = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-40)/2) ,
							  NCPkgNames::ErrorLabel(),
							  _("Error importing list of packages and patterns from ")
							  + filename->toString(),
							  NCPkgNames::OKLabel(),
							  "");
	        errorMsg->setNiceSize(40,5);
		NCursesEvent input = errorMsg->showInfoPopup();

		YDialog::deleteTopmostDialog();
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
    string headline = NCPkgNames::PackageHelp();
    
    if ( !event.selection )
    {
	return false;
    }

    string selId = getMenuId( event.selection );
    
    if ( selId ==  NCPkgNames::GeneralHelp()->toString() )
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
    else if ( selId == NCPkgNames::StatusHelp()->toString() )
    {
	text += NCPkgNames::HelpOnStatus1();
	text += NCPkgNames::HelpOnStatus2();
	text += NCPkgNames::HelpOnStatus3();
	text += NCPkgNames::HelpOnStatus4();
	text += NCPkgNames::HelpOnStatus5();
	text += NCPkgNames::HelpOnStatus6();
	text += NCPkgNames::HelpOnStatus7();
    }
    else if ( selId == NCPkgNames::UpdateHelp()->toString() )
    {
	text += NCPkgNames::HelpOnUpdate();
    }
    else if ( selId == NCPkgNames::SearchHelp()->toString() )
    {
	headline = NCPkgNames::SearchHeadline();
	text += NCPkgNames::HelpOnSearch();
    }
    
    // open the popup with the help text
    NCPopupInfo * pkgHelp = new NCPopupInfo( wpos( (NCurses::lines()*8)/100, (NCurses::cols()*18)/100 ),
					     headline,
					     text
					     );
    pkgHelp->setNiceSize( (NCurses::cols()*65)/100, (NCurses::lines()*85)/100 );
    pkgHelp->showInfoPopup( );

    YDialog::deleteTopmostDialog();
    
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
    NCPopupInfo * youHelp = new NCPopupInfo( wpos( NCurses::lines()/3, NCurses::cols()/6 ),
					     NCPkgNames::YouHelp(),
					     text
					     );
    youHelp->setNiceSize( (NCurses::cols()*2)/3, NCurses::lines()/3 );
    youHelp->showInfoPopup( );

    YDialog::deleteTopmostDialog();
 
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
	NCPopupInfo * cancelMsg = new NCPopupInfo( wpos( (NCurses::lines()-8)/2, (NCurses::cols()-45)/2 ),
						   NCPkgNames::NotifyLabel(),
						   NCPkgNames::CancelText(),
						   NCPkgNames::YesLabel(),
						   NCPkgNames::NoLabel()
						   );
	cancelMsg->setNiceSize( 45, 8 ); 
	NCursesEvent input = cancelMsg->showInfoPopup( );

	YDialog::deleteTopmostDialog();
	 
	if ( input == NCursesEvent::cancel ) {
	    // don't leave the package installation dialog
	    return true;
	}
    }

    restoreState ();

    NCMIL <<  "Cancel button pressed - leaving package selection" << endl;
    const_cast<NCursesEvent &>(event).result = "cancel";
    
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
	NCPkgPopupTable * autoChangePopup = new NCPkgPopupTable( wpos( 3, 8), this );
	NCursesEvent input = autoChangePopup->showInfoPopup();

	YDialog::deleteTopmostDialog();

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
	    NCPopupInfo * spaceMsg = new NCPopupInfo( wpos( (NCurses::lines()-10)/2, (NCurses::cols()-50)/2 ),
						      NCPkgNames::ErrorLabel(),
						      NCPkgNames::DiskSpaceError() + "<br>" + message,
						      NCPkgNames::OKLabel(),
						      NCPkgNames::CancelLabel() );
	    
	    spaceMsg->setNiceSize( 50, 10 ); 
	    NCursesEvent input = spaceMsg->showInfoPopup( );

	    YDialog::deleteTopmostDialog();

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

	const_cast<NCursesEvent &>(event).result = "accept"; 
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

    NCPopupInfo * info = new NCPopupInfo ( wpos( NCurses::lines()/10, NCurses::cols()/10),
					   NCPkgNames::NotifyLabel(),
					   "<i>" + pkgName + "</i><br><br>"
					   + createDescrText( licenseText ),
					   NCPkgNames::AcceptLabel(),
					   NCPkgNames::CancelLabel(),
					   NCPkgNames::PrintLicenseText() );

    info->setNiceSize( (NCurses::cols() * 80)/100, (NCurses::lines()*80)/100);
    license_confirmed = info->showInfoPopup( ) != NCursesEvent::cancel;

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

    YDialog::deleteTopmostDialog();
     
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

    // if (  visibleInfo->compare( NCPkgNames::PatchDescr() ) == YO_EQUAL )
    if ( visibleInfo == patchdescrItem ) 
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
	if ( infoText )
	{
	    infoText->setValue( descr );
	}	
    }
    // else if (  visibleInfo->compare( NCPkgNames::PatchPackages() ) == YO_EQUAL )
    else if ( visibleInfo == patchpkgsItem )
    {
	if ( patchPkgs )
	{
	    fillPatchPackages ( patchPkgs, objPtr);
	}
    }
    // else if (  visibleInfo->compare( NCPkgNames::PatchPackagesVersions() ) == YO_EQUAL )
    else if ( visibleInfo ==  pkgversionsItem )
    {
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
    
    NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-30)/2 ),
					  "",
					  _( "System dependencies verify OK." ),
					  NCPkgNames::OKLabel()
					  );
    info->setNiceSize( 30, 5 );
    
    NCMIL << "Verifying system" << endl;

    if ( depsPopup )
    {
	saveState();
	//call the solver (with S_Verify it displays no popup)
	cancel = depsPopup->showDependencies( NCPkgPopupDeps::S_Verify, &ok );

       //display the popup with automatic changes
	NCPkgPopupTable * autoChangePopup = new NCPkgPopupTable( wpos( 3, 8 ), this );
        NCursesEvent input = autoChangePopup->showInfoPopup();

        if ( input == NCursesEvent::cancel )
        {
            // user clicked on Cancel
	    restoreState();
            cancel = true;
        }
	if ( ok && input == NCursesEvent::button )
	{
	    // dependencies OK, no automatic changes/the user has accepted the changes
	    info->showInfoPopup();
	}
    }

    YDialog::deleteTopmostDialog();	// delete NCPopupInfo dialog

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

    if ( visibleInfo == longdescrItem )
    {
	// ask the package manager for the description of this package
	zypp::Text value = pkgPtr->description();
	string descr = createDescrText( value );
	NCDBG << "Description: " << descr << endl;
	
        // show the description	
	if ( infoText )
	{
	    infoText->setValue( descr );
	}
    }
    else if ( visibleInfo == filesItem )
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
	if ( infoText )
	{
	    infoText->setValue( text );
	}	
    }
    else if ( visibleInfo == pkginfoItem )
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
	if ( infoText )
	{
	    infoText->setValue( text );
	}
    }
    else if ( visibleInfo == versionsItem )
    {
	if ( versionsList )
	{
	    fillAvailableList( versionsList, slbPtr );
	}
    }
    else if ( visibleInfo == relationsItem )
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
	if ( infoText )
	{
	    infoText->setValue( text );
	}
    }
 
    NCDBG <<  "Showing package information: " << visibleInfo << endl;
    
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
#warning gcc complains here - disabled milestone logging for now
	// UIMIL << *it << endl;
	diff += (it->pkg_size - it->used_size) * 1024;
    }

    // show pkg_diff, i.e. total difference of disk space (can be negative in installed system
    // if packages are deleted)
    if ( diskspaceLabel )
    {
	diskspaceLabel->setText( diff.asString() );
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

    // show the download size
    if ( diskspaceLabel )
    {
	diskspaceLabel->setText( totalSize.asString() );
    }
}


///////////////////////////////////////////////////////////////////
//
// getPackageList()
//
NCPkgTable * NCPackageSelector::getPackageList()
{
    return pkgList;
}

//
// Create layout for Online Update
//
void NCPackageSelector::createYouLayout( YWidget * selector, NCPkgTable::NCPkgTableType type )
{
    // TODO
}

//
// Create layout for Package Selector
//
void NCPackageSelector::createPkgLayout( YWidget * selector, NCPkgTable::NCPkgTableType type )
{
     // the vertical split is the (only) child of the dialog
    YLayoutBox * split = YUI::widgetFactory()->createVBox( selector );

    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( split );

    YAlignment * left1 = YUI::widgetFactory()->createLeft( hSplit );
			
    // label of the filter menu ( keep it short ) - filters out a set of packages
    filterMenu = new NCMenuButton( left1, _("&Filter") );
    YUI_CHECK_NEW( filterMenu );
    filterMenu->setFunctionKey( 4 );

    // begin: menu items of the filter menu
    // please note: use unique hotkeys until end:  
    groupsItem = new YMenuItem( _("RPM &Groups") );
    patternsItem = new YMenuItem( _("Pa&tterns") );
    languagesItem = new YMenuItem( _("&Languages") );
    reposItem =  new YMenuItem( _("&Repositories" ) );
    searchItem =  new YMenuItem( _("&Search" ) );
    installedItem = new YMenuItem( _( "Installed &Packages" ) );
    whatifItem = new YMenuItem(	_( "&Installation Summary" ) );
    // end: menu items of the filter menu
    updatelistItem = new YMenuItem(	_( "&Update List" ) );

    YItemCollection itemCollection;
    itemCollection.push_back( groupsItem );
    itemCollection.push_back( patternsItem );
    itemCollection.push_back( languagesItem );
    itemCollection.push_back( reposItem );
    itemCollection.push_back( searchItem );
    itemCollection.push_back( installedItem );
    itemCollection.push_back( whatifItem );
    itemCollection.push_back( updatelistItem );
    filterMenu->addItems( itemCollection );

    YAlignment * left2 = YUI::widgetFactory()->createLeft( hSplit );

    // label Actions menu ( keep it short ) - actions to change status of a package
    actionMenu = new NCMenuButton( left2,  _( "A&ctions" ) );
    YUI_CHECK_NEW( actionMenu );
    actionMenu->setFunctionKey( 5 );

    // Please note: add an appropriate number of whitespaces to get a well
    // formated menu (the [ ]s should be in one column) and use unique hotkeys until end:
    // begin: Actions menu, toggle the status, e.g. change from installed to delete
    toggleItem =  new YMenuItem( _( "&Toggle    [SPACE]" ) );
    selectItem =  new YMenuItem( _( "&Select      [+]" ) );
    deleteItem =  new YMenuItem( _( "&Delete      [-]" ) );
    updateItem =  new YMenuItem( _( "&Update      [>]" ) );
    tabooItem =   new YMenuItem( _( "Taboo &Yes   [!]" ) );
    notabooItem = new YMenuItem( _( "Taboo &No    [%]" ) );
    // end: Actions menu, set status of all packages (title of a submenu)
    allItem =     new YMenuItem( _( "All Listed &Packages" ) );

    YItemCollection itemCollection2;
    itemCollection2.push_back( toggleItem );
    itemCollection2.push_back( selectItem );
    itemCollection2.push_back( deleteItem );
    itemCollection2.push_back( updateItem );
    itemCollection2.push_back( tabooItem );
    itemCollection2.push_back( notabooItem );
    itemCollection2.push_back( allItem );

    // begin: submenu items actions concerning all packages
    // please note: use unique hotkeys until end:
    selallItem = new YMenuItem( allItem, _( "&Install All" ) );
    deselallItem = new YMenuItem( allItem, _( "Do &Not Install Any" ) );
    delallItem = new YMenuItem( allItem,  _( "&Delete All" ) );
    dontdelItem = new YMenuItem( allItem,  _( "Do Not D&elete Any" ) );
    updnewerItem = new YMenuItem( allItem,   _( "&Update If Newer Version Available" ) );
    updallItem = new YMenuItem( allItem, _( "U&pdate All Unconditionally") );
     // end: submenu items: actions concerning all packages
    dontupdItem = new YMenuItem( allItem,  _( "Do Not Update and &Keep Installed" ) );

    actionMenu->addItems( itemCollection2 );
    
    YAlignment * left3 = YUI::widgetFactory()->createLeft( hSplit );

    // label Information menu ( keep it short! )
    infoMenu = new NCMenuButton( left3, _( "&Information" ) );
    YUI_CHECK_NEW( infoMenu );
    infoMenu->setFunctionKey( 6 );

    // begin: menu items of the info menu
    // please note: use unique hotkeys until end:
    pkginfoItem = new YMenuItem( _( "&Package Info" ) );
    longdescrItem = new YMenuItem( _( "&Long Description" ) );
    versionsItem = new YMenuItem( _( "&Versions" ) );
    filesItem = new YMenuItem( _( "&File List" ) );
    // end: menu items of the info menu
    relationsItem = new YMenuItem( _( "Package &Relations" ) );

    YItemCollection itemCollection3;
    itemCollection3.push_back( pkginfoItem );
    itemCollection3.push_back( longdescrItem );
    itemCollection3.push_back( versionsItem );
    itemCollection3.push_back( filesItem );
    itemCollection3.push_back( relationsItem );
    infoMenu->addItems( itemCollection3 );

    YAlignment * left4 = YUI::widgetFactory()->createLeft( hSplit );

    // label Etc. menu ( keep it short! )
    etcMenu = new NCMenuButton( left4,  _( "&Etc." ) );
    YUI_CHECK_NEW( etcMenu );
    etcMenu->setFunctionKey( 7 );
    // add items to Etc. menu
    createEtcMenu();
    
    // add the package table 
    YTableHeader * tableHeader = new YTableHeader();

    pkgList = new NCPkgTable( split, tableHeader );
    YUI_CHECK_NEW( pkgList );
    
    NCPkgStatusStrategy * strategy;
    // set the table type
    switch ( type )
    {
	case NCPkgTable::T_Patches:
	    strategy = new PatchStatStrategy();
	    pkgList->setTableType( NCPkgTable::T_Patches, strategy );
	case NCPkgTable::T_Update:
	    strategy = new UpdateStatStrategy();
	    pkgList->setTableType( NCPkgTable::T_Update, strategy );
	default:
	    strategy = new PackageStatStrategy();
	    pkgList->setTableType( NCPkgTable::T_Packages, strategy );
    }
    // set the pointer to the packager object
    pkgList->setPackager( this );
    
    // HBox for Filter and Disk Space (both in additional HBoxes )
    YLayoutBox * hSplit2 = YUI::widgetFactory()->createHBox( split );

    YLayoutBox * hSplit3 = YUI::widgetFactory()->createHBox( hSplit2 );
    // label text - keep it short
    new NCLabel( hSplit3,  _( "Filter: " ) );
    filterLabel = YUI::widgetFactory()->createLabel ( hSplit3, "....................................." );
    
    new NCSpacing( hSplit2, YD_HORIZ, true, 0.5 );

    YLayoutBox * hSplit4 = YUI::widgetFactory()->createHBox( hSplit2 );
    // label text - keep it short (use abbreviation if necessary)
    new NCLabel( hSplit4,   _( "Required Disk Space: " ) );
    diskspaceLabel = YUI::widgetFactory()->createLabel ( hSplit4, "   " );

    YLayoutBox * vSplit = YUI::widgetFactory()->createVBox( split );
    replacePoint = YUI::widgetFactory()->createReplacePoint( vSplit );
    
    infoText = new NCRichText( replacePoint, " " );
    YUI_CHECK_NEW( infoText );
    
    YLayoutBox * hSplit5 = YUI::widgetFactory()->createHBox( vSplit );
    
    // add the Cancel button
    cancelButton = new NCPushButton( hSplit5, _( "&Cancel" ) );
    YUI_CHECK_NEW( cancelButton );
    cancelButton->setFunctionKey( 9 );

    // add the OK button
    okButton = new NCPushButton( hSplit5, _( "&Accept" ) );
    YUI_CHECK_NEW( okButton );
    okButton->setFunctionKey( 10 );
    
}

void NCPackageSelector::createEtcMenu( )
{
     // menu item of the Etc. menu - package dependency check
    depsItem = new YMenuItem( _( "&Dependencies" ) );
    // menu items of the Etc./Dependencies submenu
    showdepsItem = new YMenuItem( depsItem, _( "    &Check Dependencies Now" ) );
    if ( autoCheck )
	// menu entry: dependency check off
	autodepsItem = new YMenuItem( depsItem, _( "[X] &Automatic Dependency Check" ) );
    else
	// menu entry: dependency check on
	noautodepsItem = new YMenuItem( depsItem, _( "[ ] &Automatic Dependency Check" ) );
    
    verifyItem =   new YMenuItem( depsItem, _( "    &Verify System" ) );
    // menu item - list of all packages in the system
    allpksItem = new YMenuItem( _("All &Packages List" ) );
    exportItem = new YMenuItem( allpksItem, _("&Export to File") );
    importItem = new YMenuItem( allpksItem, _("&Import from File") );
    testcaseItem = new YMenuItem( _( "Generate Dependency Resolver &Test Case" ) );
    
    YItemCollection itemCollection4;
    itemCollection4.push_back( depsItem );
    itemCollection4.push_back( allpksItem );
    itemCollection4.push_back( testcaseItem );
    etcMenu->addItems( itemCollection4 );  
}

//
// Fill package list with packages of default RPM group/update list or installable patches
//
bool NCPackageSelector::fillDefaultList( )
{
    if ( !pkgList )
	return false;

    NCMIL << "Fill list: " << pkgList << endl;
    switch ( pkgList->getTableType() )
    {
	case NCPkgTable::T_Patches: {
	    fillPatchList( "installable" );	// default: installable patches

	    // set the visible info to long description 
	    setVisibleInfo ( longdescrItem );

	    // show the package description of the current item
	    pkgList->showInformation ();
	    break;
	}
	case NCPkgTable::T_Update: {
	    if ( ! zypp::getZYpp()->resolver()->problematicUpdateItems().empty() )
	    {
		fillUpdateList();
		// set the visible info to package description 
		setVisibleInfo ( pkginfoItem );
		// show the package description of the current item
		pkgList->showInformation ();
		break;
	    }
	}
	case NCPkgTable::T_Packages: {
	    YStringTreeItem * defaultGroup = getDefaultRpmGroup();

	    if ( defaultGroup )
	    {
		NCMIL << "default RPM group: " << defaultGroup->value().translation() << endl;
		fillPackageList ( YCPString( defaultGroup->value().translation()),
					    defaultGroup );
		
		// set the visible info to package description 
		setVisibleInfo ( pkginfoItem );
		// show the package description of the current item
		pkgList->showInformation ();
	    }
	    else
	    {
		NCERR << "No default RPM group available" << endl;
	    }
	    break;
	}
	default:
	    break;
    }

    pkgList->setKeyboardFocus();

    return true;
}
