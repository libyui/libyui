/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

/-*/
#include "NCurses.h"
#include "NCWidgetFactory.h"
#include "NCPushButton.h"
#include "NCMenuButton.h"
#include "NCTable.h"
#include "NCSpacing.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCMenuButton.h"
#include "NCPkgFilterPattern.h"
#include "NCPkgFilterLocale.h"
#include "NCPkgFilterRepo.h"
#include "NCPkgFilterService.h"
#include "NCPkgFilterClassification.h"
#include "NCPkgPopupDeps.h"
#include "NCPkgPopupDiskspace.h"
#include "NCPkgMenuDeps.h"
#include "NCPkgMenuView.h"
#include "NCPkgMenuExtras.h"
#include "NCPkgMenuConfig.h"
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
#include <zypp/base/Sysconfig.h>

#include "YWidgetID.h"
#include "YPackageSelector.h"

#define PATH_TO_YAST_SYSCONFIG  "/etc/sysconfig/yast2"

#define OPTION_RECOMMENDED      "PKGMGR_RECOMMENDED"
#define OPTION_REEVALUATE       "PKGMGR_REEVALUATE_RECOMMENDED"
#define OPTION_VERIFY           "PKGMGR_VERIFY_SYSTEM"
#define OPTION_AUTO_CHECK       "PKGMGR_AUTO_CHECK"
#define OPTION_EXIT             "PKGMGR_ACTION_AT_EXIT"

typedef zypp::Patch::Contents				ZyppPatchContents;
typedef zypp::Patch::Contents::Selectable_iterator	ZyppPatchContentsIterator;

#include "YEvent.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
// Constructor
//
NCPackageSelector::NCPackageSelector( long modeFlags )
      : depsPopup( 0 )
      , patternPopup( 0 )
      , languagePopup( 0 )
      , repoPopup( 0 )
      , diskspacePopup( 0 )
      , searchPopup( 0 )
      , autoCheck( true )
      , verifySystem( false )
      , installRecommended( false )
      , pkgList ( 0 )
      , depsMenu( 0 )
      , viewMenu( 0 )
      , configMenu( 0 )
      , extrasMenu( 0 )
      , helpMenu( 0 )
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
    setFlags( modeFlags );
    readSysconfig();
    saveState ();
    diskspacePopup = new NCPkgDiskspace( testMode );

    setInstallRecommended( isInstallRecommended() );
    setAutoCheck( isAutoCheck() );
    setVerifySystem( isVerifySystem() );
}


///////////////////////////////////////////////////////////////////
//
// Destructor
//
NCPackageSelector::~NCPackageSelector()
{
    // Don't call delete for the popups in destructor but call
    // YDialog::deleteTopmostDialog() instead at the end of
    // NCPackageSelectorPlugin::runPkgSelection
}

void NCPackageSelector::setFlags( long modeFlags )
{
    youMode = ( modeFlags & YPkg_OnlineUpdateMode ) ? true : false ;

    updateMode = ( modeFlags & YPkg_UpdateMode ) ? true : false ;

    repoMgrEnabled = (modeFlags & YPkg_RepoMgr) ? true : false;

    testMode = (modeFlags & YPkg_TestMode ) ? true : false ;

    repoMode = ( modeFlags & YPkg_RepoMode ) ? true : false;

    summaryMode = ( modeFlags & YPkg_SummaryMode ) ? true : false;
}

void NCPackageSelector::readSysconfig()
{
    sysconfig = zypp::base::sysconfig::read( PATH_TO_YAST_SYSCONFIG );
    std::map <std::string,std::string>::const_iterator it = sysconfig.find( OPTION_EXIT );

    if (it != sysconfig.end())
    {
	actionAtExit = it->second;
	yuiMilestone() << "Read sysconfig's action at pkg mgr exit value: " << actionAtExit << endl;
    }
    else
    {
	actionAtExit = "";
	yuiMilestone() << "Could not read " <<  OPTION_EXIT << "variable from sysconfig, disabling the menu" << endl;
    }
}

