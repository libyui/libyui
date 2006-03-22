/*-----------------------------------------------------------*- c++ -*-\
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

   File:       ObjectStatStrategy.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   
/-*/
#ifndef ObjectStatStrategy_h
#define ObjectStatStrategy_h

#include "YQZypp.h"
#include <zypp/ui/Selectable.h>

//------------------------------------------------------------
// Abstract base class for strategies to get status for packages or patches 
//------------------------------------------------------------
class ObjectStatStrategy
{

public:
    
    ObjectStatStrategy( );
    
    virtual ~ObjectStatStrategy() = 0; 

    /**
     * Gets the status information from the package manager.
     * @param slbPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual ZyppStatus getPackageStatus ( ZyppSel slbPtr, ZyppObj objPtr );

    /**
     * Informs the package manager about the new status.
     * @param newStatus The new package status
     * @param slbPtr  The selectable pointer (e.g. a package pointer)
     * @param pkgPtr  The object pointer (used for candidate selection)
     * @return bool
     */ 
    virtual bool setObjectStatus ( ZyppStatus newstatus,
				   ZyppSel slbPtr,
				   ZyppObj objPtr
				    );
   /**
     * Returns the new status to the given key (respecting the old
     * status of th eobject).
     * @param key The userinput (the key which is pressed)
     * @param slbPtr  The object pointer (e.g. a package pointer)
     * @param newStatus The new package status
     * @return bool
     */  
    virtual bool keyToStatus( const int & key,
			      ZyppSel slbPtr,
			      ZyppObj objPtr,
			      ZyppStatus & newStat );
    
    /**
     * Toggles the package status (e.g. from installed to delete)
     * @param The object pointer
     * @return bool
     */
    virtual bool toggleStatus( ZyppSel slbPtr,
			       ZyppObj objPtr,
			        ZyppStatus & newStat );
};

//------------------------------------------------------------
// Class for strategies to handle status of packages
//------------------------------------------------------------
class PackageStatStrategy : public ObjectStatStrategy
{
public:

    PackageStatStrategy( );
    
    virtual ~PackageStatStrategy() {}
    
};

//------------------------------------------------------------
// Class for strategies to handle dependencies
//------------------------------------------------------------
class DependencyStatStrategy : public ObjectStatStrategy
{
public:

    DependencyStatStrategy( );
    
    virtual ~DependencyStatStrategy() {}
    
};

//------------------------------------------------------------
// Class for strategies of update
//------------------------------------------------------------
class UpdateStatStrategy : public ObjectStatStrategy
{
public:

    UpdateStatStrategy( );
    
    virtual ~UpdateStatStrategy() {}
    
};

//------------------------------------------------------------
// Class for strategies to handle status of packages belonging to a patch
//------------------------------------------------------------
class PatchPkgStatStrategy : public ObjectStatStrategy
{
public:

    PatchPkgStatStrategy( );
    
    virtual ~PatchPkgStatStrategy() {}

    /**
     * It is not possible to set the package status for packages belonging to a patch,
     * i.e. returns always false.
     */
    virtual bool setObjectStatus( ZyppStatus newstatus,
				  ZyppSel slbPtr,
				  ZyppObj objPtr );

};

    

//------------------------------------------------------------
// Class for strategies to handle status of patches
//------------------------------------------------------------
class PatchStatStrategy : public ObjectStatStrategy
{
public:

    PatchStatStrategy( );
    
    virtual ~PatchStatStrategy() {}

     /**
     * Returns the new status to the given key (respecting the old
     * status of the patch).
     * @param key The userinput (the key which is pressed)
     * @param slbPtr  The object pointer (the patch pointer)
     * @param newStatus The new package status
     * @return bool
     */  
    virtual bool keyToStatus( const int & key,
			      ZyppSel slbPtr,
			      ZyppObj objPtr,
			      ZyppStatus & newStat );
    
    /**
     * Toggles the patch status (e.g. from selected to unselected)
     * @param The object pointer
     * @return bool
     */
    virtual bool toggleStatus( ZyppSel slbPtr,
			       ZyppObj objPtr,
			        ZyppStatus & newStat );

    /**
     * Sets the status of the patch AND the status of the patch packages
     * @param newStatus The new package status
     * @param slbPtr  The object pointer (e.g. a package pointer)
     * @return bool
     */
    virtual bool setObjectStatus( ZyppStatus newstatus, ZyppSel slbPtr, ZyppObj objPtr );
};



//------------------------------------------------------------
// Class for strategies to handle status of available packages
//------------------------------------------------------------
class AvailableStatStrategy : public ObjectStatStrategy
{
public:
    
    AvailableStatStrategy( );
    
    virtual ~AvailableStatStrategy() {}

    /**
     * Gets the status information from the package manager - and
     * returns the status for a certain package.
     * @param slbPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual ZyppStatus getPackageStatus ( ZyppSel  slbPtr, ZyppObj objPtr );

    /**
     * Informs the package manager about the new status and
     * additionally sets the candidate object to the user chosen object.
     * @param newStatus The new package status
     * @param slbPtr  The package pointer
     * @param pkgPtr  The object pointer (used for candidate selection)
     * @return bool
     */ 
    virtual bool setObjectStatus( ZyppStatus newstatus,
				   ZyppSel slbPtr, ZyppObj objPtr );
};

#endif

