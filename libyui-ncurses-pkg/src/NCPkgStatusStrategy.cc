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

#include <zypp/ui/Selectable.h>
#include <zypp/ResObject.h>


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
	    if ( installed )	// installed ->set status to delete
	    {
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
		retStat = S_Install;
	    }
   	    else if ( oldStatus ==  S_Del
		      || oldStatus == S_AutoDel)
	    {
		retStat = S_KeepInstalled;
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
	default:
	    yuiDebug() <<  "Key not valid" << endl;
	    valid = false;
    }

    if ( valid )
	newStat = retStat;

    return valid;
}

#if EXTRA_PATCH_STRATEGY

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
    if ( !slbPtr )
	return false;

    bool ok = true;

    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
	case S_Install:
	case S_AutoInstall:
	    newStatus =S_NoInst ;
	    break;
	case S_Update:
	case S_AutoUpdate:
	case S_AutoDel:
	    newStatus = S_KeepInstalled;
	    break;
	case S_KeepInstalled:
	    if ( slbPtr->hasCandidateObj() )
	    {
		newStatus = S_Update;
	    }
	    break;
	case S_NoInst:
	    newStatus = S_Install ;
	    break;
    	case S_Taboo:
	    newStatus = S_NoInst;
	    break;
	case S_Protected:
	    newStatus = S_KeepInstalled;
	    break;
	default:
	    newStatus = oldStatus;
    }

    newStat = newStatus;

    return ok;
}
#endif

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
bool AvailableStatStrategy::setObjectStatus( ZyppStatus newstatus,  ZyppSel slbPtr, ZyppObj objPtr )
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
	     slbPtr->installedObj()->edition() == newCandidate->edition() )
	{
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

	// Set status
	ok = slbPtr->setStatus( status );
	yuiMilestone() << "Set status of: " << slbPtr->name() << " to: "
	  << status << " returns: " << (ok?"true":"false") << endl;

	// Set candidate
	ok = slbPtr->setCandidate( newCandidate );
	yuiMilestone() << "Set user candidate returns: " <<  (ok?"true":"false") << endl;

    }

    return ok;
}

//------------------------------------------------------------
// Class for strategies to get status for available packages
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
