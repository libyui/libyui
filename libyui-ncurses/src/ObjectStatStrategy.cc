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


/////////////////////////////////////////////////////////////////
//
//	ObjectStatStrategy::setPackageStatus()	
//
//	Informs the package manager
//
bool ObjectStatStrategy::setPackageStatus( PMObjectPtr objPtr, PMSelectable::UI_Status newstatus )
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

PackageStatStrategy::PackageStatStrategy()
    : ObjectStatStrategy()
{
    
}

PMSelectable::UI_Status PackageStatStrategy::getStatus( PMObjectPtr objPtr )
{
    return objPtr->getSelectable()->status();
}

//------------------------------------------------------------
// Class for strategies to get status for patches
//------------------------------------------------------------

PatchStatStrategy::PatchStatStrategy()
    : ObjectStatStrategy()
{
    
}

PMSelectable::UI_Status PatchStatStrategy::getStatus( PMObjectPtr objPtr )
{
    return objPtr->getSelectable()->status();
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

AvailableStatStrategy::AvailableStatStrategy()
    : ObjectStatStrategy()
{
    
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : AvailableStatStrategy::setPackageStatus
//	METHOD TYPE : bool
//
//	DESCRIPTION : informs the package manager
//
bool AvailableStatStrategy::setPackageStatus( PMObjectPtr objPtr, PMSelectable::UI_Status newstatus )
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

PMSelectable::UI_Status AvailableStatStrategy::getStatus( PMObjectPtr objPtr )
{
    PMSelectable::UI_Status selStatus = objPtr->getSelectable()->status();
    PMSelectable::UI_Status retStatus = PMSelectable::S_NoInst;

    if ( objPtr->isInstalledObj() )
    {
	retStatus = selStatus;
    }
    else if (objPtr->isCandidateObj())
    {
	if ((selStatus == PMSelectable::S_Install) || (selStatus == PMSelectable::S_Update))
	{
	    retStatus = selStatus;
	}
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

#undef ENUM_OUT
  }

  return str << "NCPkgStatus(UNKNOWN)";
}
