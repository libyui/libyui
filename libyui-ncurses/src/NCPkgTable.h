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

   File:       NCPkgTable.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgTable_h
#define NCPkgTable_h

#include <iosfwd>

#include "NCPadWidget.h"
#include "NCTablePad.h"
#include "NCTable.h"
#include "PkgNames.h"

#include <map>          
#include <string>
#include <utility>      // for STL pair

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMSelectable.h>

#include "ObjectStatStrategy.h"


class PackageSelector;

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

/**
 * This class is used for the first column of the package table
 * which contains the status information of the package (installed,
 * not installed, to be deleted and so on).
 *
 **/
class NCPkgTableTag : public NCTableCol {

  private:

    NCPkgStatus status;
    PMObjectPtr dataPointer;
    
    // returns the corresponding string value to given package status
    string statusToStr( NCPkgStatus stat ) const;
    
  public:

    NCPkgTableTag( PMObjectPtr pkgPtr, NCPkgStatus stat = PkgNoInstall );

    virtual ~NCPkgTableTag() {}

    virtual void SetLabel( const NCstring & ) { /*NOOP*/; }

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const; 

    void setStatus( NCPkgStatus & stat ) 	{ status = stat; }
    NCPkgStatus getStatus() const              	{ return status; }
    PMObjectPtr getDataPointer() const		{ return dataPointer; }
};

/**
 * The package table class. Provides methods to fill the table,
 * set the status info and so on.
 * Has a connection to the PackageSelector which is used to do
 * changes which affect other widgets.
 *
 **/
class NCPkgTable : public NCTable {

private:

    PackageSelector * packager;		// connection to the PackageSelector,

    ObjectStatStrategy * statusStrategy; 	// particular methods to get the status
    
    // returns the corresponding package status to the given key
    NCPkgStatus keyToStatus( const int & key );

    // returns the first column of line with 'index' (the tag)
    NCPkgTableTag * getTag ( const int & index );

protected:

    /**
     * Sets the new status in first column of the package table
     * and informs the package manager about the status change.
     * @param index The index in package list
     * @param newstat The new status
     * @return bool
     */
    bool changeStatus( int index, NCPkgStatus newstat);

public:

   /**
    * Constructor
    */
    NCPkgTable( NCWidget * parent, YWidgetOpt & opt );

    virtual ~NCPkgTable();

    friend std::ostream & operator<<( std::ostream & str, NCPkgStatus obj );

   /**
    * This method is called to add a line to the package list.
    * @param status The package status (first column of the table)
    * @param elements A vector<string> containing the package data
    * @param index The index in package list
    * @param objPtr The pointer to the packagemanager object
    * @eturn void
    */
    virtual void addLine( PMSelectable::UI_Status status,
			  vector<string> elements,
			  int index,
			  PMObjectPtr objPtr );

   /**
    * Clears the package list
    */
    virtual void itemsCleared();

   /**
    * Changes the contents of a certain cell in table
    * @param index The table line
    * @param column The column
    * @param newtext The new text
    * @eturn void
    */
    virtual void cellChanged( int index, int colnum, const YCPString & newtext );

   /**
    * Returns the contents of a certain cell in table
    * @param index The table line
    * @param column The column
    * @eturn NClabel
    */
    NClabel getCellContents( int index, int colnum );

    /**
     * Handles the events concerning the package table (e.g. scroll the list,
     * change the package status, ...)
     * @param key The key which is pressed
     * @return NCursesEvent
     */
    virtual NCursesEvent wHandleInput( int key );

   /**
     * Sets the member variable PackageSelector *packager
     * @param pkg The PackageSelector pointer
     * @return void
     */
    void setPackager( PackageSelector * pkg ) { packager = pkg; }

    /**
     * Toggles the package status (e.g. from installed to delete)
     * @return bool
     */
    bool toggleStatus( PMPackagePtr pkgPtr );

    /**
     * Sets the new package status (if the status change is possible) and
     * informs the package manager.
     * @param pkgStat The new package status
     * @return bool
     */ 
    bool setNewStatus( const NCPkgStatus & pkgStat );


   /**
     * Set the status information if status has changed 
     * @return bool
     */  
    bool updateTable();

   /**
     * Returns the UI status to given internal package status.
     * @param stat The NCPkgStatus
     * @return UI_Status
     */ 
    PMSelectable::UI_Status statusToUIStat( NCPkgStatus stat );

   /**
     * Returns the internal used status to given UI status.
     * @param stat The UI status
     * @return NCPkgStatus
     */ 
    NCPkgStatus statusToPkgStat( PMSelectable::UI_Status stat );
    
    /**
     * Gets the package status of a certain package.
     * @param index The index in package table (the line)
     * @return NCPkgStatus
     */ 
    NCPkgStatus getStatus( int index );

    /**
     * Sets the status strategy. which means call particular methods to set/get the
     * status for different PMObject s (PNYouPatch, PMPackage or available PMPackage s)
     */
    void setStatusStrategy( ObjectStatStrategy * strategy ) {
	delete statusStrategy;
	statusStrategy = strategy;
    }
	
   /**
     * Gets the data pointer of a certain package.
     * @param index The index in package table (the line)
     * @return PMObjectPtr
     */ 
    PMObjectPtr getDataPointer( int index );

   /**
     * Returns the number of lines in the table (the table size)
     * @return unsigned int
     */ 
    unsigned int getNumLines( ) { return pad->Lines(); }
};

///////////////////////////////////////////////////////////////////

#endif // NCPkgTable_h
