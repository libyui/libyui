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

   File:       ObjectStatStrategy.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   
/-*/
#ifndef ObjectStatStrategy_h
#define ObjectStatStrategy_h

#include  <y2pm/PMSelectable.h>


class PMObjectPtr;


enum NCStrategyType
    {
	T_Avail,
	T_Package,
	T_Patch,
	T_Dependency,
	T_Object
    };

//------------------------------------------------------------
// Abstract base class for strategies to get status for packages or patches 
//------------------------------------------------------------
class ObjectStatStrategy
{
protected:
    NCStrategyType type;

public:
    
    ObjectStatStrategy( );
    
    virtual ~ObjectStatStrategy() = 0; 

    /**
     * Gets the status information from the package manager.
     * @param objPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual PMSelectable::UI_Status getPackageStatus ( PMObjectPtr objPtr );

    /**
     * Informs the package manager about the new status.
     * @param newStatus The new package status
     * @param objPtr  The object pointer (e.g. a package pointer)
     * @return bool
     */ 
    virtual bool setPackageStatus ( PMSelectable::UI_Status newstatus,
				    PMObjectPtr objPtr
				    );

    /**
     * Returns the type of the object
     * @return  NCStrategyType
     */
    NCStrategyType getType() { return type; }
    
   /**
     * Sets the type of the object
     * @param  NCStrategyType
     */
    void setType( NCStrategyType strategy ) { type = strategy; }

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
// Class for strategies to handle status of patches
//------------------------------------------------------------
class PatchStatStrategy : public ObjectStatStrategy
{
public:

    PatchStatStrategy( );
    
    virtual ~PatchStatStrategy() {}

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
     * @param objPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual PMSelectable::UI_Status getPackageStatus ( PMObjectPtr  objPtr );

    /**
     * Informs the package manager about the new status and
     * additionally sets the candidate object to the user chosen object.
     * @param newStatus The new package status
     * @param objPtr  The package pointer
     * @return bool
     */ 
    virtual bool setPackageStatus( PMSelectable::UI_Status newstatus,
				   PMObjectPtr objPtr );
};

#endif

