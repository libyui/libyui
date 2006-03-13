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

   File:       ObjectStatStrategy.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "ObjectStatStrategy.h"

#include "NCTable.h"

#include "YQZypp.h"

#include <zypp/ui/Selectable.h>
#include <zypp/ResObject.h>


//------------------------------------------------------------
// Base class for strategies to handle status
//------------------------------------------------------------

//
// Constructor
//
ObjectStatStrategy::ObjectStatStrategy()
{
}

//
// Destructor - must be defined here (because it is pure virtual)
//
ObjectStatStrategy::~ObjectStatStrategy()
{
}

///////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::getPackageStatus()
//
// Gets status from package manager
//
ZyppStatus ObjectStatStrategy::getPackageStatus( ZyppSel slbPtr,
						 ZyppObj objPtr )
{
    if ( slbPtr )
    {
	return slbPtr->status();
    }
    else
    {
	NCERR << "Object pointer not valid" << endl;
	return S_NoInst;
    }
}

/////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::setObjectStatus()	
//
// Informs the package manager about the status change
//
bool ObjectStatStrategy::setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr, ZyppObj objPtr )
{
    bool ok = false;
    
    if ( !slbPtr )
    {
	NCERR << "Invalid package object" << endl;
	return false;
    }

    ok = slbPtr->set_status( newstatus );

    NCMIL << "Set status of: " <<  slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;
    
    return ok;
}

