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
//#include "NCPopupTree.h"
#include "NCMenuButton.h"
#include "NCPopupSelection.h"
#include "NCPopupDeps.h"
#include "NCPopupDiskspace.h"
#include "NCPopupPkgTable.h"
#include "NCPopupPkgDescr.h"
#include "NCPopupFile.h"
#include "PackageSelector.h"
#include "YSelectionBox.h"

#include <iomanip>
#include <list>
#include <string>
#include <set>

#include "YQZypp.h"		// tryCastToZyppPkg(), tryCastToZyppPatch()
#include <zypp/ui/Selectable.h>
#include <zypp/ui/PatchContents.h>

typedef zypp::ui::PatchContents			ZyppPatchContents;
typedef zypp::ui::PatchContents::const_iterator	ZyppPatchContentsIterator;

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>

using namespace std;
using std::string;
using std::wstring;

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
PackageSelector::PackageSelector( YNCursesUI * ui, const YWidgetOpt & opt, string floppyDevice )
    : y2ui( ui )
      , visibleInfo( YCPNull() )
      , filterPopup( 0 )
      , depsPopup( 0 )
      , selectionPopup( 0 )
      , diskspacePopup( 0 )
      , searchPopup( 0 )
      , filePopup( 0 )
      , youMode( false )
      , updateMode( false )
      , autoCheck( true )
      , _rpmGroupsTree (0)
{
    // Fill the handler map
    eventHandlerMap[ PkgNames::Packages()->toString() ]	= &PackageSelector::PackageListHandler;
    eventHandlerMap[ PkgNames::Cancel()->toString() ] 	= &PackageSelector::CancelHandler;
    eventHandlerMap[ PkgNames::OkButton()->toString() ]	= &PackageSelector::OkButtonHandler;
    eventHandlerMap[ PkgNames::Search()->toString() ] 	= &PackageSelector::SearchHandler;
    eventHandlerMap[ PkgNames::Diskinfo()->toString() ] = &PackageSelector::DiskinfoHandler;
    // Filter menu
    eventHandlerMap[ PkgNames::RpmGroups()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Selections()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::UpdateList()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Installed()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Whatif()->toString() ] = &PackageSelector::FilterHandler;

    // YOU filter
    eventHandlerMap[ PkgNames::Recommended()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Security()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Optional()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::InstalledPatches()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::AllPatches()->toString() ] = &PackageSelector::FilterHandler; 
    eventHandlerMap[ PkgNames::NewPatches()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::InstalledPatches()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::InstallablePatches()->toString() ] = &PackageSelector::FilterHandler;    
    eventHandlerMap[ PkgNames::YaST2Patches()->toString() ] = &PackageSelector::FilterHandler;

    // Information menu
    eventHandlerMap[ PkgNames::Files()->toString() ]   	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PkgInfo()->toString() ] 	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::LongDescr()->toString() ]= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Versions()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Relations()->toString() ] = &PackageSelector::InformationHandler;

    // YOU information 
    eventHandlerMap[ PkgNames::PatchDescr()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PatchPackages()->toString() ] = &PackageSelector::InformationHandler;
    
    // Action menu
    eventHandlerMap[ PkgNames::Toggle()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Select()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Delete()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Update()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::TabooOn()->toString() ]	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::TabooOff()->toString() ]	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::SourceYes()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::SourceNo()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::InstallAll()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::DontInstall()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::DeleteAll()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::DontDelete()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::UpdateAll()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::DontUpdate()->toString() ] = &PackageSelector::StatusHandler;
    
    // Etc. menu
    eventHandlerMap[ PkgNames::ShowDeps()->toString() ] = &PackageSelector::DependencyHandler;
    eventHandlerMap[ PkgNames::AutoDeps()->toString() ] = &PackageSelector::DependencyHandler;
    eventHandlerMap[ PkgNames::SaveSel()->toString() ] = &PackageSelector::SelectionHandler;
    eventHandlerMap[ PkgNames::LoadSel()->toString() ] = &PackageSelector::SelectionHandler;
    
    // Help menu
    eventHandlerMap[ PkgNames::GeneralHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::StatusHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::FilterHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::UpdateHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::SearchHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::PatchHelp()->toString() ]  = &PackageSelector::YouHelpHandler;

    if ( opt.youMode.value() )
	youMode = true;

    if ( opt.updateMode.value() )
	updateMode = true;


    // read test source information
    if ( opt.testMode.value() )
    {
#ifdef FIXME_PATCHES
	if ( youMode )
	{
	    PMYouServer server( "dir:///8.1-patches" );
            InstYou &you = Y2PM::youPatchManager().instYou();
            you.settings()->setPatchServer( server );
	    you.settings()->setReloadPatches( false );
            you.retrievePatchInfo();
	    you.selectPatches( zypp::Patch::kind_recommended |
			       zypp::Patch::kind_security     );
	    NCMIL <<  "Fake YOU patches initialized" << endl;	
	}
	else
#endif
	{
#ifdef FIXME
	    Y2PM y2pm;
	    InstSrcManager& MGR = y2pm.instSrcManager();

	    Url url( "dir:///space/instTest" );

	    InstSrcManager::ISrcIdList nids;
	    PMError err = MGR.scanMedia( nids, url );

	    if ( nids.size() )
	    {
		err = MGR.enableSource( *nids.begin() );
		NCMIL << "Fake source enabled: " << err << endl;
	    }
#endif
	}
    }

    saveState ();

    if ( !youMode )
    {
	// create the selections popup
	selectionPopup = new NCPopupSelection( wpos( 1, 1 ), this );

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
	// create the filter popup
	filterPopup = new NCPopupTree( wpos( 1, 1 ),  this );	 

	// create the search popup
	searchPopup = new NCPopupSearch( wpos( 1, 1 ), this );

        // the file popup
	filePopup = new NCPopupFile( wpos( 1, 1), floppyDevice, this );
    }
	
    // the dependency popups
    depsPopup = new NCPopupDeps( wpos( 1, 1 ), this );

    // the disk space popup
    diskspacePopup = new NCPopupDiskspace( wpos( 1, 1 ) );

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
    if ( depsPopup )
    {
	delete depsPopup;	
    }
    if ( diskspacePopup )
    {
	delete diskspacePopup;
    }
    if ( filePopup )
    {
	delete filePopup;
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

///////////////////////////////////////////////////////////////////
//
// detection whether the user has made any changes
//
 
void PackageSelector::saveState ()
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

void PackageSelector::restoreState ()
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

bool PackageSelector::diffState ()
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
void PackageSelector::setVisibleInfo( const YCPValue & info )
{
    visibleInfo = info;
}

///////////////////////////////////////////////////////////////////
//
// fillAvailableList
//
// Fills the package table (on bottom ) with the list of available packages 
//
bool PackageSelector::fillAvailableList( NCPkgTable * pkgTable, ZyppSel selectable )
{
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
	pkgTable->createListEntry( tryCastToZyppPkg (*it), selectable );
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
bool PackageSelector::showSelPackages( const YCPString & label, const set<string> & wanted )
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
// fillSearchList
//
// Fills the package table
//
bool PackageSelector::fillSearchList( const YCPString & expr,
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
	pkg = tryCastToZyppPkg ((*listIt)->theObj());

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
	
	++listIt;
    }

    // show the package list
    packageList->drawList();
    
    // set filter label to 'Search'
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(PkgNames::SearchResults()) );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchList
//
// Fills the package table with the list of YOU patches
//
bool PackageSelector::fillPatchList( string filter )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear list of patches
    packageList->itemsCleared ();

#ifdef FIXME
    // get the patch list and sort it
    list<ZyppSel> patchList( zyppPatchesBegin (), zyppPatchesEnd () );
    patchList.sort( sortByName );
    list<ZyppSel>::iterator listIt = patchList.begin(); 
#endif
    
    for ( ZyppPoolIterator listIt = zyppPatchesBegin();
	  listIt != zyppPatchesEnd();
	  ++listIt )
    {
	ZyppPatch patchPtr  = tryCastToZyppPatch( ( *listIt)->theObj() );

	if ( patchPtr )
	{
	    checkPatch( patchPtr, *listIt, filter );
	}
		
    }
    
    if ( filter == "installable"
	 && packageList->getNumLines() == 0 )
    {
	packageList->createInfoEntry( PkgNames::UpToDate() );	
    }
    
    // show the patches
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	if ( filter == "installable" )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(PkgNames::YOUPatches()) );
	}
	else if ( filter == "installed" )
	{
  	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(PkgNames::InstPatches()) );  
	}
	else
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(PkgNames::Patches()) );  
	}
    }
    
    return true;
}