void NCPackageSelector::writeSysconfig( )
{

    if( !actionAtExit.empty() )
    {
        try
        {
            zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
                                                   OPTION_EXIT,
                                                   actionAtExit,
                                                   "Set behaviour when package installation has finished.");
        }
        catch( const std::exception &e )
        {
            yuiError() << "Writing " << OPTION_EXIT << " failed" << endl;
        }
    }

    try
    {
        zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
                                               OPTION_AUTO_CHECK,
                                               (autoCheck?"yes":"no"),
                                               "Automatic dependency checking" );
    }
    catch( const std::exception &e )
    {
        yuiError() << "Writing " << OPTION_AUTO_CHECK << " failed" << endl;
    }

    try
    {
        zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
                                               OPTION_VERIFY,
                                               (verifySystem?"yes":"no"),
                                               "System verification mode" );
    }
    catch( const std::exception &e )
    {
        yuiError() << "Writing " << OPTION_VERIFY << " failed" << endl;
    }

    try
    {
        zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
                                               OPTION_RECOMMENDED,
                                               (installRecommended?"yes":"no"),
                                               "Install recommended packages" );
    }
    catch( const std::exception &e )
    {
        yuiError() << "Writing " << OPTION_RECOMMENDED << " failed" << endl;
    }
}

bool NCPackageSelector::checkNow( bool *ok )
{
    bool ret = false;

    depsPopup = new NCPkgPopupDeps( wpos( 3, 4 ), this );
    ret = depsPopup->showDependencies( NCPkgPopupDeps::S_Solve, ok );
    YDialog::deleteTopmostDialog();
    return ret;
}

bool NCPackageSelector::systemVerification( bool *ok )
{
    bool ret = false;

    depsPopup = new NCPkgPopupDeps( wpos( 3, 4 ), this );
    ret = depsPopup->showDependencies( NCPkgPopupDeps::S_Verify, ok );
    YDialog::deleteTopmostDialog();
    return ret;
}

bool NCPackageSelector::doInstallRecommended( bool *ok )
{
    zypp::getZYpp()->resolver()->setIgnoreAlreadyRecommended( false );
    zypp::getZYpp()->resolver()->resolvePool();
    *ok = true;
    bool ret = true;
    return ret;
}

//
// 'Clean dependencies on remove' option' is NOT saved and cannot be set in /etc/sysconfig/yast2.
// The package selector starts with setting from /etc/zypp/zypp.conf (default is false).
//
bool NCPackageSelector::isCleanDepsOnRemove()
{
    return zypp::getZYpp()->resolver()->cleandepsOnRemove();
}

void NCPackageSelector::setCleanDepsOnRemove( bool on )
{
    zypp::getZYpp()->resolver()->setCleandepsOnRemove( on );
    zypp::getZYpp()->resolver()->resolvePool();
    updatePackageList();
}

//
// 'Install recommended packages' option can be set and is saved
// in /etc/sysconfig/yast2
//
bool NCPackageSelector::isInstallRecommended()
{
    std::map <std::string,std::string>::const_iterator it = sysconfig.find( OPTION_RECOMMENDED );

    if ( it != sysconfig.end() )
    {
        yuiMilestone() << OPTION_RECOMMENDED << ": " << it->second << endl;
        if ( it->second == "yes" )
            installRecommended = true;
        else if ( it->second == "no")
            installRecommended = false;
        else
            installRecommended = !(zypp::getZYpp()->resolver()->onlyRequires());    // reverse value
    }
    else
    {
        installRecommended = !(zypp::getZYpp()->resolver()->onlyRequires());        // reverse value
    }
    yuiMilestone() << "installRecommended: " << (installRecommended?"yes":"no") << endl;

    return installRecommended;
}

void NCPackageSelector::setInstallRecommended( bool on )
{
    installRecommended = on;
    zypp::getZYpp()->resolver()->setOnlyRequires( !on );    // reverse value here !
    // solve after changing the solver settings
    zypp::getZYpp()->resolver()->resolvePool();
    updatePackageList();
}


bool NCPackageSelector::isAutoCheck()
{
    // automatic dependency check is on by default (check on every click)

    std::map <std::string,std::string>::const_iterator it = sysconfig.find( OPTION_AUTO_CHECK);

    if ( it != sysconfig.end() )
    {
        yuiMilestone() << OPTION_AUTO_CHECK << ": " << it->second << endl;
        if ( it->second == "no" )
            autoCheck = false;
    }
    yuiMilestone() << "autoCheck " << (autoCheck?"yes":"no") << endl;

    return autoCheck;
}

bool NCPackageSelector::isVerifySystem( )
{
    std::map <std::string,std::string>::const_iterator it = sysconfig.find( OPTION_VERIFY );

    if ( it != sysconfig.end() )
    {
        yuiMilestone() << OPTION_VERIFY << ": " << it->second << endl;
        if ( it->second == "yes" )
            verifySystem = true;
        else if ( it->second == "no")
            verifySystem = false;
        else
           verifySystem = zypp::getZYpp()->resolver()->systemVerification();
    }
    else
    {
        verifySystem = zypp::getZYpp()->resolver()->systemVerification();
    }
    yuiMilestone() << "verifySystem: " << (verifySystem?"yes":"no") << endl;

    return verifySystem;
}

