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

   File:       NCPkgStatusStrategy.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgStatusStrategy.h"
#include "NCTable.h"
#include "NCZypp.h"
#include "NCPopupInfo.h"
#include "NCPkgStrings.h"

#include <zypp/ui/Selectable.h>
#include <zypp/base/LogTools.h>
#include <zypp/ResObject.h>

using std::endl;

//------------------------------------------------------------
// Base class for strategies to handle status
//------------------------------------------------------------

//
// Constructor
//
NCPkgStatusStrategy::NCPkgStatusStrategy()
{
}

//
// Destructor - must be defined here (because it is pure virtual)
//
NCPkgStatusStrategy::~NCPkgStatusStrategy()
{
}

///////////////////////////////////////////////////////////////////
//
// NCPkgStatusStrategy::getPackageStatus()
//
// Gets status from package manager
//
ZyppStatus NCPkgStatusStrategy::getPackageStatus( ZyppSel slbPtr,
						 ZyppObj objPtr )
{
    if ( slbPtr )
    {
	return slbPtr->status();
    }
    else
    {
	yuiError() << "Selectable pointer not valid" << endl;
	return S_NoInst;
    }
}

/////////////////////////////////////////////////////////////////
//
// NCPkgStatusStrategy::setObjectStatus()
//
// Informs the package manager about the status change
//
bool NCPkgStatusStrategy::setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;

    if ( !slbPtr )
    {
	yuiError() << "Invalid package object" << endl;
	return false;
    }

    ok = slbPtr->setStatus( newstatus );

    yuiMilestone() << "Set status of: " <<  slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;

    return ok;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgStatusStrategy::keyToStatus()
