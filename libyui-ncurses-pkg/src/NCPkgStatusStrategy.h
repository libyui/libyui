/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgStatusStrategy.h

   Author:     Gabriele Strattner <gs@suse.de>


/-*/
#ifndef NCPkgStatusStrategy_h
#define NCPkgStatusStrategy_h

#include "NCZypp.h"
#include <zypp/ui/Selectable.h>


//------------------------------------------------------------
// Abstract base class for strategies to get status for packages or patches
//------------------------------------------------------------
class NCPkgStatusStrategy
{

public:

    NCPkgStatusStrategy( );

    virtual ~NCPkgStatusStrategy() = 0;

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
     /**
    * Do a "small" solver run for all "resolvable collections", i.e., for
    * selections, patterns, languages, patches.
    **/
    void	solveResolvableCollections();


};

//------------------------------------------------------------
// Class for strategies to handle status of packages
//------------------------------------------------------------
class PackageStatStrategy : public NCPkgStatusStrategy
{
public:

    PackageStatStrategy( );

    virtual ~PackageStatStrategy() {}

};

//------------------------------------------------------------
// Class for strategies to handle dependencies
//------------------------------------------------------------
class DependencyStatStrategy : public NCPkgStatusStrategy
{
public:

    DependencyStatStrategy( );

    virtual ~DependencyStatStrategy() {}

};

//------------------------------------------------------------
// Class for strategies of update
//------------------------------------------------------------
class UpdateStatStrategy : public NCPkgStatusStrategy
{
public:

    UpdateStatStrategy( );

    virtual ~UpdateStatStrategy() {}

};

//------------------------------------------------------------
// Class for strategies of selections
//------------------------------------------------------------
class SelectionStatStrategy : public NCPkgStatusStrategy
{
public:

    SelectionStatStrategy( );

    virtual ~SelectionStatStrategy() {}

    /**
     * Sets the status of the selection
     * @param newStatus The new selection status
     * @param slbPtr  The selectable pointer
     * @qparam objPtr The object (selection) pointer
     * @return bool
     */
    virtual bool setObjectStatus( ZyppStatus newstatus,
				  ZyppSel slbPtr,
				  ZyppObj objPtr );

};

//------------------------------------------------------------
// Class for strategies to handle status of packages belonging to a patch
//------------------------------------------------------------
class PatchPkgStatStrategy : public NCPkgStatusStrategy
{
public:

    PatchPkgStatStrategy( );

    virtual ~PatchPkgStatStrategy() {}

    /**
     * It is not possible to std::set the package status for packages belonging to a patch,
     * i.e. returns always false.
     */
    virtual bool setObjectStatus( ZyppStatus newstatus,
				  ZyppSel slbPtr,
				  ZyppObj objPtr );

};



//------------------------------------------------------------
// Class for strategies to handle status of patches
//------------------------------------------------------------
class PatchStatStrategy : public NCPkgStatusStrategy
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
class AvailableStatStrategy : public NCPkgStatusStrategy
{
public:

    AvailableStatStrategy( );

    virtual ~AvailableStatStrategy() {}

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

//----------------------------------------------------------------
// Class for strategies to handle status of multi version packages
//----------------------------------------------------------------
class MultiVersionStatStrategy : public NCPkgStatusStrategy
{
public:

    MultiVersionStatStrategy( );

    virtual ~MultiVersionStatStrategy() {}

    /**
     * Gets the status information from the package manager.
     * @param slbPtr The package whose status to calculate.
     * @return UI_Status The new status of the given package
     *
     **/
    virtual ZyppStatus getPackageStatus ( ZyppSel slbPtr, ZyppObj objPtr );

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


    /**
     * Check if any package version is marked for installation where its
     * 'multiversion' flag is set to 'multiversion'.
     * @param slbPtr The selectable whose status has to be calculated.
     * @param multiversion Check for multiversion or non-multiversion packages.
     * @return bool
     *
     **/
    virtual bool anyMultiVersionToInstall( ZyppSel slbPtr, bool multiversion ) const;

    /**
     * Ask user if he really wants to install incompatible package versions.
     * @param multiversion Selected package is a multiversion package.
     * @return bool Return 'true' if he hits [Continue], 'false' if [Cancel].
     *
     **/
    virtual bool mixedMultiVersionPopup( bool multiversion ) const;

};


#endif