///////////////////////////////////////////////////////////////////
//
// fillUpdateList
//
//
bool PackageSelector::fillUpdateList( )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    ZyppPoolIterator b = zyppPkgBegin ();
    ZyppPoolIterator e = zyppPkgEnd ();
    ZyppPoolIterator i;

    // FIXME, how is an update problem defined?
    // see packagemanager/src/pkg/PMPackageManager_update.cc _update_items
    UIINT << "Update problems not defined!" << endl;

    for (i = b; i != e; ++i)
    {
	ZyppSel slb = *i;
	ZyppPkg pkg = tryCastToZyppPkg (slb->theObj ()); // ??

	zypp::ui::Status st = slb->status ();
	switch (st) {
	    case S_Protected:
	    case S_Taboo:
	    case S_AutoDel:
		packageList->createListEntry( pkg, slb );
		break;
	    default:
		break;
	}
    }

    // show the list
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString(PkgNames::UpdateProblem()) );
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchPackages
//
//
bool PackageSelector::fillPatchPackages ( NCPkgTable * pkgTable, ZyppObj objPtr )
{
    if ( !pkgTable )
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
	    NCDBG << "Patch package found: " <<  (*it)->name().c_str() << endl;

	    ZyppSel sel = selMapper.findZyppSel( pkg );

	    if ( sel )
		if ( contains( patchSelectables, sel ) )
		{
		    NCMIL << "Suppressing duplicate selectable: " << (*it)->name().c_str() << "-" <<
			(*it)->edition().asString().c_str() << " " <<
			(*it)->arch().asString().c_str() << endl;
		}
		else
		{
		    patchSelectables.insert( sel );
		    pkgTable->createListEntry( pkg, sel );			    
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
		pkgLine.push_back( PkgNames::Script() );
	
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

#ifdef OLD_CODE
    list<ZyppPkg> packages = patchPtr->packages();
    string preScript = patchPtr->preScript();
    string postScript = patchPtr->postScript();
    list<PMYouFile> files = patchPtr->files();

    list<ZyppPkg>::const_iterator listIt;
    list<PMYouFile>::const_iterator fileIt;
    
    NCMIL << "Number of patch packages: " << packages.size() << endl;
	
    for ( listIt = packages.begin(); listIt != packages.end();  ++listIt )    
    {
	pkgTable->createListEntry( (*listIt) );
    }
    for ( fileIt = files.begin(); fileIt != files.end(); ++fileIt )
    {
	vector<string> pkgLine;
	pkgLine.reserve(4);

	pkgLine.push_back( fileIt->name() );
	pkgLine.push_back( "   " );	// versions empty
	pkgLine.push_back( "   " );	
	pkgLine.push_back( PkgNames::File() );	// additional file
	
	pkgTable->addLine( S_NoInst,
			   pkgLine,
			   ZyppObj()
			   );	
    }

    if ( preScript != "" )
    {
	vector<string> pkgLine;
	pkgLine.reserve(4);

	pkgLine.push_back( preScript ); 
	pkgLine.push_back( "   " );	// versions empty
	pkgLine.push_back( "   " );	
	pkgLine.push_back( PkgNames::PreScript() );
	
	pkgTable->addLine( S_NoInst,
			   pkgLine,
			   ZyppObj()
			   );
    }
    if ( postScript != "" )
    {
	vector<string> pkgLine;
	pkgLine.reserve(4);

	pkgLine.push_back(  postScript );
	pkgLine.push_back( "   " );	// versions empty
	pkgLine.push_back( "   " );	
	pkgLine.push_back( PkgNames::PostScript() );
	
	pkgTable->addLine( S_NoInst,
			   pkgLine,
			   ZyppObj()
			   );
    }
#endif
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
bool PackageSelector::fillSummaryList( NCPkgTable::NCPkgTableListType type )
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
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( PkgNames::InstSummary() );
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
bool PackageSelector::fillPackageList( const YCPString & label, YStringTreeItem * rpmGroup )
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
// match
//

bool PackageSelector::match ( string s1, string s2, bool ignoreCase )
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
bool PackageSelector::checkPackage( ZyppObj opkg, ZyppSel slb,
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
bool PackageSelector::checkPatch( ZyppPatch 	patchPtr,
				  ZyppSel	selectable,
				  string 	filter )
				   
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList || !patchPtr
	 || !selectable )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    if ( filter == "all"
	 || ( filter == "installed" && selectable->status() == S_KeepInstalled )
	 // FIXME Is filter installable correct? There was a method installable() before. Condition was:
	 // ( patchPtr->installable() && patchPtr->getSelectable()->status() != PMSelectable::S_KeepInstalled )
	 || ( filter == "installable" && ( selectable->status() != S_KeepInstalled ) )
	 || ( filter == "new" && ( selectable->status() == S_Install ||
				   selectable->status() == S_NoInst ) )
	 || ( filter == "security" && patchPtr->category() == "security" )
	 || ( filter == "recommended" && patchPtr->category() == "recommended" )
	 || ( filter == "optional" && patchPtr->category() == "optional" )
	 || ( filter == "YaST2" && patchPtr->category() == "yast" )

	 )	
    {
	packageList->createPatchEntry( patchPtr, selectable );
	return true;
    }
    else
    {
	return false;
    }
}


///////////////////////////////////////////////////////////////////
//
// SearchHandler
//
// Opens popup for package search and asks the package manager
//
bool PackageSelector::SearchHandler( const NCursesEvent& event)
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
bool PackageSelector::InformationHandler( const NCursesEvent&  event )
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

    if ( visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	// show the package table
	const char * tableLayout = "`PkgSpecial( `id(\"availpkgs\"), `opt(`notify), \"pkgTable\" )"; 
	Parser parser( tableLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();

	y2ui->evaluateReplaceWidget( YCPSymbol("replaceinfo"), layout->asTerm() );

	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));

	if ( pkgAvail )
	{
	    // set the connection to the PackageSelector !!!!
	    pkgAvail->setPackager( this );
	    // set status strategy
	    ObjectStatStrategy * strategy = new AvailableStatStrategy();
	    pkgAvail->setTableType( NCPkgTable::T_Availables, strategy );

	    pkgAvail->fillHeader( );
	    fillAvailableList( pkgAvail, packageList->getSelPointer( packageList->getCurrentItem() ) );
	}
    }
