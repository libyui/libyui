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
#include "NCSpacing.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPkgFilterRPMGroups.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCMenuButton.h"
#include "NCPkgFilterPattern.h"
#include "NCPkgFilterLocale.h"
#include "NCPkgFilterRepo.h"
#include "NCPkgPopupDeps.h"
#include "NCPkgPopupDiskspace.h"
#include "NCPkgMenuDeps.h"
#include "NCPkgMenuView.h"
#include "NCPkgMenuExtras.h"
#include "NCPkgMenuHelp.h"
#include "NCPkgMenuAction.h"
#include "NCPkgPopupDescr.h"
#include "NCPackageSelector.h"
#include "NCLayoutBox.h"
#include "YSelectionBox.h"
#include "YAlignment.h"
#include "YNCursesUI.h"
#include "YApplication.h"
#include "NCi18n.h"

//#include <fstream>
//#include <iomanip>
#include <list>
#include <string>
#include <set>

#include "NCZypp.h"		// tryCastToZyppPkg(), tryCastToZyppPatch()
#include <zypp/ui/Selectable.h>

#include "YWidgetID.h"
#include "YPackageSelector.h"

typedef zypp::Patch::Contents				ZyppPatchContents;
typedef zypp::Patch::Contents::Selectable_iterator	ZyppPatchContentsIterator;

#include "YEvent.h"

using namespace std;
using std::string;
using std::wstring;
using std::map;
using std::pair;

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
// Constructor
//
NCPackageSelector::NCPackageSelector( YNCursesUI * ui, YWidget * wRoot, long modeFlags )
    : y2ui( ui )
      , filterPopup( 0 )
      , depsPopup( 0 )
      , patternPopup( 0 )
      , languagePopup( 0 )
      , repoPopup( 0 )
      , diskspacePopup( 0 )
      , searchPopup( 0 )
      , youMode( false )
      , updateMode( false )
      , testMode ( false )
      , autoCheck( true )
      , pkgList ( 0 )
      , depsMenu( 0 )
      , viewMenu( 0 )
      , extrasMenu( 0 )
      , helpMenu( 0 )
      , youHelpButton( 0 )
      , filterMain( 0 )
      , actionMenu( 0 )
      , filterMenu( 0 )
      , updatelistItem( 0 )
      , packageLabel( 0 )
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
    if ( modeFlags & YPkg_OnlineUpdateMode )
	youMode = true;

    if ( modeFlags & YPkg_UpdateMode )
	updateMode = true;

    // read test source information
    if (modeFlags & YPkg_TestMode )
	testMode = true;

    saveState ();
    diskspacePopup = new NCPkgDiskspace( testMode );

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

bool NCPackageSelector::checkNow( bool *ok ) 
{
    bool ret = false;

    depsPopup = new NCPkgPopupDeps( wpos( 3, 8 ), this );
    ret = depsPopup->showDependencies( NCPkgPopupDeps::S_Solve, ok );
    YDialog::deleteTopmostDialog();
    return ret;
}

bool NCPackageSelector::verifySystem( bool *ok ) 
{
    bool ret = false;

    depsPopup = new NCPkgPopupDeps( wpos( 3, 8 ), this );
    ret = depsPopup->showDependencies( NCPkgPopupDeps::S_Verify, ok ); 
    YDialog::deleteTopmostDialog();
    return ret;
}

//////////////////////////////////////////////////////////////////
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

    p.saveState<zypp::Pattern> ();
    //p.saveState<zypp::Language> ();
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

    p.restoreState<zypp::Pattern> ();
    //p.restoreState<zypp::Language> ();
}