///////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::keyToStatus()
//
// Returns the corresponding status
//
bool ObjectStatStrategy::keyToStatus( const int & key,
				      ZyppSel slbPtr,
				      ZyppObj objPtr,
				      ZyppStatus & newStat )
{
    if ( !slbPtr )
	return false;
    
    bool valid = true;
    ZyppStatus retStat = S_NoInst;
    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    
    // get the new status
    switch ( key )
    {
	case '-':
	    if ( oldStatus == S_KeepInstalled 
		 || oldStatus == S_AutoDel   )
	    {
		// if required, NCPkgTable::changeStatus() shows the delete notify
		retStat = S_Del;
	    }
	    else if ( oldStatus == S_AutoUpdate
		      ||  oldStatus == S_Update )
	    {
		retStat =  S_KeepInstalled;
	    }
	    else if ( oldStatus == S_Install
		      || oldStatus == S_AutoInstall )
	    {
		retStat = S_NoInst;	
	    }
	    else if (  oldStatus == S_Taboo )
	    {
		retStat = S_NoInst;
	    }
	    else if (  oldStatus == S_Protected )
	    {
		retStat = S_KeepInstalled;
	    }
	    else
	    {
		valid = false;
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
	case '!':
	    if ( oldStatus == S_NoInst
		 || oldStatus == S_AutoInstall )
	    {
		retStat = S_Taboo;
	    }
	    else if  ( oldStatus == S_KeepInstalled
		       || oldStatus == S_AutoUpdate )
	    {
		retStat = S_Protected;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	default:
	    NCDBG <<  "Key not valid" << endl;
	    valid = false;
    }

    if ( valid )
	newStat = retStat;
    
    return valid;
}


///////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::toggleStatus()
//
// Returns the new status
//
bool ObjectStatStrategy::toggleStatus( ZyppSel slbPtr,
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
	    if ( slbPtr->hasCandidateObj() )
	    {
		newStatus = S_Update;
	    }
	    else
	    {
		newStatus = S_Del;
	    }
	    break;
	case S_NoInst:
	    newStatus = S_Install ;
	    break;
	case S_AutoInstall:
	    newStatus = S_NoInst;
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


//------------------------------------------------------------
// Class for strategies to get status for packages
//------------------------------------------------------------

//
// Constructor
//
PackageStatStrategy::PackageStatStrategy()
    : ObjectStatStrategy()
{
}



#ifdef FIXME
//------------------------------------------------------------
// Class for strategies to get status for patches
//------------------------------------------------------------

//
// Constructor
//
PatchStatStrategy::PatchStatStrategy()
    : ObjectStatStrategy()
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
				      ZyppStatus & newStat )
{
    if ( !slbPtr )
	return false;
    
    bool valid = true;
    ZyppStatus retStat = S_NoInst;
    ZyppStatus oldStatus = getPackageStatus( slbPtr, objPtr );
    
    // get the new status
    switch ( key )
    {
	case '-':
	    if ( oldStatus == S_Install
		 || oldStatus == S_AutoInstall )
	    {
		retStat = S_NoInst;	
	    }
	    else if (  oldStatus == S_Update )
	    {
		retStat = S_KeepInstalled;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '+':
	    if ( oldStatus == S_NoInst
		 || oldStatus == S_AutoInstall )
	    {
		retStat = S_Install;
	    }
	    else
	    {
		valid = false;
	    }
	    
	    break;
	case '>':
	    if ( oldStatus == S_KeepInstalled )
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
	    NCDBG <<  "Key not valid" << endl;
	    valid = false;
    }

    if ( valid )
	newStat = retStat;
    
    return valid;
}


///////////////////////////////////////////////////////////////////
//
// PatchStatStrategy::toggleStatus()
//
// Returns the new status
//
bool PatchStatStrategy::toggleStatus( ZyppSel slbPtr,
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
	    newStatus =S_NoInst ;
	    break;
	case S_Update:
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
	default:
	    newStatus = oldStatus;
    }

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
bool PatchStatStrategy::setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr )
{
    bool ok = false;

    if ( !slbPtr )
    {
	NCERR << "Invalid patch object" << endl;
	return false;
    }

    ok = slbPtr->set_status( newstatus );
    NCMIL << "Set status of: " << slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;

    // additionally inform the YOU patch manager about the status change
    // (which sets the correct status of the patch packages)
    Y2PM::youPatchManager().updatePackageStates();
    
    return ok;
}
#endif

//------------------------------------------------------------
// Class for strategies for depndencies
//------------------------------------------------------------

//
// Constructor
//
DependencyStatStrategy::DependencyStatStrategy()
    : ObjectStatStrategy()
{
}

//------------------------------------------------------------
// Class for strategies to get status for available packages
//------------------------------------------------------------

//
// Constructor
//
AvailableStatStrategy::AvailableStatStrategy()
    : ObjectStatStrategy()
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

    if ( !slbPtr )
    {
	return false;
    }

//    ok = slbPtr->set_status( newstatus );
    ok = slbPtr->set_status( S_Update ); // FIXME only works for installed
    if ( ok )
    {
	// this package is the candidate now
	bool ret = slbPtr->setCandidate( objPtr );
	// the new candidate can have a different one
	slbPtr->setLicenceConfirmed (false);
	NCMIL << "Set user candidate returns: " <<  (ret?"true":"false") << endl;	
    }
    NCMIL << "Set status of: " << slbPtr->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;
    
    return ok;
}


///////////////////////////////////////////////////////////////////
//
// AvailableStatStrategy::getStatus
//
// Returns the status of the certain package
//
ZyppStatus AvailableStatStrategy::getPackageStatus( ZyppSel slbPtr,
						    ZyppObj objPtr )
{
    ZyppStatus retStatus = S_NoInst;

    if ( !slbPtr || ! objPtr )
    {
	return retStatus;
    }

    // ZyppStatus status = slbPtr->status();

    if (objPtr == slbPtr->candidateObj())
	retStatus = S_KeepInstalled;
    else if (slbPtr->hasInstalledObj() &&
	     slbPtr->installedObj()->edition() == objPtr->edition() )
        retStatus = S_Del;
/*    
    if ( slbPtr->hasInstalledObj()
	 && slbPtr->edition() == slbPtr->getInstalledObj()->edition() )
    {
	// installed package: show status S_KeepInstalled or S_Delete
	if ( status == S_KeepInstalled
	     || status == S_Del )
	    retStatus = status;
    }
    else if ( slbPtr->isCandidateObj() )
    {
	if ( status != S_KeepInstalled
	     && status != S_Del )
	retStatus = status;
    }
    // else show S_NoInst
*/	
    return retStatus;
}

//------------------------------------------------------------
// Class for strategies to get status for available packages
//------------------------------------------------------------

//
// Constructor
//
UpdateStatStrategy::UpdateStatStrategy()
    : ObjectStatStrategy()
{
}


//------------------------------------------------------------
// Class for strategies to get status for patch packages
//------------------------------------------------------------

//
// Constructor
//
PatchPkgStatStrategy::PatchPkgStatStrategy()
    : ObjectStatStrategy()
{
}

bool PatchPkgStatStrategy::setObjectStatus( ZyppStatus newstatus,
					     ZyppSel slbPtr, ZyppObj objPtr )
{
    // it is not possible to set the status of the packages belonging to a certain patch
    return false;
}