void NCPackageSelector::setVerifySystem( bool on )
{
    verifySystem = on;
    zypp::getZYpp()->resolver()->setSystemVerification( on );
    // solve after changing the solver settings
    zypp::getZYpp()->resolver()->resolvePool();
    updatePackageList();
}

//
// 'Allow vendor change' option is NOT saved and cannot be set in /etc/sysconfig/yast2.
// The package selector starts with setting from /etc/zypp/zypp.conf (default is false).
//
bool NCPackageSelector::isAllowVendorChange()
{
    zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();
    bool change = resolver->allowVendorChange();
    yuiMilestone() << "Vendor change allowed: " << (change?"true":"false") << endl;
    return change;
}

void NCPackageSelector::setAllowVendorChange( bool on )
{
    zypp::getZYpp()->resolver()->setAllowVendorChange( on );
    zypp::getZYpp()->resolver()->resolvePool();
    updatePackageList();
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

    p.saveState<zypp::Pattern> ();
    //p.saveState<zypp::Language> ();
}

void NCPackageSelector::restoreState ()
{
    ZyppPool p = zyppPool ();

    p.restoreState<zypp::Package> ();
    p.restoreState<zypp::SrcPackage> ();

    p.restoreState<zypp::Patch> ();

    p.restoreState<zypp::Pattern> ();
    //p.restoreState<zypp::Language> ();
}