bool NCPackageSelector::diffState ()
{
    ZyppPool p = zyppPool ();

    bool diff = false;

    ostream & log = yuiMilestone();
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

    diff = diff || p.diffState<zypp::Pattern> ();
    log << diff << endl;
    //diff = diff || p.diffState<zypp::Language> ();
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

    if ( event == NCursesEvent::handled )
	return false;

    // Call the appropriate handler
    if ( event == NCursesEvent::button )
    {
	if ( event.widget == okButton )
	    retVal = OkButtonHandler( event );
	else if ( event.widget == cancelButton )
	    retVal = CancelHandler( event );
	else if ( event.widget == youHelpButton )
	    retVal = YouHelpHandler( event );
	else if ( event.widget == filterPopup )
	{   
	    retVal = filterPopup->handleEvent();
	}
	else if ( event.widget == filterMain )
	{   
	    retVal = filterMain->handleEvent();
	}
	else if ( event.widget == searchButton )
	{
	    retVal = searchPopup->showSearchResultPackages();
	}
    }
    else if ( event == NCursesEvent::menu )
    {
	if ( event.widget == actionMenu )
	    // change package/patch status
	    retVal = actionMenu->handleEvent( event );
	else if ( event.widget == viewMenu )
	    // show package/patch information
	    retVal = viewMenu->handleEvent( event );
	else if ( event.widget == depsMenu )
	    retVal = depsMenu->handleEvent( event );
	else if ( event.widget == extrasMenu )
	    retVal = extrasMenu->handleEvent( event );
	else if ( event.widget == helpMenu )
	    retVal = helpMenu->handleEvent( event );
	else if ( event.widget == filterMenu )
	    retVal = filterMenu->handleEvent( event );
	else if ( event.selection->label().substr(0,4) == "pkg:" )
	    // handle hyper links
	    retVal = LinkHandler( event.selection->label() );

    }

    return retVal;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchSearchList
//
// Fills the patch list with search results
//
bool NCPackageSelector::fillPatchSearchList( const string & expr )
{
   NCPkgTable * packageList = PackageList();

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
	  string name = patchPtr->name();
      
	  string::iterator pos = search( name.begin(), name.end(),
					 expr.begin(), expr.end(),
					 ic_compare );
	  
          if ( pos != name.end()  )
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
    if ( packageLabel )
    {
	packageLabel->setLabel( NCPkgStrings::SearchResults() );
    }

    return true;
}


///////////////////////////////////////////////////////////////////
//
// fillPatchList
//
// Fills the package table with the list of YOU patches
//
bool NCPackageSelector::fillPatchList( NCPkgMenuFilter::PatchFilter filter )
{
    NCPkgTable * packageList = PackageList();

    if ( !packageList )
    {
	yuiError() << "No valid NCPkgTable widget" << endl;
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

    if ( filter == NCPkgMenuFilter::F_All
	 && packageList->getNumLines() == 0 )
    {
	packageList->createInfoEntry( NCPkgStrings::NoPatches() );
    }

    // show the patches
    packageList->drawList();

    // show the selected filter label
    if ( packageLabel )
    {
	switch ( filter )
	{
	    case  NCPkgMenuFilter::F_Needed:
		{
		    // show common label "Needed Patches"
		    packageLabel->setLabel( NCPkgStrings::YOUPatches() );
		    break;
		}
	    case NCPkgMenuFilter::F_Unneeded:
		{
		    packageLabel->setLabel( NCPkgStrings::InstPatches() );
		    break;
		}
	    default:
		{
		    packageLabel->setLabel( NCPkgStrings::Patches() );
		}
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
    NCPkgTable * packageList = PackageList();

    if ( !packageList )
    {
	yuiError() << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    list<zypp::PoolItem> problemList = zypp::getZYpp()->resolver()->problematicUpdateItems();

    for ( list<zypp::PoolItem>::const_iterator it = problemList.begin();
	  it != problemList.end();
	  ++it )
    {
	ZyppPkg pkg = tryCastToZyppPkg( (*it).resolvable() );

	if ( pkg )
	{
	    ZyppSel slb = selMapper.findZyppSel( pkg );

	    if ( slb )
	    {
		yuiMilestone() << "Problematic package: " <<  pkg->name().c_str() << " " <<
		    pkg->edition().asString().c_str() << endl;
		packageList->createListEntry( pkg, slb );
	    }
	}

    }

    // show the list
    packageList->drawList();

    // show the selected filter label
    if ( packageLabel )
    {
	packageLabel->setLabel( NCPkgStrings::UpdateProblem() );
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

    ZyppPatchContents patchContents( patchPtr->contents() );
    
    yuiMilestone() <<  "Filtering for patch: " << patchPtr->name().c_str() << " number of atoms: "
		   << patchContents.size() << endl ;

    for ( ZyppPatchContentsIterator it = patchContents.selectableBegin();
	  it != patchContents.selectableEnd();
	  ++it )
    {
	ZyppPkg pkg = tryCastToZyppPkg( (*it)->theObj() );

	if ( pkg )
	{
	    yuiMilestone() << "Patch package found: " <<  (*it)->name().c_str() << endl;
	    ZyppSel sel = selMapper.findZyppSel( pkg );

	    if ( sel )
	    {
		if ( inContainer( patchSelectables, sel ) )
		{
		    yuiMilestone() << "Suppressing duplicate selectable: " << (*it)->name().c_str() << "-" <<
			pkg->edition().asString().c_str() << " " <<
			pkg->arch().asString().c_str() << endl;
		}
		else
		{
		    patchSelectables.insert( sel );
		    yuiDebug() << (*it)->name().c_str() << ": Version: " <<  pkg->edition().asString() << endl;

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
	else  // No ZyppPkg - some other kind of object
	{
	    yuiDebug() << "Found unknown atom of kind %s: %s" <<
		(*it)->kind().asString().c_str() <<
		(*it)->name().c_str() << endl;

	}
    }

    // show the list
    pkgTable->drawList();

    return true;
}

// patches

///////////////////////////////////////////////////////////////////
//
// checkPatch
//
//
bool NCPackageSelector::checkPatch( ZyppPatch 	patchPtr,
				    ZyppSel	selectable,
				    NCPkgMenuFilter::PatchFilter filter )

{
    NCPkgTable * packageList = PackageList();
    bool displayPatch = false;

    if ( !packageList || !patchPtr
	 || !selectable )
    {
	yuiError() << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }
    yuiMilestone() << "Filter: " << filter << endl;
    switch ( filter )
    {
	case  NCPkgMenuFilter::F_All:
	    {
		displayPatch = true;
		break;
	    }	
	case NCPkgMenuFilter::F_Unneeded:	// unneeded means not relevant or satisfied
	    {
		if ( selectable->hasCandidateObj() &&
		     ( !selectable->candidateObj().isRelevant() ||
		       ( selectable->candidateObj().isSatisfied()  &&
			 ! selectable->candidateObj().status().isToBeInstalled() ) ) )
		{
		    displayPatch = true;
		}
		break;
	    }
	case NCPkgMenuFilter::F_Needed:		// needed means relevant patches
	    {
		// only shows patches relevant to the system
		if ( selectable->hasCandidateObj() && 
		     selectable->candidateObj().isRelevant() )
		{
		    // and only those that are needed 
		    if ( ! selectable->candidateObj().isSatisfied() ||
			 // may be it is satisfied because is preselected
			 selectable->candidateObj().status().isToBeInstalled() )
			displayPatch = true;
		}
		break;
	    }
	case NCPkgMenuFilter::F_Security:
	    {
		if ( patchPtr->category() == "security" )
		    displayPatch = true;
		break;
	    }
	case NCPkgMenuFilter::F_Recommended:
	    {
		if ( patchPtr->category() == "recommended" )
		    displayPatch = true;
		break;
	    }
	case NCPkgMenuFilter::F_Optional:
	    {
		if (  patchPtr->category() == "optional" )
		    displayPatch = true;
		break;
	    }
	default:
	    yuiWarning() << "Unknown patch filter" << endl; 
    }

    if ( displayPatch )
	packageList->createPatchEntry( patchPtr, selectable );

    return displayPatch;
}

///////////////////////////////////////////////////////////////////
//
// deleteReplacePoint
//
// Gets ( and returns ) the current size of the widget at the replace
// point and deletes it.
// 
wrect NCPackageSelector::deleteReplacePoint()
{
    // delete current child of the ReplacePoint
    YWidget * replaceChild = replacePoint->firstChild();
    wrect oldSize;

    if ( replaceChild )
    {
	oldSize = dynamic_cast<NCWidget *>(replaceChild)->wGetSize();

	delete replaceChild;
	// reset all info widgets
	infoText = 0;		// NCPkgPackageDetails ( NCRichText )
	versionsList = 0;	// NCPkgTable - type: T_Availables
	patchPkgs = 0;		// NCPkgTable - type: T_PatchPkgs
	patchPkgsVersions = 0;	// NCPkgTable - type: T_Availables
    }
    
    return oldSize;
}

///////////////////////////////////////////////////////////////////
//
// showInformation
//
// Creates an NCPkgPackageDetails (a RichtText widget) which is
// used to show the required information (called from NCPkgMenuView)
// 
// 
void NCPackageSelector::showInformation()
{
    wrect oldSize = deleteReplacePoint();
     
    // show the rich text widget
    infoText = new NCPkgPackageDetails( replacePoint, " ", this);

    if ( infoText )
    {
	infoText->setSize( oldSize.Sze.W, oldSize.Sze.H );
	infoText->Redraw();
    }
}

///////////////////////////////////////////////////////////////////
//
// showVersionsList
//
// Creates an NCPkgTable (type T_Availables) which is used to show
// the list of package versions (called from NCPkgMenuView)
// 
// 
void NCPackageSelector::showVersionsList()
{
    wrect oldSize = deleteReplacePoint();
    NCPkgTable * packageList = PackageList();

    // show a package table with all available package versions
    YTableHeader * tableHeader = new YTableHeader();
    versionsList = new NCPkgTable( replacePoint, tableHeader );
    // YDialog::currentDialog()->setInitialSize(); -> doesn't work
    // call versionsList->setSize() and versionsList->Redraw() instead

    if ( versionsList && packageList )
    {

	// set the connection to the NCPackageSelector !!!!
	versionsList->setPackager( this );
	// set status strategy
	NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	versionsList->setTableType( NCPkgTable::T_Availables, strategy );
	versionsList->fillHeader( );
	versionsList->setSize( oldSize.Sze.W, oldSize.Sze.H );
	
	versionsList->fillAvailableList(  packageList->getSelPointer( packageList->getCurrentItem() ) );
	versionsList->Redraw();

	packageList->setKeyboardFocus();
    }  
}

///////////////////////////////////////////////////////////////////
//
// showPatchPackages
//
// Creates an NCPkgTable (type T_PatchPkgs) which is used to show
// the list of all packages belonging to a patch (called from NCPkgMenuView)
// 
// 
void NCPackageSelector::showPatchPackages()
{
    wrect oldSize = deleteReplacePoint();
    NCPkgTable * packageList = PackageList();
      
    // show a package table with packages belonging to a patch
    YTableHeader * tableHeader = new YTableHeader();
    patchPkgs =  new NCPkgTable( replacePoint, tableHeader );

    if ( patchPkgs && packageList )
    {
	// set the connection to the NCPackageSelector !!!!
	patchPkgs->setPackager( this );
	// set status strategy - don't set extra strategy, use 'normal' package strategy
	NCPkgStatusStrategy * strategy = new PackageStatStrategy();
	patchPkgs->setTableType( NCPkgTable::T_PatchPkgs, strategy );
	patchPkgs->fillHeader( );
	patchPkgs->setSize( oldSize.Sze.W, oldSize.Sze.H );

	fillPatchPackages( patchPkgs, packageList->getDataPointer( packageList->getCurrentItem() ) );
	patchPkgs->Redraw();

	packageList->setKeyboardFocus();
    }  
}

///////////////////////////////////////////////////////////////////
//
// showPatchPkgsVersions
//
// Creates an NCPkgTable (type T_Availables) which is used to show
// a list of all versions of all packages belonging to a patch
// (called from NCPkgMenuView)
// 
// 
void NCPackageSelector::showPatchPkgVersions()
{
    wrect oldSize = deleteReplacePoint();
    NCPkgTable * packageList = PackageList();
       
    // show a package table with versions of the packages beloning to a patch
    YTableHeader * tableHeader = new YTableHeader();
    patchPkgsVersions =  new NCPkgTable( replacePoint, tableHeader );

    if ( patchPkgsVersions && packageList )
    {
	// set the connection to the NCPackageSelector !!!!
	patchPkgsVersions->setPackager( this );
	// set status strategy and table type
	NCPkgStatusStrategy * strategy = new AvailableStatStrategy();
	patchPkgsVersions->setTableType( NCPkgTable::T_Availables, strategy );
	patchPkgsVersions->fillHeader( );
	patchPkgsVersions->setSize( oldSize.Sze.W, oldSize.Sze.H );

	fillPatchPackages( patchPkgsVersions, packageList->getDataPointer( packageList->getCurrentItem() ), true );
	patchPkgsVersions->Redraw();

	packageList->setKeyboardFocus();
    }
}


void NCPackageSelector::replaceFilter( FilterMode mode)
{
    patternLabel->setLabel( "                           " );
    YWidget * replaceChild = replPoint->firstChild();
    wrect oldSize;

    if ( replaceChild )
    {
	oldSize = dynamic_cast<NCWidget *>(replaceChild)->wGetSize();

	delete replaceChild;

	filterPopup = 0;
	patternPopup = 0;
	languagePopup = 0;
	repoPopup = 0;
	searchPopup = 0;
    }

    //replace the description area already here, so the next selected
    //filter can update it right away (#377857)
    replaceFilterDescr( mode == Search );

    switch (mode)
    {
	case Patterns:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   patternPopup = new NCPkgFilterPattern( replPoint, hhh, this );
	   patternPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   patternPopup->Redraw();
	   patternPopup->showPatternPackages();
	   break;
	}
	case Languages:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   languagePopup = new NCPkgLocaleTable( replPoint, hhh, this );
	   languagePopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   languagePopup->Redraw();
	   languagePopup->showLocalePackages();
	   break;
	}
	case Repositories:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   repoPopup = new NCPkgRepoTable( replPoint, hhh, this );
	   repoPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   repoPopup->Redraw();
	   repoPopup->showRepoPackages();
	   break;
        }
	case RPMGroups:
	{
	    filterPopup = new NCPkgFilterRPMGroups ( replPoint, " ", this);  
	    filterPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    filterPopup->Redraw();

            YStringTreeItem * defaultGroup = filterPopup->getDefaultRpmGroup();

	    if ( defaultGroup )
	    {
		yuiMilestone() << "default RPM group: " << defaultGroup->value().translation() << endl;
		filterPopup->showRPMGroupPackages ( defaultGroup->value().translation(), defaultGroup );
	    }
	    else
	    {
		yuiError() << "No default RPM group available" << endl;
	    }
	    break;
	}
        case Search:
	{
	    searchPopup = new NCPkgFilterSearch( replPoint, YD_VERT, this );
	    searchPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    searchPopup->Redraw();
	    break;
	}
        case Summary:
 	{
	    placeholder = new NCEmpty( replPoint );
	    placeholder->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    placeholder->Redraw();
	    pkgList->fillSummaryList(NCPkgTable::L_Changes);
	}
	default:
	    yuiError() << "zatim nic" << endl;
    }

   if (mode == Search)
   {
       pkgList->itemsCleared();
   }
   else
   {
       pkgList->setCurrentItem(0);
       pkgList->showInformation ();
   }

}

void NCPackageSelector::replaceFilterDescr( bool b )
{
    YWidget * replaceChild = replPoint2->firstChild();
    wrect oldSize;

    if ( replaceChild )
    {
	oldSize = dynamic_cast<NCWidget *>(replaceChild)->wGetSize();

	delete replaceChild;
        filter_desc = 0;
	searchSet = 0;
    }   

    if (b)
    {
	searchSet = new NCPkgSearchSettings( replPoint2, NCPkgStrings::SearchIn() );
	searchSet->setSize( oldSize.Sze.W, oldSize.Sze.H );
	searchSet->Redraw();
    }
    else 
    {
	filter_desc = new NCRichText( replPoint2, "");
	filter_desc->setSize( oldSize.Sze.W, oldSize.Sze.H );
	filter_desc->Redraw();
    }

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
	    yuiMilestone() << "Package " << pkgName << " found" << endl;
	    // open popup with package info
	    NCPkgPopupDescr * popupDescr = new NCPkgPopupDescr( wpos(1,1), this );
	    popupDescr->showInfoPopup( pkgPtr, *i );

	    YDialog::deleteTopmostDialog();

	    found = true;
	}
    }

    if ( !found )
    {
	yuiError() << "Package " << pkgName << " NOT found" << endl;
	// open error popup
    }

    return found;
}


///////////////////////////////////////////////////////////////////
//
// YOUHelpHandler
//
// Show the Online Update Help
//
bool NCPackageSelector::YouHelpHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = PackageList();
    string text  = "";

    text += NCPkgStrings::YouHelp1();
    text += NCPkgStrings::YouHelp2();
    text += NCPkgStrings::YouHelp3();

    // open the popup with the help text
    NCPopupInfo * youHelp = new NCPopupInfo( wpos( (NCurses::lines()*8)/100, (NCurses::cols())*18/100 ),
					     NCPkgStrings::YouHelp(),
					     text
					     );
    youHelp->setNiceSize( (NCurses::cols()*65)/100, (NCurses::lines()*85)/100 );
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
						   NCPkgStrings::NotifyLabel(),
						   NCPkgStrings::CancelText(),
						   NCPkgStrings::YesLabel(),
						   NCPkgStrings::NoLabel()
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

    yuiMilestone() <<  "Cancel button pressed - leaving package selection" << endl;
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
						      NCPkgStrings::ErrorLabel(),
						      NCPkgStrings::DiskSpaceError() + "<br>" + message,
						      NCPkgStrings::OKLabel(),
						      NCPkgStrings::CancelLabel() );

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
	yuiMilestone() <<  "OK button pressed - leaving package selection, starting installation" << endl;

        // return false, leave the package selection
	return false;
    }
    else
    {
	NCPkgTable * packageList = PackageList();
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
    yuiMilestone() << "Showing all pending license agreements" << endl;

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
			yuiMilestone() << "Package/Patch %s has a license agreement"
				       << sel->name().c_str() << endl;

			if( ! sel->hasLicenceConfirmed() )
			{
			    allConfirmed = showLicenseAgreement( sel, licenseText ) && allConfirmed;
			}
			else
			{
			    yuiMilestone() << "Package/Patch %s's  license is already confirmed"
					   << sel->name().c_str() << endl;
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
					   NCPkgStrings::NotifyLabel(),
					   "<i>" + pkgName + "</i><br><br>"
					   + createDescrText( licenseText ),
					   NCPkgStrings::AcceptLabel(),
					   NCPkgStrings::CancelLabel() );

    info->setNiceSize( (NCurses::cols() * 80)/100, (NCurses::lines()*80)/100);
    license_confirmed = info->showInfoPopup( ) != NCursesEvent::cancel;

    if ( !license_confirmed )
    {
	// make sure the package won't be installed
	switch ( slbPtr->status() )
	{
	    case S_Install:
	    case S_AutoInstall:
		slbPtr->setStatus( S_Taboo );
		break;

	    case S_Update:
	    case S_AutoUpdate:
		slbPtr->setStatus(  S_Protected );
		break;

	    default:
		break;
	}

	ok = false;
    } else {
	yuiMilestone() << "User confirmed license agreement for " << pkgName << endl;
	slbPtr->setLicenceConfirmed (true);
	ok = true;
    }

    YDialog::deleteTopmostDialog();

    return ok;
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

    if ( doit || autoCheck )
    {
	yuiMilestone() << "Checking dependencies" << endl;
	cancel = checkNow( & ok );
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
// createDescrText
//
#define DOCTYPETAG "<!-- DT:Rich -->"

string NCPackageSelector::createDescrText( string value )
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
    NCPkgTable * packageList = PackageList();

    if ( packageList )
    {
	packageList->updateTable();
    }
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
	    ZyppPatchContents patchContents( patch->contents() );

	    for ( ZyppPatchContentsIterator contents_it = patchContents.selectableBegin();
		  contents_it != patchContents.selectableEnd();
		  ++contents_it )
	    {
		ZyppPkg pkg = tryCastToZyppPkg( (*contents_it)->theObj() );
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
	    totalSize += (*it)->candidateObj()->installsize();
    }

    // show the download size
    if ( diskspaceLabel )
    {
	diskspaceLabel->setText( totalSize.asString() );
    }
}


///////////////////////////////////////////////////////////////////
//
// PackageList()
//
NCPkgTable * NCPackageSelector::PackageList()
{
    return pkgList;
}

//
// Create layout for Online Update
//
void NCPackageSelector::createYouLayout( YWidget * selector )
{
    // the vertical split is the (only) child of the dialog
    YLayoutBox * split = YUI::widgetFactory()->createVBox( selector );

    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( split );

    YAlignment * left1 = YUI::widgetFactory()->createLeft( hSplit );
    filterMenu = new NCPkgMenuFilter( left1, NCPkgStrings::Filter(), this );
    
    YAlignment * left2 = YUI::widgetFactory()->createLeft( hSplit );
    actionMenu = new NCPkgMenuAction( left2, NCPkgStrings::Actions(), this );

    YAlignment * left3 = YUI::widgetFactory()->createLeft( hSplit );
    viewMenu = new NCPkgMenuView( left3, NCPkgStrings::View(), this);

    YAlignment * left4 = YUI::widgetFactory()->createLeft( hSplit );
    depsMenu = new NCPkgMenuDeps( left4, NCPkgStrings::Deps(), this);

       // add the package table
    YTableHeader * tableHeader = new YTableHeader();

    pkgList = new NCPkgTable( split, tableHeader );
    YUI_CHECK_NEW( pkgList );

    // set table type 'T_Patches'
    NCPkgStatusStrategy * strategy;
    strategy = new PatchStatStrategy();
    pkgList->setTableType( NCPkgTable::T_Patches, strategy );
    pkgList->fillHeader();

    // set the pointer to the packager object
    pkgList->setPackager( this );

       // HBox for Filter and Disk Space (both in additional HBoxes )
    YLayoutBox * hSplit2 = YUI::widgetFactory()->createHBox( split );

    YLayoutBox * hSplit3 = YUI::widgetFactory()->createHBox( hSplit2 );
    // label text - keep it short
    new NCLabel( hSplit3,  _( "Filter: " ) );
    packageLabel = YUI::widgetFactory()->createLabel ( hSplit3, "....................................." );

    new NCSpacing( hSplit2, YD_HORIZ, true, 0.5 );

    YLayoutBox * hSplit4 = YUI::widgetFactory()->createHBox( hSplit2 );
    // label text - keep it short (use abbreviation if necessary)
    new NCLabel( hSplit4,   _( "Total Download Size: " ) );
    diskspaceLabel = YUI::widgetFactory()->createLabel ( hSplit4, "   " );

    YLayoutBox * vSplit = YUI::widgetFactory()->createVBox( split );
    replacePoint = YUI::widgetFactory()->createReplacePoint( vSplit );

    infoText = new NCPkgPackageDetails( replacePoint, " ", this );
    YUI_CHECK_NEW( infoText );

    YLayoutBox * bottom_bar = YUI::widgetFactory()->createHBox( vSplit );
    YAlignment *ll = YUI::widgetFactory()->createLeft( bottom_bar );

    youHelpButton = new NCPushButton ( ll, _("&Help"));
    YUI_CHECK_NEW( youHelpButton );
    youHelpButton->setFunctionKey( 1 );
    
    YAlignment *r = YUI::widgetFactory()->createRight( bottom_bar );
    YLayoutBox * hSplit5 = YUI::widgetFactory()->createHBox( r );
    
    // add the Cancel button
    cancelButton = new NCPushButton( hSplit5, _( "&Cancel" ) );
    YUI_CHECK_NEW( cancelButton );
    cancelButton->setFunctionKey( 9 );

    // add the OK button
    okButton = new NCPushButton( hSplit5, _( "&Accept" ) );
    YUI_CHECK_NEW( okButton );
    okButton->setFunctionKey( 10 );
    
}

//
// Create layout for Package Selector
//
void NCPackageSelector::createPkgLayout( YWidget * selector, NCPkgTable::NCPkgTableType type )
{
     // the vertical split is the (only) child of the dialog
    YLayoutBox * vsplit = YUI::widgetFactory()->createVBox( selector );
    YLayoutBox * menu_bar = YUI::widgetFactory()->createHBox( vsplit );
    YLayoutBox * panels = YUI::widgetFactory()->createVBox( vsplit );
    YLayoutBox * bottom_bar = YUI::widgetFactory()->createHBox( vsplit );

    YAlignment * left1 = YUI::widgetFactory()->createLeft( menu_bar );

    YLayoutBox * menu_buttons = YUI::widgetFactory()->createHBox( left1);
    depsMenu = new NCPkgMenuDeps( menu_buttons, NCPkgStrings::Deps(), this);
    viewMenu = new NCPkgMenuView( menu_buttons, NCPkgStrings::View(), this);
    extrasMenu = new NCPkgMenuExtras( menu_buttons, NCPkgStrings::Extras(), this);

    YLayoutBox * hbox_top = YUI::widgetFactory()->createHBox( panels );
    YLayoutBox * hbox_bottom = YUI::widgetFactory()->createHBox( panels );

    YLayoutBox * vbox_left = YUI::widgetFactory()->createVBox( hbox_top );
    vbox_left->setWeight(YD_HORIZ,1);
    YFrame * fr = YUI::widgetFactory()->createFrame (vbox_left, "");
    YLayoutBox * vv = YUI::widgetFactory()->createVBox( fr );
    YAlignment *l = YUI::widgetFactory()->createLeft( vv );
    filterMain = new NCPkgFilterMain (l, NCPkgStrings::Filter(), this );

    replPoint = YUI::widgetFactory()->createReplacePoint( vv );
    YTableHeader *hhh = new YTableHeader();
    patternPopup = new NCPkgFilterPattern( replPoint, hhh, this );

    YAlignment *l1 = YUI::widgetFactory()->createLeft( vbox_left );
    patternLabel = new NCLabel( l1, "                           " );
    

    // add the package table
    YTableHeader * tableHeader = new YTableHeader();

    YLayoutBox * v = YUI::widgetFactory()->createVBox( hbox_top );
    v->setWeight(YD_HORIZ,2);
    pkgList = new NCPkgTable( v, tableHeader );
    YUI_CHECK_NEW( pkgList );

    NCPkgStatusStrategy * strategy;
    // set table type and status strategy (either 'normal' package list or update list) 
    switch ( type )
    {
	case NCPkgTable::T_Packages:
	    strategy = new PackageStatStrategy();
	    pkgList->setTableType( NCPkgTable::T_Packages, strategy );
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
    YLayoutBox * hSplit2 = YUI::widgetFactory()->createHBox( v );

    // label text - keep it short
    new NCLabel( hSplit2,  NCPkgStrings::PackageName() );
    packageLabel = YUI::widgetFactory()->createLabel ( hSplit2, "......................" );

    new NCSpacing( hSplit2, YD_HORIZ, true, 0.5 );
    
    actionMenu = new NCPkgMenuAction ( hSplit2, NCPkgStrings::Actions(), this );
   
    replPoint2 = YUI::widgetFactory()->createReplacePoint( hbox_bottom );
    replPoint2->setWeight(YD_HORIZ, 1);
    filter_desc = new NCRichText( replPoint2, " " );
    YLayoutBox * vSplit = YUI::widgetFactory()->createVBox( hbox_bottom );
    vSplit->setWeight(YD_HORIZ, 2);
    replacePoint = YUI::widgetFactory()->createReplacePoint( vSplit );

    infoText = new NCPkgPackageDetails( replacePoint, " ", this );
    YUI_CHECK_NEW( infoText );

    YAlignment *ll = YUI::widgetFactory()->createLeft( bottom_bar );
    helpMenu = new NCPkgMenuHelp (ll, _("&Help"));
    YUI_CHECK_NEW( helpMenu );
    
    YAlignment *r = YUI::widgetFactory()->createRight( bottom_bar );
    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( r );
   
    // add the Cancel button
    cancelButton = new NCPushButton( hSplit, _( "&Cancel" ) );
    YUI_CHECK_NEW( cancelButton );
    cancelButton->setFunctionKey( 9 );

    // add the OK button
    okButton = new NCPushButton( hSplit, _( "&Accept" ) );
    YUI_CHECK_NEW( okButton );
    okButton->setFunctionKey( 10 );

}

//
// Fill package list with packages of default RPM group/update list or installable patches
//
bool NCPackageSelector::fillDefaultList( )
{
    if ( !pkgList )
	return false;

    yuiMilestone() << "Fill list: " << (NCWidget *) pkgList << endl;

    switch ( pkgList->getTableType() )
    {
	case NCPkgTable::T_Patches: {
	    fillPatchList( NCPkgMenuFilter::F_Needed );	// default: needed patches

	    // set the visible info to long description
	    pkgList->setVisibleInfo(NCPkgTable::I_PatchDescr);
	    // show the patch description of the current item
	    pkgList->showInformation ();
	    break;
	}
	case NCPkgTable::T_Update: {
	    if ( ! zypp::getZYpp()->resolver()->problematicUpdateItems().empty() )
	    {
		fillUpdateList();
		// set the visible info to technical information
		pkgList->setVisibleInfo(NCPkgTable::I_Technical);
                // show the package information of the current item
		pkgList->showInformation ();
		break;
	    }
	}
	case NCPkgTable::T_Packages: {

		pkgList->setVisibleInfo(NCPkgTable::I_Technical);
		// update the pattern status which is available only after the initial
		// solver run
		patternPopup->updateTable();
	        patternPopup->showPatternPackages();
		// show the package inforamtion of the current item
		pkgList->showInformation ();
	        break;
	}
	default:
	    break;
    }

    pkgList->setKeyboardFocus();

    return true;

}
