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
    type = T_Object;
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
	NCERR << "Pointer not valid" << endl;
	return PMSelectable::S_NoInst;
    }
}

/////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::setPackageStatus()	
//
// Informs the package manager
//
bool ObjectStatStrategy::setPackageStatus( PMSelectable::UI_Status newstatus, PMObjectPtr objPtr )
{
    bool ok = false;

    if ( !objPtr || !objPtr->hasSelectable() )
    {
	NCERR << "Invalid package object" << endl;
	return false;
    }

    ok = objPtr->getSelectable()->set_status( newstatus );
    NCMIL << "Set status to: " << newstatus << " returns: " << (ok?"true":"false") << endl;
    
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
    type = T_Package;
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
    type = T_Patch;
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
    type = T_Avail;
}

///////////////////////////////////////////////////////////////////
//
// AvailableStatStrategy::setPackageStatus
//
// Informs the package manager about the new status (sets the candidate)
//
bool AvailableStatStrategy::setPackageStatus( PMSelectable::UI_Status newstatus,  PMObjectPtr objPtr )
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
    NCMIL << "Set status to: " << newstatus << " returns: " << (ok?"true":"false") << endl;
    
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
    PMSelectable::UI_Status selStatus = objPtr->getSelectable()->status();
    PMSelectable::UI_Status retStatus = PMSelectable::S_NoInst;

    // return status for installed package or the candidate ( not for all availables )
    if ( objPtr->isInstalledObj()
	 || objPtr->isCandidateObj() )
    {
	retStatus = selStatus;
    }

    return retStatus;
}