// patches
    else if ( visibleInfo->compare( PkgNames::PatchPackages() ) == YO_EQUAL )
    {
        // show the package table
	const char * tableLayout = "`PkgSpecial( `id(\"patchpkgs\"), `opt(`notify), \"pkgTable\" )"; 
	Parser parser( tableLayout );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();
	if (parsed_code != NULL)
	    layout = parsed_code->evaluate();

	y2ui->evaluateReplaceWidget( YCPSymbol ("replaceinfo"), layout->asTerm() );

	NCPkgTable * patchPkgs = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::PatchPkgs(), true));

	if ( patchPkgs )
	{
	    // set the connection to the PackageSelector !!!!
	    patchPkgs->setPackager( this );
	    // set status strategy
	    ObjectStatStrategy * strategy = new PatchPkgStatStrategy();
	    patchPkgs->setTableType( NCPkgTable::T_PatchPkgs, strategy );

	    patchPkgs->fillHeader( );
	    fillPatchPackages( patchPkgs, packageList->getDataPointer( packageList->getCurrentItem() ) );
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
bool PackageSelector::DependencyHandler( const NCursesEvent&  event )
{
    if ( event.selection.isNull() )
    {
	return false;
    }

    if ( event.selection->compare( PkgNames::ShowDeps() ) == YO_EQUAL )
    {
	// show the dependency popup
	showPackageDependencies( true ); 	// do the check
	// update the package list and the disk space info
	updatePackageList();
	showDiskSpace();	
    }
    else if ( event.selection->compare( PkgNames::AutoDeps() ) == YO_EQUAL )
    {
	char menu[600];
	
	if ( autoCheck )
	{
	    sprintf ( menu,
		      "`MenuButton( \"%s\", ["
		      "`menu( \"%s\", [`item( `id(\"showdeps\"), \"%s\" ), `item( `id(\"autodeps\"), \"%s\" ) ] )"
#ifdef FIXME
		      ","
		      "`menu( \"%s\", [`item( `id(\"save\"), \"%s\" ), `item( `id(\"load\"), \"%s\" ) ] )"
#endif
		      "] )",
		      PkgNames::MenuEtc().c_str(),
		      PkgNames::MenuDeps().c_str(),
		      PkgNames::MenuCheckDeps().c_str(),
		      PkgNames::MenuNoAutoDeps().c_str()
#ifdef FIXME
		      ,
		      PkgNames::MenuSel().c_str(),
		      PkgNames::MenuSaveSel().c_str(),
		      PkgNames::MenuLoadSel().c_str()
#endif
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
	    sprintf ( menu,
		      "`MenuButton( \"%s\", ["
		      "`menu( \"%s\", [`item( `id(\"showdeps\"), \"%s\" ), `item( `id(\"autodeps\"), \"%s\" ) ] )"
#ifdef FIXME
		      ","
		      "`menu( \"%s\", [`item( `id(\"save\"), \"%s\" ), `item( `id(\"load\"), \"%s\" ) ] )"
#endif
		      "] )",
		      PkgNames::MenuEtc().c_str(),
		      PkgNames::MenuDeps().c_str(),
		      PkgNames::MenuCheckDeps().c_str(),
		      PkgNames::MenuAutoDeps().c_str()
#ifdef FIXME
		      ,
		      PkgNames::MenuSel().c_str(),
		      PkgNames::MenuSaveSel().c_str(),
		      PkgNames::MenuLoadSel().c_str()
#endif
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
bool PackageSelector::FilterHandler( const NCursesEvent&  event )
{
    NCursesEvent retEvent;
    NCPkgTable * packageList = getPackageList();
 
    if ( !packageList
	 || event.selection.isNull() )
    {
	return false;
    }

    if ( event.selection->compare( PkgNames::RpmGroups() ) == YO_EQUAL )
    {
	if ( filterPopup )
	{
	    // show the filter popup (fills the package list) 
	    retEvent = filterPopup->showFilterPopup( );
	}
    }
    else if ( event.selection->compare( PkgNames::Selections() ) == YO_EQUAL )
    {
	if ( selectionPopup )
	{
	    // show the selection popup
	    retEvent = selectionPopup->showSelectionPopup( );
	}
    }
// patches
    else if ( event.selection->compare( PkgNames::Recommended() ) ==  YO_EQUAL )
    {
	fillPatchList( "recommended" );	// patch kind
    }
    else if ( event.selection->compare( PkgNames::Security() )  ==  YO_EQUAL )
    {
	fillPatchList( "security" );		// patch kind
    }
    else if ( event.selection->compare( PkgNames::Optional() )  ==  YO_EQUAL )
    {
	fillPatchList( "optional" );		// patch kind
    }
    else if (  event.selection->compare( PkgNames::YaST2Patches() ) ==  YO_EQUAL )
    {
	fillPatchList( "YaST2" );		// patch kind
    } 
    else if ( event.selection->compare( PkgNames::AllPatches() )  ==  YO_EQUAL )
    {
	fillPatchList( "all" );			// show all patches
    }
    else if (  event.selection->compare( PkgNames::InstalledPatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "installed" );		// show installed patches
    }
    else if (  event.selection->compare( PkgNames::InstallablePatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "installable" );		// show installed patches
    }
    else if (  event.selection->compare( PkgNames::NewPatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "new" );			// show new patches
    }
    else if (  event.selection->compare( PkgNames::UpdateList() ) ==  YO_EQUAL )
    {
	fillUpdateList();
    }
    else if (  event.selection->compare( PkgNames::Whatif() ) ==  YO_EQUAL )
    {
	fillSummaryList( NCPkgTable::L_Changes );
    }
    else if (  event.selection->compare( PkgNames::Installed() ) ==  YO_EQUAL )
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
bool PackageSelector::StatusHandler( const NCursesEvent&  event )
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
    if ( event.selection->compare( PkgNames::Toggle() ) == YO_EQUAL )
    {
	packageList->toggleObjStatus( );
    }
    else if ( event.selection->compare( PkgNames::Select() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '+' );
    }
    else if ( event.selection->compare( PkgNames::Delete() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '-' );
    }
    else if ( event.selection->compare( PkgNames::Update() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '>' );
    }
    else if ( event.selection->compare( PkgNames::TabooOn() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '!' );	
    }
    else if ( event.selection->compare( PkgNames::TabooOff() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '-' );	
    } 
    else if ( event.selection->compare( PkgNames::SourceYes() ) == YO_EQUAL )
    {
#ifdef FIXME
	packageList->SourceInstall( true );	
#endif
    }
    else if ( event.selection->compare( PkgNames::SourceNo() ) == YO_EQUAL )
    {
#ifdef FIXME
	packageList->SourceInstall( false );	
#endif
    }	
    else if ( event.selection->compare( PkgNames::InstallAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Install );
    }
    else if ( event.selection->compare( PkgNames::DontInstall() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontInstall );
    }
        else if ( event.selection->compare( PkgNames::DeleteAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Delete );
    }
    else if ( event.selection->compare( PkgNames::DontDelete() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_DontDelete );
    }
    else if ( event.selection->compare( PkgNames::UpdateAll() ) == YO_EQUAL )
    {
	packageList->changeListObjStatus( NCPkgTable::A_Update );
    }
    else if ( event.selection->compare( PkgNames::DontUpdate() ) == YO_EQUAL )
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
    NCPkgTable * packageList = getPackageList();

    if ( diskspacePopup )
    {
	diskspacePopup->showInfoPopup();
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
bool PackageSelector::LinkHandler ( string link )
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
	    NCPopupPkgDescr popupDescr( wpos(1,1), this );
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
// HelpHandler
// 
// Show the help popup
//
bool PackageSelector::HelpHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
    string text = "";
    YCPString headline = YCPString(PkgNames::PackageHelp());
    
    if ( event.selection.isNull() )
    {
	return false;
    }
    
    if ( event.selection->compare( PkgNames::GeneralHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpPkgInst1();
	text += PkgNames::HelpPkgInst12();
	text += PkgNames::HelpPkgInst13();
	text += PkgNames::HelpPkgInst2();
	text += PkgNames::HelpPkgInst3();
	text += PkgNames::HelpPkgInst4();
	text += PkgNames::HelpPkgInst5();
	text += PkgNames::HelpPkgInst6();
    }
    else if ( event.selection->compare( PkgNames::StatusHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnStatus1();
	text += PkgNames::HelpOnStatus2();
	text += PkgNames::HelpOnStatus3();
	text += PkgNames::HelpOnStatus4();
	text += PkgNames::HelpOnStatus5();
	text += PkgNames::HelpOnStatus6();
	text += PkgNames::HelpOnStatus7();
    }
    else if ( event.selection->compare( PkgNames::UpdateHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnUpdate();
    }
    else if ( event.selection->compare( PkgNames::SearchHelp() ) == YO_EQUAL )
    {
	headline = YCPString(PkgNames::SearchHeadline());
	text += PkgNames::HelpOnSearch();
    }
    
    // open the popup with the help text
    NCPopupInfo pkgHelp( wpos( 1, 1 ), headline, YCPString( text ) );
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
bool PackageSelector::YouHelpHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
    string text  = "";

    text += PkgNames::YouHelp1();
    text += PkgNames::YouHelp2();
    text += PkgNames::YouHelp3();

    // open the popup with the help text
    NCPopupInfo youHelp( wpos( 1, 1 ), YCPString(PkgNames::YouHelp()), YCPString(text) );
    youHelp.showInfoPopup( );

    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }
       
    return true;
}

///////////////////////////////////////////////////////////////////
//
// SelectionHandler
// 
// Save/Load selections
//
bool PackageSelector::SelectionHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
    if ( event.selection.isNull()
	 || !filePopup )
    {
	return false;
    }
    
    if ( event.selection->compare( PkgNames::SaveSel() ) == YO_EQUAL )
    {
	filePopup->saveToFile();
    }
    else if ( event.selection->compare( PkgNames::LoadSel() ) == YO_EQUAL )
    {
	filePopup->loadFromFile();
	updatePackageList();
	showDiskSpace();
    }

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
bool PackageSelector::CancelHandler( const NCursesEvent&  event )
{
    bool changes = diffState ();

    if (changes) {
	// show a popup and ask the user
	NCPopupInfo cancelMsg( wpos( 2, 2 ),
			   YCPString( PkgNames::NotifyLabel() ),
			   YCPString( PkgNames::CancelText() ),
			   PkgNames::OKLabel(),
			   PkgNames::CancelLabel() );
	cancelMsg.setNiceSize( 35, 8 ); 
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
bool PackageSelector::OkButtonHandler( const NCursesEvent&  event )
{
    bool closeDialog = true;


    // check/show dependencies and automatic changes also if youMode == true

    // show the dependency popup
    if ( showPackageDependencies( true ) )
    {
	// don't leave the package installation if the user has clicked on Cancel
	// in dependency popup because maybe he wants to change his choices
	closeDialog = false;
    }
    // show the automatic changes list
    NCPopupPkgTable autoChangePopup( wpos( 1, 1), this );
    NCursesEvent input = autoChangePopup.showInfoPopup();

    if ( input == NCursesEvent::cancel )
    {
	// user clicked on Cancel
	closeDialog = false;
    }

    if ( diskspacePopup )
    {
	string message = "";
	message = diskspacePopup->checkDiskSpace();
	if ( message != "" )
	{
	    // open the popup e.g. with the text "/usr needs 50 MB more disk space"
	    NCPopupInfo spaceMsg( wpos( 2, 2 ),
				  YCPString( PkgNames::ErrorLabel() ),
				  YCPString( PkgNames::DiskSpaceError() + "<br>" + message ),
				  PkgNames::OKLabel(),
				  PkgNames::CancelLabel() );
	    
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



///////////////////////////////////////////////////////////////////
//
// showPatchInformation
//
// Shows the patch information
//
bool PackageSelector::showPatchInformation ( ZyppObj objPtr, ZyppSel selectable )
{
    ZyppPatch patchPtr = tryCastToZyppPatch( objPtr );

    if ( !patchPtr )
    {
	NCERR << "Patch not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }    

    if (  visibleInfo->compare( PkgNames::PatchDescr() ) == YO_EQUAL )
    {
	string descr;
	
	descr += PkgNames::Patch();
	descr += selectable->name();
	descr += "&nbsp;";
	descr += PkgNames::Size();
	descr += patchPtr->size().asString( 8 );
	descr += "<br>";
	// get and format the patch description
	zypp::Text value = patchPtr->description();
	descr += createDescrText( value );
	
	// show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString( descr ) );
	}	
    }
    else if (  visibleInfo->compare( PkgNames::PatchPackages() ) == YO_EQUAL )
    {
	NCPkgTable *patchPkgList  = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::PatchPkgs(), true));
	if ( patchPkgList )
	{
	    fillPatchPackages ( patchPkgList, objPtr);
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
bool PackageSelector::showPackageDependencies ( bool doit )
{
    bool cancel = false;
    
    if ( depsPopup
	 && (doit || autoCheck) )
    {
	cancel = depsPopup->showDependencies( );
    }

    return cancel;
}

///////////////////////////////////////////////////////////////////
//
// showDependencies
//
// Checks and shows the dependencies
//
void PackageSelector::showSelectionDependencies ( )
{
    showPackageDependencies (true);
}

///////////////////////////////////////////////////////////////////
//
// showPackageInformation
//
// Shows the package information
//
bool PackageSelector::showPackageInformation ( ZyppObj pkgPtr, ZyppSel slbPtr )
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

    if ( visibleInfo->compare( PkgNames::LongDescr() ) == YO_EQUAL )
    {
	// ask the package manager for the description of this package
	zypp::Text value = pkgPtr->description();
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
	string text = PkgNames::ListOfFiles();
	// the file list is available only for installed packages
	ZyppPkg package = tryCastToZyppPkg (slbPtr->installedObj());

	if ( package )
	{
	    // get the file list from the package manager/show the list
	    list<string> fileList = package->filenames();
	    text += createText( fileList, false ) ;
	}
	
	// get the widget id 
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );  
	if ( descrInfo  )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(text) );
	}	
    }
    else if ( visibleInfo->compare( PkgNames::PkgInfo() ) == YO_EQUAL )
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
	
	text += PkgNames::Version();
	text +=  version;
	if ( instVersion != "" )
	{
	    text += "  ";
	    text += PkgNames::InstVersion();
	    text += instVersion;
	}
	text +=  "  ";
	
	// show the size
	text += PkgNames::Size();
	text += pkgPtr->size().asString();
	text +=  "  ";
	
	ZyppPkg package = tryCastToZyppPkg (pkgPtr);
	if ( package )
	{
	    // add the media nr
	    text += PkgNames::MediaNo();
	    char num[5];
	    int medianr = package->mediaId ();
	    sprintf( num, "%d", medianr );
	    text += num;
	    text += "<br>";	    

	    // the license
	    text += PkgNames::License();
	    text += package->license();
	    text += "  ";
	    text += "<br>";

	    // the rpm group
	    text += PkgNames::RpmGroup();
	    text += package->group ();
	    text += "<br>";
	}
	
	// show Provides:
	text += PkgNames::Provides();
	zypp::CapSet provides = package->dep (zypp::Dep::PROVIDES);
	text += createRelLine(provides);
	text += "<br>";

	// show the authors
	if ( package )
	{
	    text += PkgNames::Authors();
	    list<string> authors = package->authors(); // zypp::Package	
	    text += createText( authors, true );
	}
	
        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );

	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(text) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));
	if ( pkgAvail )
	{
	    fillAvailableList( pkgAvail, slbPtr );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Relations() ) == YO_EQUAL )
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
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
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

string PackageSelector::createDescrText( zypp::Text value )
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
void PackageSelector::updatePackageList()
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
string PackageSelector::createRelLine( const zypp::CapSet & info )
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
string PackageSelector::createText( list<string> info, bool oneline )
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
void PackageSelector::showDiskSpace()
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
    YWidget * diskSpace = y2ui->widgetWithId( PkgNames::Diskspace(), true );
    if ( diskSpace )
    {
	static_cast<NCLabel *>(diskSpace)->setLabel( label );
    }
}


///////////////////////////////////////////////////////////////////
//
// showDownloadSize()
//
// total download size of YOU patches
//
void PackageSelector::showDownloadSize()
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
    YWidget * diskSpace = y2ui->widgetWithId( PkgNames::Diskspace(), true );
    if ( diskSpace )
    {
	static_cast<NCLabel *>(diskSpace)->setLabel( label );
    }
}


///////////////////////////////////////////////////////////////////
//
// getPackageList()
//
NCPkgTable * PackageSelector::getPackageList()
{
	YWidget * packages = y2ui->widgetWithId( PkgNames::Packages(), true );
	
	return ( dynamic_cast<NCPkgTable *>(packages) );
}