bool NCPackageSelector::diffState ()
{
    ZyppPool p = zyppPool ();

    bool diff = false;

    std::ostream & log = yuiMilestone();
    log << "diffState" << endl;
    diff = diff || p.diffState<zypp::Package> ();
    log << diff << endl;
    diff = diff || p.diffState<zypp::SrcPackage> ();
    log << diff << endl;

    diff = diff || p.diffState<zypp::Patch> ();
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

    yuiMilestone() << "widget event: " << event << endl;
    // Call the appropriate handler
    if ( event == NCursesEvent::button )
    {
	if ( event.widget == okButton )
        {
	    retVal = OkButtonHandler( event );
        }
	else if ( event.widget == cancelButton )
        {
	    retVal = CancelHandler( event );
        }
	else if ( event.widget == filterMain )
	{
	    retVal = filterMain->handleEvent();
	}
	else if ( event.widget == searchField )
	{
            if ( event.reason ==  YEvent::Activated )
            {
                retVal = searchPopup->showSearchResultPackages();
            }
            else // no action, reason was YEvent::SelectionChanged
            {
                retVal = true;
            }
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
	else if ( event.widget == configMenu )
	    retVal = configMenu->handleEvent( event );
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
bool NCPackageSelector::fillPatchSearchList( const std::string & expr, bool checkName, bool checkSum )
{
   NCPkgTable * packageList = PackageList();

    if ( !packageList )
    {
      return false;
    }

    // clear the patch list
    packageList->itemsCleared ();

    zypp::PoolQuery q;
    q.addString( expr );
    q.addKind( zypp::ResKind::patch );
    q.addAttribute( zypp::sat::SolvAttr::keywords );
    if ( checkName )
    {
        q.addAttribute( zypp::sat::SolvAttr::name );
    }
    if ( checkSum )
    {
        q.addAttribute( zypp::sat::SolvAttr::summary );
    }

    for( zypp::PoolQuery::Selectable_iterator it = q.selectableBegin();
	it != q.selectableEnd(); it++)
    {
	yuiMilestone() << (*it)->name() << endl;
        ZyppPatch patchPtr = tryCastToZyppPatch( (*it)->theObj() );
        packageList->createPatchEntry ( patchPtr, *it);
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
    std::list<ZyppSel> patchList( zyppPatchesBegin (), zyppPatchesEnd () );
    patchList.sort( sortByName );
    std::list<ZyppSel>::iterator listIt = patchList.begin();

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

    std::list<zypp::PoolItem> problemList = zypp::getZYpp()->resolver()->problematicUpdateItems();

    for ( std::list<zypp::PoolItem>::const_iterator it = problemList.begin();
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
bool NCPackageSelector::fillPatchPackages ( NCPkgTable * pkgTable, ZyppObj objPtr )
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

    if ( !packageList )
    {
	yuiError() << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }
    if ( !patchPtr || !selectable || !selectable->hasCandidateObj() )
    {
	yuiError() << "Patch data not valid" << endl;
        return false;
    }
    yuiDebug() << "Filter: " << filter << endl;

    if ( filter == NCPkgMenuFilter::F_All )
    {
        displayPatch = true;
    }
    // only show patches relevant for the system, means any of the patch packages is installed
    else if ( selectable->candidateObj().isRelevant() )
    {
        switch ( filter )
        {
            case NCPkgMenuFilter::F_Unneeded:	// unneeded means satisfied (installed)
                {
                    if ( selectable->candidateObj().isSatisfied() &&
                         !selectable->candidateObj().status().isToBeInstalled() )
                    {
                        displayPatch = true;
                    }
                    break;
                }
            case NCPkgMenuFilter::F_Needed:	// needed means not satisfied...
                {
                    if ( !selectable->candidateObj().isSatisfied() ||
                         // or already satified because the patch is preselected
                         selectable->candidateObj().status().isToBeInstalled() )
                        displayPatch = true;
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
                    if ( patchPtr->category() == "optional" )
                        displayPatch = true;
                    break;
                }
            default:
                yuiWarning() << "Unknown patch filter" << endl;
        }
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
        // fill default header, set correct header in NCPkgTable::fillAvailableList()
        versionsList->fillHeader();
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
    // only available if patch packages are currently shown
    if ( patchPkgs )
    {
        // get selected line and show availables for this package
        ZyppSel sel = patchPkgs->getSelPointer( patchPkgs->getCurrentItem() );

        // show the availables
	NCPkgPopupTable * availablePopup =
            new NCPkgPopupTable( wpos( 3, 8), this,
                                 // headline of package versions popup
                                 _("Package Versions"),
                                 // text above of list of all package versions
                                 _("List of all available package versions:"),
                                 "",            // no additional text line
                                 false );       // no 'Cancel' button
	NCursesEvent input = availablePopup->showAvailablesPopup( sel );

	YDialog::deleteTopmostDialog();

	patchPkgs->setKeyboardFocus();
    }
}

void NCPackageSelector::clearInfoArea()
{
    if ( infoText )
       infoText->setText("");
    if ( versionsList )
	 versionsList->itemsCleared();

    packageLabel->setText(".....................................");
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

	patternPopup = 0;
	languagePopup = 0;
	repoPopup = 0;
        servicePopup = 0;
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
	   patternPopup->setKeyboardFocus();
	   break;
	}
	case Languages:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   languagePopup = new NCPkgLocaleTable( replPoint, hhh, this );
	   languagePopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   languagePopup->Redraw();
	   languagePopup->showLocalePackages();
	   languagePopup->setKeyboardFocus();
	   break;
	}
	case Repositories:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   repoPopup = new NCPkgRepoTable( replPoint, hhh, this );
	   repoPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   repoPopup->Redraw();
	   repoPopup->showRepoPackages();
	   repoPopup->setKeyboardFocus();
	   break;
        }
	case Services:
	{
	   YTableHeader *hhh = new YTableHeader ();
	   servicePopup = new NCPkgServiceTable( replPoint, hhh, this );
	   servicePopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	   servicePopup->Redraw();
	   servicePopup->showServicePackages();
	   servicePopup->setKeyboardFocus();
	   break;
        }
        case Search:
	{
	    searchPopup = new NCPkgFilterSearch( replPoint, YD_VERT, this );
            searchPopup->createLayout( replPoint );
	    searchPopup->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    searchPopup->Redraw();

            searchField = searchPopup->getSearchField();
            if ( searchField )
            {
                searchField->setKeyboardFocus();
                searchField->setNotify(true);
            }
	    break;
	}
        case Summary:
 	{
	    inst_summary = new NCPkgFilterInstSummary( replPoint, _( "&Packages with Status" ), this );
	    inst_summary->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    inst_summary->Redraw();
	    inst_summary->setKeyboardFocus();
	    pkgList->fillSummaryList(NCPkgTable::L_Changes);
	    break;
	}
        case PkgClassification:
 	{
	    pkgClass = new NCPkgFilterClassification( replPoint, this );
	    pkgClass->setSize( oldSize.Sze.W, oldSize.Sze.H );
	    pkgClass->Redraw();
	    pkgClass->setKeyboardFocus();
	    break;
	}

	default:
	    yuiError() << "zatim nic" << endl;
    }

   if (mode == Search)
   {
       pkgList->itemsCleared();
       clearInfoArea();
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
bool NCPackageSelector::LinkHandler ( std::string link )
{
    bool found = false;
    // e.g. link is pkg://hp-officeJet
    std::string pkgName = link.substr(6);

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
	cancelMsg->setPreferredSize( 45, 8 );
	cancelMsg->focusCancelButton();
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
	NCPkgPopupTable * autoChangePopup =
            new NCPkgPopupTable( wpos( 3, 8), this,
                                 // headline - packages with automatic status change
                                 _("Automatic Changes"),
                                 // text part1 of popup with automatic changes (it's a label; text continous)
                                 _( "In addition to your manual selections, the following" ),
                                 // text part2 of popup with automatic changes
                                 _( "packages have been changed to resolve dependencies:" )
                                 );
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
	std::string message = "";
	message = diskspacePopup->checkDiskSpace();
	if ( message != "" )
	{
	    // open the popup e.g. with the text "/usr needs 50 MB more disk space"
	    NCPopupInfo * spaceMsg = new NCPopupInfo( wpos( (NCurses::lines()-10)/2, (NCurses::cols()-50)/2 ),
						      NCPkgStrings::ErrorLabel(),
						      NCPkgStrings::DiskSpaceError() + "<br>" + message +
						      _( "You can choose to install anyway, but you risk getting a corrupted system." ),
						      _( "&Continue anyway" ),
						      NCPkgStrings::CancelLabel() );

	    spaceMsg->setPreferredSize( 50, 10 );
	    spaceMsg->focusOkButton();
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

	writeSysconfig();
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
	    packageList->updateTable();
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
		    std::string licenseText = sel->candidateObj()->licenseToConfirm();

		    if ( ! licenseText.empty() )
		    {
			yuiMilestone() << "Package/Patch " << sel->name().c_str() <<
			    "has a license" << endl;

			if( ! sel->hasLicenceConfirmed() )
			{
			    allConfirmed = showLicenseAgreement( sel, licenseText ) && allConfirmed;
			}
			else
			{
			    yuiMilestone() << "License for " << sel->name().c_str() <<
				" is already confirmed" << endl;
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

bool NCPackageSelector::showLicenseAgreement( ZyppSel & slbPtr , std::string licenseText )
{
    if ( !slbPtr )
	return false;

    bool license_confirmed = true;
    bool ok = true;
    std::string pkgName = slbPtr->name();

    license_confirmed = showLicensePopup( pkgName, licenseText );

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
// createLicenseText
//
bool NCPackageSelector::showLicensePopup( std::string pkgName, std::string license )
{
    std::string html_text = "";
    const std::string htmlIdent(DOCTYPETAG);
    bool confirmed = false;

    if ( license.find( htmlIdent ) != std::string::npos )
    {
	html_text = license;	// HTML text
    }
    else
    {
	html_text = "<pre>" + license + "</pre>";	// add <pre> to preserve newlines and spaces
    }

    NCPopupInfo * info = new NCPopupInfo ( wpos( NCurses::lines()/10, NCurses::cols()/10),
					   // headline of a popup showing the package license
					   _( "End User License Agreement" ),
					   "<i>" + pkgName + "</i><br><br>"
					   + html_text,
					   NCPkgStrings::AcceptLabel(),
					   NCPkgStrings::CancelLabel() );

    info->setPreferredSize( (NCurses::cols() * 80)/100, (NCurses::lines()*80)/100);
    info->focusOkButton();
    confirmed = info->showInfoPopup( ) != NCursesEvent::cancel;

    YDialog::deleteTopmostDialog();

    return confirmed;
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


    // check whether required diskspace enters the warning range
    if ( diskspacePopup )
    {
	diskspacePopup->checkDiskSpaceRange( );
	// show pkg_diff, i.e. total difference of disk space (can be negative in installed system
        // if packages are deleted)
        if ( diskspaceLabel )
        {
	    diskspaceLabel->setText( diskspacePopup->calculateDiff().asString() );
        }
    }
}


///////////////////////////////////////////////////////////////////
//
// showDownloadSize()
//
// total download size of YOU patches
//
void NCPackageSelector::showDownloadSize()
{
    std::set<ZyppSel> selectablesToInstall;

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

    for ( std::set<ZyppSel>::iterator it = selectablesToInstall.begin();
	  it != selectablesToInstall.end();
	  ++it )
    {
	if ( (*it)->candidateObj() )
	    totalSize += zypp::ByteCount::SizeType((*it)->candidateObj()->installSize());
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

    // set sort strategy
    std::vector<std::string> pkgHeader = pkgList->getHeader( );
    pkgList->setSortStrategy( new NCPkgTableSort( pkgHeader ) );

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
    // create label with spaces to have enough space available for download size
    diskspaceLabel = YUI::widgetFactory()->createLabel ( hSplit4, "            " );

    YLayoutBox * vSplit = YUI::widgetFactory()->createVBox( split );
    replacePoint = YUI::widgetFactory()->createReplacePoint( vSplit );

    infoText = new NCPkgPackageDetails( replacePoint, " ", this );
    YUI_CHECK_NEW( infoText );

    YLayoutBox * bottom_bar = YUI::widgetFactory()->createHBox( vSplit );
    YAlignment *ll = YUI::widgetFactory()->createLeft( bottom_bar );

    helpMenu = new NCPkgMenuHelp (ll, _( "&Help" ), this);

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

    //Add only if requested by `opt(`repoMgr) flag - #381956
    if (isRepoMgrEnabled())
        configMenu = new NCPkgMenuConfig( menu_buttons, _( "C&onfiguration" ), this);

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
    //Search view is now default (#404694)
    searchPopup = new NCPkgFilterSearch( replPoint, YD_VERT, this );
    searchPopup->createLayout( replPoint );

    searchField = searchPopup->getSearchField();
    if ( searchField )
    {
        searchField->setKeyboardFocus();
        searchField->setNotify( true );
    }

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
    pkgList->fillHeader();

    // set sort strategy
    std::vector<std::string> pkgHeader = pkgList->getHeader( );
    pkgList->setSortStrategy( new NCPkgTableSort( pkgHeader ) );

    // label text + actions menu
    YLayoutBox * hSplit2 = YUI::widgetFactory()->createHBox( v );
    new NCLabel( hSplit2,  NCPkgStrings::PackageName() );
    packageLabel = YUI::widgetFactory()->createLabel ( hSplit2, "......................" );
    new NCSpacing( hSplit2, YD_HORIZ, true, 0.5 );
    actionMenu = new NCPkgMenuAction ( hSplit2, NCPkgStrings::Actions(), this );

    //Search parameters resp. filter description
    replPoint2 = YUI::widgetFactory()->createReplacePoint( hbox_bottom );
    replPoint2->setWeight(YD_HORIZ, 1);
    searchSet = new NCPkgSearchSettings( replPoint2, NCPkgStrings::SearchIn() );

    //Package description resp. package version table
    YLayoutBox * vSplit = YUI::widgetFactory()->createVBox( hbox_bottom );
    vSplit->setWeight(YD_HORIZ, 2);
    replacePoint = YUI::widgetFactory()->createReplacePoint( vSplit );
    infoText = new NCPkgPackageDetails( replacePoint, " ", this );
    YUI_CHECK_NEW( infoText );

    //Bottom button bar
    YAlignment *ll = YUI::widgetFactory()->createLeft( bottom_bar );
    helpMenu = new NCPkgMenuHelp (ll, _( "&Help" ), this);
    YUI_CHECK_NEW( helpMenu );

    //right-alignment for OK-Cancel
    YAlignment *right = YUI::widgetFactory()->createRight( bottom_bar );
    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( right );

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

    yuiMilestone() << "Filling package list: " << (NCWidget *) pkgList << endl;

    switch ( pkgList->getTableType() )
    {
	case NCPkgTable::T_Patches: {
	    fillPatchList( NCPkgMenuFilter::F_Needed );	// default: needed patches

	    // set the visible info to long description
	    pkgList->setVisibleInfo(NCPkgTable::I_PatchDescr);
	    // show the patch description of the current item
	    pkgList->showInformation ();
            pkgList->setKeyboardFocus();
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
		//Search view is the default (#404694)
		pkgList->setVisibleInfo(NCPkgTable::I_Technical);
		searchField->setKeyboardFocus();
	        break;
	}
	default:
	    break;
    }

    // if started with `repoMode or `summaryMode replace filter accordingly
    if ( repoMode )
    {
	replaceFilter ( NCPackageSelector::Repositories );
	if ( filterMain)
	    filterMain->setReposSelected();
    }
    else if ( summaryMode )
    {
	replaceFilter ( NCPackageSelector::Summary );
	if ( filterMain )
	    filterMain->setSummarySelected();
    }

    return true;

}
