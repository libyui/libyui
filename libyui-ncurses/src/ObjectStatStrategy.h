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


enum NCPkgStatus
{
    PkgToDelete,	// S_Del: delete installedObj
    PkgToInstall,	// S_Install: install candidateObj ( have no installedObj )
    PkgToUpdate,	// S_Update: install candidateObj ( have installedObj )
    PkgNoInstall,	// S_NoInst: is not/will not installed - no modification ( have no installedObj )
    PkgInstalled,	// S_KeepInstalled: keep this version - no modification ( have installedObj )
    PkgAutoInstall,	// S_Auto: automatically installed - like S_Install, but not requested by user
    PkgTaboo,		// F_Taboo: Never install this
    PkgToReplace	// Replace
};

//------------------------------------------------------------
// Abstract base class for strategies to get status for packages or patches 
//------------------------------------------------------------
class ObjectStatStrategy
{
public:

    ObjectStatStrategy( ) {}
    
    virtual ~ObjectStatStrategy() {} 

    friend std::ostream & operator<<( std::ostream & str, NCPkgStatus obj );
    
    /**
     * Calculate new installation status of given package.
     *
     * @param objPtr The package whose status to calculate.
     * @return new status of given package
     **/
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr objPtr ) = 0;

    /**
     * Informs the package manager.
     * @param pkgPtr  The package pointer
     * @param newStatus The new package status
     * @return bool
     */ 
    virtual bool setPackageStatus ( PMObjectPtr objPtr,
				    PMSelectable::UI_Status newstatus );

    //virtual toggleStatus() = 0;

    virtual bool validateNewStatus( const NCPkgStatus & oldStatus,
				    const NCPkgStatus & newStatus,
				    const PMObjectPtr & objPtr  );
};

//------------------------------------------------------------
// Class for strategies to handle status of packages
//------------------------------------------------------------
class PackageStatStrategy : public ObjectStatStrategy
{
public:

    PackageStatStrategy( );
    
    virtual ~PackageStatStrategy() {}
    
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr objPtr );
    
};

//------------------------------------------------------------
// Class for strategies to handle status of patches
//------------------------------------------------------------
class PatchStatStrategy : public ObjectStatStrategy
{
public:

    PatchStatStrategy( );
    
    virtual ~PatchStatStrategy() {}
    
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr objPtr );

    bool validateNewStatus( const NCPkgStatus & oldStatus,
			    const NCPkgStatus & newStatus,
			    const  PMObjectPtr & objPtr  );   
};


//------------------------------------------------------------
// Class for strategies to handle status of available packages
//------------------------------------------------------------
class AvailableStatStrategy : public ObjectStatStrategy
{
public:
    
    AvailableStatStrategy( );
    
    virtual ~AvailableStatStrategy() {}
    
    virtual PMSelectable::UI_Status getStatus ( PMObjectPtr  objPtr );

    virtual bool setPackageStatus( PMObjectPtr objPtr, PMSelectable::UI_Status newstatus );
};

#endif

