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
    T_Object
};

enum NCPkgStatus
{
    PkgToDelete,	// S_Del: delete installedObj
    PkgToInstall,	// S_Install: install candidateObj ( have no installedObj )
    PkgToUpdate,	// S_Update: install candidateObj ( have installedObj )
    PkgNoInstall,	// S_NoInst: is not/will not installed - no modification ( have no installedObj )
    PkgInstalled,	// S_KeepInstalled: keep this version - no modification ( have installedObj )
    PkgAutoInstall,	// S_Auto: automatically installed - like S_Install, but not requested by user
    PkgAutoDelete,	// S_AutoDel: automatically deleted - unmaintained packages (update)
    PkgTaboo,		// F_Taboo: Never install this
    PkgToReplace	// Replace
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

    friend std::ostream & operator<<( std::ostream & str, NCPkgStatus obj );
    
    /**
     * Gets the status information from the package manager.
     * @param objPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr objPtr );

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
     * Checks whether the new status is valid.
     * @param oldStatus	The old status
     * @param newStatus	The new status
     * @param objPtr	The object pointer 
     * @return bool
     */
    virtual bool validateNewStatus( const NCPkgStatus & oldStatus,
				    const NCPkgStatus & newStatus,
				    const PMObjectPtr & objPtr  );

    /**
     * Returns the type of the object
     * @return  NCStrategyType
     */
    NCStrategyType getType() { return type; }
    
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
// Class for strategies to handle status of patches
//------------------------------------------------------------
class PatchStatStrategy : public ObjectStatStrategy
{
public:

    PatchStatStrategy( );
    
    virtual ~PatchStatStrategy() {}

    /**
     * Checks whether the new status is valid (there are only few status 
     * changes possible for patches).
     * @param oldStatus	The old status
     * @param newStatus	The new status
     * @param objPtr	The object pointer 
     * @return bool
     */
    virtual bool validateNewStatus( const NCPkgStatus & oldStatus,
				    const NCPkgStatus & newStatus,
				    const PMObjectPtr & objPtr  );   
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
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr  objPtr );

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

