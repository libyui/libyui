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
// ObjectStatStrategy::getStatus()
//
// Gets status from package manager
//
PMSelectable::UI_Status ObjectStatStrategy::getStatus( PMObjectPtr objPtr )
{
    return objPtr->getSelectable()->status();
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
	NCERR << "Invalif package object" << endl;
	return false;
    }

    ok = objPtr->getSelectable()->set_status( newstatus );
    NCMIL << "Set status to: " << newstatus << " returns: " << (ok?"true":"false") << endl;
    
    return ok;
}

///////////////////////////////////////////////////////////////////
//
// ObjectStatStrategy::validateNewStatus()
//
// Checks whether the new status is valid
//
bool ObjectStatStrategy::validateNewStatus( const NCPkgStatus & oldStatus,
					    const NCPkgStatus & newStatus,
					    const  PMObjectPtr & objPtr  )
{
    bool valid = false;
    
    // check whether the status change is possible
    switch ( oldStatus )
    {
	case PkgToDelete: {
	    if ( ( newStatus == PkgInstalled )	||
		 ( objPtr->hasCandidateObj() &&
		   ( newStatus == PkgToUpdate
		     || newStatus == PkgToReplace ) ) )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToInstall: {
	    if ( newStatus == PkgNoInstall || newStatus == PkgTaboo )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToUpdate: {
	    if ( newStatus == PkgToDelete ||  newStatus == PkgInstalled ||
		 newStatus == PkgToReplace )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgInstalled: {
	    if ( ( newStatus == PkgToDelete ) 	||
		 ( objPtr->hasCandidateObj() &&
		   ( newStatus == PkgToUpdate
		     || newStatus == PkgToReplace
		     || newStatus == PkgToDelete ) ) )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToReplace: {
	    if ( newStatus == PkgToDelete || newStatus == PkgInstalled )
	    {
		valid = true;
	    }
	    break;  
	}
	case PkgNoInstall:
	    if ( newStatus == PkgToInstall || newStatus == PkgTaboo )
	    {
		valid = true;
	    }
	    break;
 	case PkgAutoInstall:
	    // FIXME show a warning !!!!!
	    if ( newStatus == PkgNoInstall )
	    {
		valid = true;
	    }
	    break;
	case PkgAutoDelete:
	    if ( newStatus == PkgInstalled || newStatus == PkgToUpdate )
	    {
		valid = true;
	    }
	    break;
	case PkgAutoUpdate:
	    if ( newStatus == PkgInstalled || newStatus == PkgToDelete )
	    {
		valid = true;
	    }
	    break;
	case PkgTaboo:
	    if ( newStatus == PkgNoInstall )
	    {
		valid = true;
	    }
	    break;
    }

    NCMIL << "Change from: " << oldStatus << " to "
	  << newStatus << ": " << (valid?"true":"false") << endl;

    return valid;
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


///////////////////////////////////////////////////////////////////
//
// PatchStatStrategy::validateNewStatus()
//
// Checks whether the new status is valid
//
bool PatchStatStrategy::validateNewStatus( const NCPkgStatus & oldStatus,
					   const NCPkgStatus & newStatus,
					   const  PMObjectPtr & objPtr  )
{
    bool valid = false;
    
    // check whether the status change is possible
    switch ( oldStatus )
    {
	case PkgToInstall: {
	    if ( newStatus == PkgNoInstall || newStatus == PkgTaboo )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgNoInstall:
	    if ( newStatus == PkgToInstall )
	    {
		valid = true;
	    }
	    break;
	case PkgTaboo:
	    if ( newStatus == PkgNoInstall )
	    {
		valid = true;
	    }
	    break;
	default:
	    valid = false;
    }

    NCMIL << "Change from: " << oldStatus << " to "
	  << newStatus << ": " << (valid?"true":"false") << endl;

    return valid;
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
PMSelectable::UI_Status AvailableStatStrategy::getStatus( PMObjectPtr objPtr )
{
    PMSelectable::UI_Status selStatus = objPtr->getSelectable()->status();
    PMSelectable::UI_Status retStatus = PMSelectable::S_NoInst;

    // set status for installed package or the candidate ( not for all availables )
    if ( objPtr->isInstalledObj()
	 || objPtr->isCandidateObj() )
    {
	retStatus = selStatus;
    }

    return retStatus;
}


ostream & operator<<( ostream & str, NCPkgStatus obj )
{
  switch ( obj ) {
#define ENUM_OUT(V) case V: return str << #V; break

    ENUM_OUT( PkgToDelete );
    ENUM_OUT( PkgToInstall );
    ENUM_OUT( PkgToUpdate );
    ENUM_OUT( PkgNoInstall );
    ENUM_OUT( PkgInstalled );
    ENUM_OUT( PkgAutoInstall );
    ENUM_OUT( PkgTaboo );
    ENUM_OUT( PkgToReplace );
    ENUM_OUT( PkgAutoDelete );
    ENUM_OUT( PkgAutoUpdate );

#undef ENUM_OUT
  }

  return str << "NCPkgStatus(UNKNOWN)";
}