//
// Returns the corresponding status
//
bool NCPkgStatusStrategy::keyToStatus( const int & key,
				      ZyppSel slbPtr,
				      ZyppObj objPtr,
				      ZyppStatus & newStat )
{
    if ( !slbPtr )
	return false;

    bool valid = true;
    ZyppStatus retStat = S_NoInst;
    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    bool installed = !slbPtr->installedEmpty();

    // get the new status
    switch ( key )
    {
	case '-':
	    if ( installed )	// installed package -> always set status to delete
	    {
		// if required, NCPkgTable::changeStatus() shows the delete notify
		retStat = S_Del;
	    }
	    else
	    {
		retStat = S_NoInst;
	    }
	    break;
	case '+':
	    if ( oldStatus == S_NoInst
		 || oldStatus == S_AutoInstall )
	    {
		// if required, NCPkgTable::changeStatus() shows the notify message
		retStat = S_Install;
	    }
	    else if ( oldStatus ==  S_Del
		      || oldStatus == S_AutoDel)
	    {
		retStat = S_KeepInstalled;
	    }
	    else if ( oldStatus == S_AutoUpdate )
	    {
		retStat = S_Update;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '>':
	    if ( oldStatus == S_KeepInstalled
		 ||  oldStatus == S_Del
		 ||  oldStatus == S_AutoDel )
	    {
		if ( slbPtr->hasCandidateObj() )
		{
		    retStat = S_Update;
		}
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	//this is the case for 'going back' i.e. S_Install -> S_NoInst, S_Update -> S_KeepInstalled
	//not for S_Del, since '+' key does this
	case '<':
	    if ( oldStatus == S_Install
		 || oldStatus == S_AutoInstall )
	    {
		retStat = S_NoInst;
	    }
	    else if ( oldStatus == S_Update
		      || oldStatus == S_AutoUpdate )
	    {
		retStat = S_KeepInstalled;
	    }
	    break;
	case '!':	// set S_Taboo
	    if ( !installed )
	    {
		retStat = S_Taboo;
	    }
	    break;
    	case '*':	// set S_Protected
	    if ( installed )
	    {
		retStat = S_Protected;
	    }
	    break;
	default:
	    yuiDebug() <<  "Key not valid" << endl;
	    valid = false;
    }

    if ( valid )
	newStat = retStat;

    return valid;
}


///////////////////////////////////////////////////////////////////
//
// NCPkgStatusStrategy::toggleStatus()
//
// Returns the new status
//
bool NCPkgStatusStrategy::toggleStatus( ZyppSel slbPtr,
				       ZyppObj objPtr,
				       ZyppStatus & newStat )
{
    if ( !slbPtr )
	return false;

    bool ok = true;

    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    ZyppStatus newStatus = oldStatus;
    ZyppPattern patPtr = tryCastToZyppPattern (objPtr);

    switch ( oldStatus )
    {
	case S_Del:
	    newStatus = S_KeepInstalled;
	    break;
	case S_Install:
	    newStatus =S_NoInst ;
	    break;
	case S_Update:
	    newStatus = S_Del;
	    break;
	case S_KeepInstalled:
	    if ( patPtr )
		newStatus = S_Install;

	    else if ( slbPtr->hasCandidateObj() )
	    {
		newStatus = S_Update;
	    }
	    else
	    {
		newStatus = S_Del;
	    }
	    break;
	case S_NoInst:
	    if ( slbPtr->hasCandidateObj() || patPtr )
            {
	        newStatus = S_Install;
            }
	    else
	    {
		yuiWarning() << "No candidate object for " << slbPtr->theObj()->name().c_str() << endl;
		newStatus = S_NoInst;
            }
	    break;
	case S_AutoInstall:
	    //Correct transition is S_Taboo -> #254816
	    newStatus = S_Taboo;
	    break;
	case S_AutoDel:
	    newStatus = S_KeepInstalled;
	    break;
	case S_AutoUpdate:
	    newStatus = S_KeepInstalled;
	    break;
	case S_Taboo:
	    newStatus = S_NoInst;
	    break;
	case S_Protected:
	    newStatus = S_KeepInstalled;
	    break;
    }

    yuiMilestone() << "Status toogled: old " << oldStatus << ", new " << newStatus << endl;
    newStat = newStatus;

    return ok;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgStatusStrategy::solveResolvableCollections()
//
// Do a "small" solver run
//
void NCPkgStatusStrategy::solveResolvableCollections()
{
    zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();
    resolver->resolvePool();
}



//------------------------------------------------------------
// Class for strategies to get status for packages
//------------------------------------------------------------

//
// Constructor
//
PackageStatStrategy::PackageStatStrategy()
    : NCPkgStatusStrategy()
{
}




//------------------------------------------------------------
// Class for strategies to get status for patches
//------------------------------------------------------------

//
// Constructor
//
PatchStatStrategy::PatchStatStrategy()
    : NCPkgStatusStrategy()
{
}


///////////////////////////////////////////////////////////////////
//
// PatchStatStrategy::keyToStatus()
//
// Returns the corresponding status
//
bool PatchStatStrategy::keyToStatus( const int & key,
				     ZyppSel slbPtr,
				     ZyppObj objPtr,
				     ZyppStatus & newStat )
{
    if ( !slbPtr || !slbPtr->hasCandidateObj() )
	return false;

    bool valid = true;
    ZyppStatus retStat = S_NoInst;
    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    bool toBeInst = slbPtr->candidateObj().status().isToBeInstalled();
    bool isRelevant = slbPtr->candidateObj().isRelevant();
    bool isBroken = slbPtr->candidateObj().isBroken();

    yuiMilestone() << slbPtr->name() << ": " << (toBeInst?"to be installed":"not to be installed,")
                   << " old status: " << oldStatus << endl;

    // get the new status
    switch ( key )
    {
	case '-':
            // To be installed includes S_Install and S_AutoInstall
	    if ( toBeInst )
            {
		retStat = S_NoInst;
	    }
            else if ( oldStatus == S_Taboo )
            {
                if ( isBroken )
                {
                    retStat = S_Install;
                }
                else
                {
                    retStat = S_NoInst;
                }
            }
            else // patches cannot be deleted
            {
                valid = false;
            }
	    break;
	case '+':
            // only relevant patches can be installed
	    if ( isRelevant &&
                 ( oldStatus == S_NoInst ||
                   oldStatus == S_AutoInstall ||
                   oldStatus == S_KeepInstalled ) )
	    {
		retStat = S_Install;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '!':
            {
                // For patches there isn't an installed object available (patches are not installed,
                // they can be satisfied because required version/s of the patch package/s is/are
                // installed). Therefore they only can be set to S_Taboo (not to S_Protected).
                retStat = S_Taboo;
	    }
	    break;
	default:
	    yuiDebug() <<  "Key not valid" << endl;
	    valid = false;
    }

    if ( valid )
    {
	newStat = retStat;
    }
    yuiMilestone() << "New status: " << newStat << endl;

    return valid;
}

///////////////////////////////////////////////////////////////////
//
// PatchStatStrategy::toggleStatus()
//
// Returns the new status
//
bool PatchStatStrategy::toggleStatus( ZyppSel slbPtr,
				      ZyppObj objPtr,
				      ZyppStatus & newStat )
{
    if ( !slbPtr || !slbPtr->hasCandidateObj() )
	return false;

    bool ok = true;

    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    bool isBroken = slbPtr->candidateObj().isBroken();
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
	case S_Install:
	case S_AutoInstall:
	    newStatus = S_NoInst ;
	    break;
	case S_KeepInstalled:
            newStatus = S_Install;
	    break;
	case S_NoInst:
	    newStatus = S_Install ;
	    break;
    	case S_Taboo:
            if ( isBroken )
            {
                newStatus = S_Install;
            }
            else
            {
                newStatus = S_NoInst;
            }
	    break;
	default:
	    newStatus = oldStatus;
    }
    yuiMilestone() << "Status toogled: old " << oldStatus << ", new " << newStatus << endl;

    newStat = newStatus;

    return ok;
}


/////////////////////////////////////////////////////////////////
//
// PatchStatStrategy::setObjectStatus()
//
// Inform the package manager about the status change
// of the patch
//
bool PatchStatStrategy::setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;

    if ( !slbPtr )
    {
	yuiError() << "Invalid patch object" << endl;
	return false;
    }

    ok = slbPtr->setStatus( newstatus );
    yuiMilestone() << "Set status of: " << slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;

    // do a solver run
    solveResolvableCollections();

    return ok;
}

//------------------------------------------------------------
// Class for strategies for selections
//------------------------------------------------------------

//
// Constructor
//
SelectionStatStrategy::SelectionStatStrategy()
    : NCPkgStatusStrategy()
{
}

/////////////////////////////////////////////////////////////////
//
// SelectionStatStrategy::setObjectStatus()
//
// Inform the package manager about the status change
// of the selection
//
bool SelectionStatStrategy::setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;

    if ( !slbPtr || !objPtr )
    {
	yuiError() << "Invalid selection" << endl;
	return false;
    }

    // workaround to clean previous state properly see bnc#916568 comment#8 especially
    // basically problem is that if going from status taboo to status noinst sat solver
    // is set to only remove lock and let solver to install it. What is actually expected
    // by users is soft lock of package or pattern to not install it unless required
    // so as workaround cycling thrue states, use different state as previous.
    if (newstatus == S_NoInst)
      slbPtr->setStatus( S_Protected );

    ok = slbPtr->setStatus( newstatus );
    yuiMilestone() << "Set status of: " << slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;

    // do a solver run -> solver runs in NCPkgTable::changeStatus()
    // solveResolvableCollections();

    return ok;
}

//------------------------------------------------------------
// Class for strategies for depndencies
//------------------------------------------------------------

//
// Constructor
//
DependencyStatStrategy::DependencyStatStrategy()
    : NCPkgStatusStrategy()
{
}

//------------------------------------------------------------
// Class for strategies to get status for available packages
//------------------------------------------------------------

//
// Constructor
//
AvailableStatStrategy::AvailableStatStrategy()
    : NCPkgStatusStrategy()
{
}

///////////////////////////////////////////////////////////////////
//
// AvailableStatStrategy::setObjectStatus
//
// Informs the package manager about the new status (sets the candidate)
//
bool AvailableStatStrategy::setObjectStatus( ZyppStatus newstatus,
                                             ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;

    if ( !slbPtr || !objPtr )
    {
	return false;
    }

    ZyppObj newCandidate = objPtr;

    if ( newCandidate != slbPtr->candidateObj() )
    {
        yuiMilestone() << "CANDIDATE changed" << endl;

        // Change status of selectable
        ZyppStatus status = slbPtr->status();

        if ( slbPtr->installedObj() &&
             slbPtr->installedObj()->edition() == newCandidate->edition() &&
             slbPtr->installedObj()->vendor() == newCandidate->vendor()
             )
        {
            yuiMilestone() << "Identical package installed" << endl;
            // Switch back to the original instance -
            // the version that was previously installed
            status = S_KeepInstalled;
        }
        else
        {
            switch ( status )
            {
                case S_KeepInstalled:
                case S_Protected:
                case S_AutoDel:
                case S_AutoUpdate:
                case S_Del:
                case S_Update:

                    status = S_Update;
                    break;

                case S_NoInst:
                case S_Taboo:
                case S_Install:
                case S_AutoInstall:
                    status = S_Install;
                    break;
            }
        }

        // Set candidate
        ok = bool( slbPtr->setCandidate( newCandidate ) );
        yuiMilestone() << "Set user candidate returns: " <<  (ok?"true":"false") << endl;
        if ( ok )
        {
            // Set status
            ok = slbPtr->setStatus( status );
            yuiMilestone() << "Set status of: " << slbPtr->name() << " to: "
                           << status << " returns: " << (ok?"true":"false") << endl;
        }
    }

    return ok;
}


//------------------------------------------------------------
// Class for strategies to get status for multi version packages
//------------------------------------------------------------

//
// Constructor
//
MultiVersionStatStrategy::MultiVersionStatStrategy()
    : NCPkgStatusStrategy()
{
}

///////////////////////////////////////////////////////////////////
//
// MultiVersionStatStrategy::getPackageStatus()
//
// Gets status from package manager
//
ZyppStatus MultiVersionStatStrategy::getPackageStatus( ZyppSel slbPtr,
                                                       ZyppObj objPtr )
{
    if ( !slbPtr || !objPtr )
    {
        yuiError() << "Selectable pointer not valid" << endl;
        return S_NoInst;
    }

    zypp::PoolItem itemPtr ( objPtr->satSolvable() );
    return slbPtr->pickStatus( itemPtr );
}

///////////////////////////////////////////////////////////////////
//
// MultiVersionStatStrategy::setObjectStatus
//
// Checking for multiversion and not-multiversion conflicts and
// informs the package manager about the new status.
//
bool MultiVersionStatStrategy::setObjectStatus( ZyppStatus newstatus,
                                                ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;

    if ( !slbPtr || !objPtr )
    {
	return false;
    }
    zypp::PoolItem itemPtr ( objPtr->satSolvable() );

    bool multiVersion = itemPtr->multiversionInstall();
    yuiMilestone() << "Selected: "
		   << ( multiVersion ? "Multiversion " : "Non-Multiversion " )
		   << itemPtr;

    if ( anyMultiVersionToInstall( slbPtr, !multiVersion ) )
    {
	yuiMilestone() << "Multiversion and non-multiversion conflict!" << endl;

	if ( mixedMultiVersionPopup( multiVersion ) )
	{
	    ok = slbPtr->setPickStatus( itemPtr, newstatus );
	    yuiMilestone() << "Set new status of: "<< slbPtr->name() << ", " << objPtr->edition()
			   << " to: " << newstatus << " returns: " <<  (ok?"true":"false") << endl;
        }
	else
	{
	    yuiMilestone() << "Selection canceled by the user.";
	}
    }
    else
    {
        ok = slbPtr->setPickStatus( itemPtr, newstatus );
        yuiMilestone() << "Set new status of: "<< slbPtr->name() << ", " << objPtr->edition()
                       << " to: " << newstatus << " returns: " <<  (ok?"true":"false") << endl;
    }

    return ok;
}

///////////////////////////////////////////////////////////////////
//
// MultiVersionStatStrategy::anyMultiVersionToInstall
//
// Check if any package version is marked for installation where its
// 'multiversion' flag is set to 'multiversion'.
//
bool MultiVersionStatStrategy::anyMultiVersionToInstall( ZyppSel slbPtr, bool multiversion ) const
{
    if ( ! slbPtr )
	return false;

    zypp::ui::Selectable::available_iterator it = slbPtr->availableBegin();

    while ( it != slbPtr->availableEnd() )
    {
	if ( it->multiversionInstall() == multiversion )
	{
	    switch ( slbPtr->pickStatus( *it ) )
	    {
		case S_Install:
		case S_AutoInstall:
		    yuiMilestone() << "Found " << ( multiversion ? "multiversion" : "non-multiversion" )
				   << " to install" << endl;
		    return true;
	        case S_KeepInstalled:
		    yuiMilestone() << "Found " << ( multiversion ? "multiversion" : "non-multiversion" )
				   << " wich is already installed" << endl;
		    return true;
		default:
		    break;
	    }
	}

	++it;
    }

    yuiMilestone() << "No " << ( multiversion ? "multiversion" : "non-multiversion" )
		   << " to install" << endl;
    return false;
}

///////////////////////////////////////////////////////////////////
//
// MultiVersionStatStrategy::mixedMultiVersionPopup
//
// Ask user if he really wants to install incompatible package versions.
// Return 'true' if he hits [Continue], 'false' if [Cancel].
//
bool MultiVersionStatStrategy::mixedMultiVersionPopup( bool multiversion ) const
{
    std::string msg = NCPkgStrings::MultiversionIntro();

    if ( multiversion )
    {
        msg += NCPkgStrings::MultiversionText();
    }
    else
    {
        msg += NCPkgStrings::NotMultiversionText();
    }

    NCPopupInfo * cancelMsg = new NCPopupInfo( wpos( (NCurses::lines()-22)/2, (NCurses::cols()-45)/2 ),
					       NCPkgStrings::MultiversionHead(),
					       msg,
					       NCPkgStrings::ContinueLabel(),
					       NCPkgStrings::CancelLabel()
					       );
    cancelMsg->setPreferredSize( 60, 15 );
    cancelMsg->focusCancelButton();
    NCursesEvent input = cancelMsg->showInfoPopup( );

    YDialog::deleteTopmostDialog();

    return !(input == NCursesEvent::cancel);
}




//------------------------------------------------------------
// Class for strategies to get status for update packages
//------------------------------------------------------------

//
// Constructor
//
UpdateStatStrategy::UpdateStatStrategy()
    : NCPkgStatusStrategy()
{
}


//------------------------------------------------------------
// Class for strategies to get status for patch packages
//------------------------------------------------------------

//
// Constructor
//
PatchPkgStatStrategy::PatchPkgStatStrategy()
    : NCPkgStatusStrategy()
{
}

bool PatchPkgStatStrategy::setObjectStatus( ZyppStatus newstatus,
					     ZyppSel slbPtr, ZyppObj objPtr )
{
    // it is not possible to set the status of the packages belonging to a certain patch
    return false;
}
