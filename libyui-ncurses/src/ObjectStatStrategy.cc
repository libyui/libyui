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

#include <Y2PM.h>
#include <y2pm/PMYouPatchManager.h>

#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>


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
PMSelectable::UI_Status ObjectStatStrategy::getPackageStatus( PMObjectPtr objPtr )
{
    if ( objPtr && objPtr->hasSelectable() )
    {
	return objPtr->getSelectable()->status();
    }
    else
    {
	NCERR << "Object pointer not valid" << endl;
	return PMSelectable::S_NoInst;
    }
}

/////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::setObjectStatus()	
//
// Informs the package manager about the status change
//
bool ObjectStatStrategy::setObjectStatus( PMSelectable::UI_Status newstatus, PMObjectPtr objPtr )
{
    bool ok = false;
    
    if ( !objPtr || !objPtr->hasSelectable() )
    {
	NCERR << "Invalid package object" << endl;
	return false;
    }

    ok = objPtr->getSelectable()->set_status( newstatus );

    NCMIL << "Set status of: " <<  objPtr->getSelectable()->name() << " to: "
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
				      PMObjectPtr objPtr,
				      PMSelectable::UI_Status & newStat )
{
    if ( !objPtr )
	return false;
    
    bool valid = true;
    PMSelectable::UI_Status retStat = PMSelectable::S_NoInst;
    PMSelectable::UI_Status oldStatus = getPackageStatus( objPtr );
    
    // get the new status
    switch ( key )
    {
	case '-':
	    if ( oldStatus == PMSelectable::S_KeepInstalled 
		 || oldStatus == PMSelectable::S_AutoDel   )
	    {
		// if required, NCPkgTable::changeStatus() shows the delete notify
		retStat = PMSelectable:: S_Del;
	    }
	    else if ( oldStatus == PMSelectable::S_AutoUpdate
		      ||  oldStatus == PMSelectable::S_Update )
	    {
		retStat =  PMSelectable::S_KeepInstalled;
	    }
	    else if ( oldStatus == PMSelectable:: S_Install
		      || oldStatus == PMSelectable:: S_AutoInstall )
	    {
		retStat = PMSelectable::S_NoInst;	
	    }
	    else if (  oldStatus == PMSelectable::S_Taboo )
	    {
		retStat = PMSelectable::S_NoInst;
	    }
	    else if (  oldStatus == PMSelectable::S_Protected )
	    {
		retStat = PMSelectable::S_KeepInstalled;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '+':
	    if ( oldStatus == PMSelectable::S_NoInst
		 || oldStatus == PMSelectable::S_AutoInstall )
	    {
		// if required, NCPkgTable::changeStatus() shows the notify message
		retStat = PMSelectable::S_Install;
	    }
	    else if ( oldStatus ==  PMSelectable::S_Del
		      || oldStatus == PMSelectable::S_AutoDel)
	    {
		retStat = PMSelectable::S_KeepInstalled;
	    }
	    else if ( oldStatus == PMSelectable::S_AutoUpdate )
	    {
		retStat = PMSelectable::S_Update;
	    }
	    else
	    {
		valid = false;
	    }
	    
	    break;
	case '>':
	    if ( oldStatus == PMSelectable::S_KeepInstalled
		 ||  oldStatus == PMSelectable::S_Del
		 ||  oldStatus == PMSelectable::S_AutoDel )
	    {
		if ( objPtr->hasCandidateObj() )
		{
		    retStat = PMSelectable::S_Update;
		}
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '!':
	    if ( oldStatus == PMSelectable::S_NoInst || oldStatus == PMSelectable::S_AutoInstall )
	    {
		retStat = PMSelectable::S_Taboo;
	    }
	    else if  ( oldStatus == PMSelectable::S_KeepInstalled )
	    {
		retStat = PMSelectable::S_Protected;
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
bool ObjectStatStrategy::toggleStatus( PMObjectPtr objPtr,
				       PMSelectable::UI_Status & newStat )
{
    if ( !objPtr )
	return false;
    
    bool ok = true;
    
    PMSelectable::UI_Status oldStatus = getPackageStatus( objPtr ); 
    PMSelectable::UI_Status newStatus = oldStatus;

    switch ( oldStatus )
    {
	case PMSelectable:: S_Del:
	    newStatus = PMSelectable::S_KeepInstalled;
	    break;
	case PMSelectable::S_Install:
	    newStatus =PMSelectable::S_NoInst ;
	    break;
	case PMSelectable::S_Update:
	    newStatus = PMSelectable:: S_Del;
	    break;
	case PMSelectable:: S_KeepInstalled:
	    if ( objPtr->hasCandidateObj() )
	    {
		newStatus = PMSelectable::S_Update;
	    }
	    else
	    {
		newStatus = PMSelectable:: S_Del;
	    }
	    break;
	case PMSelectable::S_NoInst:
	    newStatus = PMSelectable::S_Install ;
	    break;
	case PMSelectable::S_AutoInstall:
	    newStatus = PMSelectable::S_NoInst;
	    break;
	case PMSelectable::S_AutoDel:
	    newStatus = PMSelectable:: S_KeepInstalled;
	    break;
	case PMSelectable::S_AutoUpdate:
	    newStatus = PMSelectable:: S_KeepInstalled;
	    break;
	case PMSelectable::S_Taboo:
	    newStatus = PMSelectable::S_NoInst;
	    break;
	case PMSelectable::S_Protected:
	    newStatus = PMSelectable::S_KeepInstalled;
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
				      PMObjectPtr objPtr,
				      PMSelectable::UI_Status & newStat )
{
    if ( !objPtr )
	return false;
    
    bool valid = true;
    PMSelectable::UI_Status retStat = PMSelectable::S_NoInst;
    PMSelectable::UI_Status oldStatus = getPackageStatus( objPtr );
    
    // get the new status
    switch ( key )
    {
	case '-':
	    if ( oldStatus == PMSelectable:: S_Install
		 || oldStatus == PMSelectable:: S_AutoInstall )
	    {
		retStat = PMSelectable::S_NoInst;	
	    }
	    else if (  oldStatus == PMSelectable:: S_Update )
	    {
		retStat = PMSelectable::S_KeepInstalled;
	    }
	    else
	    {
		valid = false;
	    }
	    break;
	case '+':
	    if ( oldStatus == PMSelectable::S_NoInst
		 || oldStatus == PMSelectable::S_AutoInstall )
	    {
		retStat = PMSelectable::S_Install;
	    }
	    else
	    {
		valid = false;
	    }
	    
	    break;
	case '>':
	    if ( oldStatus == PMSelectable::S_KeepInstalled )
	    {
		if ( objPtr->hasCandidateObj() )
		{
		    retStat = PMSelectable::S_Update;
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
bool PatchStatStrategy::toggleStatus( PMObjectPtr objPtr,
				      PMSelectable::UI_Status & newStat )
{
    if ( !objPtr )
	return false;
    
    bool ok = true;
    
    PMSelectable::UI_Status oldStatus = getPackageStatus( objPtr ); 
    PMSelectable::UI_Status newStatus = oldStatus;

    switch ( oldStatus )
    {
	case PMSelectable::S_Install:
	    newStatus =PMSelectable::S_NoInst ;
	    break;
	case PMSelectable::S_Update:
	    newStatus = PMSelectable:: S_KeepInstalled;
	    break;
	case PMSelectable::S_KeepInstalled:
	    if ( objPtr->hasCandidateObj() )
	    {
		newStatus = PMSelectable::S_Update;
	    }
	    break;
	case PMSelectable::S_NoInst:
	    newStatus = PMSelectable::S_Install ;
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
bool PatchStatStrategy::setObjectStatus( PMSelectable::UI_Status newstatus, PMObjectPtr objPtr )
{
    bool ok = false;

    if ( !objPtr || !objPtr->hasSelectable() )
    {
	NCERR << "Invalid patch object" << endl;
	return false;
    }

    ok = objPtr->getSelectable()->set_status( newstatus );
    NCMIL << "Set status of: " << objPtr->getSelectable()->name() << " to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;

    // additionally inform the YOU patch manager about the status change
    // (which sets the correct status of the patch packages)
    Y2PM::youPatchManager().updatePackageStates();
    
    return ok;
}

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
bool AvailableStatStrategy::setObjectStatus( PMSelectable::UI_Status newstatus,  PMObjectPtr objPtr )
{
    bool ok = false;

    if ( !objPtr || !objPtr->hasSelectable() )
    {
	return false;
    }

    ok = objPtr->getSelectable()->set_status( newstatus );
    if ( ok )
    {
	// this package is the candidate now
	bool ret = objPtr->getSelectable()->setUserCandidate( objPtr );
	NCMIL << "Set user candidate returns: " <<  (ret?"true":"false") << endl;	
    }
    NCMIL << "Set status of: " << objPtr->getSelectable()->name() << "to: "
	  << newstatus << " returns: " << (ok?"true":"false") << endl;
    
    return ok;
}


///////////////////////////////////////////////////////////////////
//
// AvailableStatStrategy::getStatus
//
// Returns the status of the certain package
//
PMSelectable::UI_Status AvailableStatStrategy::getPackageStatus( PMObjectPtr objPtr )
{
    PMSelectable::UI_Status retStatus = PMSelectable::S_NoInst;

    if ( !objPtr || !objPtr->hasSelectable() )
    {
	return retStatus;
    }

    PMSelectable::UI_Status status = objPtr->getSelectable()->status();
    
    if ( objPtr->hasInstalledObj()
	 && objPtr->edition() == objPtr->getInstalledObj()->edition() )
    {
	// installed package: show status S_KeepInstalled or S_Delete
	if ( status == PMSelectable::S_KeepInstalled
	     || status == PMSelectable::S_Del )
	    retStatus = status;
    }
    else if ( objPtr->isCandidateObj() )
    {
	if ( status != PMSelectable::S_KeepInstalled
	     && status != PMSelectable::S_Del )
	retStatus = status;
    }
    // else show S_NoInst
	
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

bool PatchPkgStatStrategy::setObjectStatus( PMSelectable::UI_Status newstatus,
					     PMObjectPtr objPtr )
{
    // it is not possible to set the status of the packages belonging to a certain patch
    return false;
}
