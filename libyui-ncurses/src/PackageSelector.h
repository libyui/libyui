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

   File:       PackageSelector.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PackageSelector_h
#define PackageSelector_h

#include <iosfwd>

#include "Y2NCursesUI.h"
#include "PkgNames.h"

#include <map>          
#include <string>
#include <utility>      // for STL pair

#include <y2util/YRpmGroupsTree.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>

#include "NCPopupTree.h"

class NCPkgTable;
class NCPopupSelection;
class PMSelectionPtr;
class PMPackagePtr;
class PMYouPatchPtr;
class LangCode;
class NCPopupDeps;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PackageSelector
//
//	DESCRIPTION : holds the data and handles events
//
class PackageSelector
{

  friend std::ostream & operator<<( std::ostream & STREAM, const PackageSelector & OBJ );

  PackageSelector & operator=( const PackageSelector & );
  PackageSelector            ( const PackageSelector & );

  private:

    // typedef for the pointer to handler member function
    typedef bool (PackageSelector::* tHandlerFctPtr) ( const NCursesEvent& event );

    // typedef for the internal map: key=nameId, value=handler-fct-ptr
    typedef std::map<std::string, tHandlerFctPtr> tHandlerMap;

    tHandlerMap eventHandlerMap;    	// event handler map
    
    Y2NCursesUI * y2ui;			// the UI

    YCPValue visibleInfo;		// visible package info (description, file list, ...)

    NCPopupTree * filterPopup;		// the rpm group tags popup

    NCPopupDeps * depsPopup;		// the dependeny popup
    
    NCPopupSelection * selectionPopup; 	// the selections popup

    bool youMode;			// YOU
    bool updateMode;			// Update
    
    // internal helper functions (format list of string) 
    string createRelLine( list<PkgRelation> info );
    string createDescrText( list<string> info );
    string createText( list<string> info, bool oneline );

    // add a line to the package list
    bool createListEntry ( NCPkgTable *table, PMPackagePtr pkgPtr, unsigned int index );
    bool createPatchEntry ( NCPkgTable *table, PMYouPatchPtr patchPtr, unsigned int index );
    
    // internal use (copies tree items got from YPkgRpmGroupTagsFilterView)
    void cloneTree( YStringTreeItem * parentOrig, YTreeItem * parentClone );

    // the package (patch) list 
    NCPkgTable * getPackageList();
    
  protected:
 

  public:
    
    /**
     * The package selector class handles the events and holds the
     * data needed for the package selection.
     * @param ui The NCurses UI
     */
    PackageSelector( Y2NCursesUI * ui, YWidgetOpt & opt);

    /**
     * Destructor
     */ 
    virtual ~PackageSelector();

   /**
    * Fills the package table
    * @param label The selected RPM group (the label)
    * @param group The rpm group
    * @return bool
    */
    bool fillPackageList( const YCPString & label, YStringTreeItem * group );

  /**
    * Fills the package table with available YOU patches
    * @return bool
    */
    bool fillPatchList( );
    
   /**
    * Fills the list of available packages
    * @param table  The table widget
    * @param pkgPtr Show all available versions of this package 
    * @return bool
    */
    bool fillAvailableList( NCPkgTable *table, PMObjectPtr pkgPtr );    

   /**
    * Fills the package table with packages matching the search expression
    * @param expr The search expressionqc
    * @return bool
    */
    bool fillSearchList( const YCPString & expr );
    
   /**
    * Fills the header of the package table
    * @param table The table widget
    * @return void
    */
    void fillHeader( NCPkgTable *table );

   /**
    * Gets default RPM group (the first group)
    * @return YStringTreeItem *
    */
    YStringTreeItem * getDefaultRpmGroup() { return filterPopup->getDefaultGroup(); }
    
    /**
     * Handle the given event. For the given event (the widget-id
     * is contained in the event) the corresponding handler is executed.
     * @param event The NCurses event
     * @return bool
     */
    bool handleEvent( const NCursesEvent& event );

   /**
    * Handler function for "Search button pressed"
    * @param event The Ncurses event
    * @return bool
    */
    bool SearchHandler ( const NCursesEvent& event );

    /**
    * Handler function for menu selection "Etc./Check dependencies"
    * @param event The Ncurses event
    * @return bool
    */
    bool DependencyHandler( const NCursesEvent&  event );
    
    /**
     * Handler function for the "Information" menu
     * @param event The Ncurses event
     * @return bool
     */
    bool InformationHandler ( const NCursesEvent& event );

    /**
    * Handler function for "OK button pressed"
    * @param event The Ncurses event
    * @return bool 
    */
    bool OkButtonHandler ( const NCursesEvent& event );

   /**
    * Handler function for "Cancel button pressed"
    * @param event The Ncurses event
    * @return bool
    */
    bool CancelHandler ( const NCursesEvent& event );

   /**
    * Handler function for disk space information (shows the required space)
    * @param event The Ncurses event
    * @return bool
    */
    bool DiskspaceHandler ( const NCursesEvent& event );

    /**
    * Handler function for "Diskspace button pressed"
    * @param event The Ncurses event
    * @return bool
    */
    bool DiskinfoHandler ( const NCursesEvent& event );
    
    /**
     * Handler function for the package list which handles events not directly
     * related to the list. Most events are handled in NCPkgTable itself.
     * @param event The Ncurses event
     * @return bool
     */ 
    bool PackageListHandler( const NCursesEvent&  event );

    /**
     * Handler function for the "Filter" menu
     * @param event The Ncurses event
     * @return bool     
     */
    bool FilterHandler( const NCursesEvent&  event );

    /**
     * Handler function for the "Action" menu (changes the package status)
     * @param event The Ncurses event
     * @return bool     
     */
    bool StatusHandler( const NCursesEvent&  event );

   /**
    * Handler function for the "Help" menu
    * @param event The Ncurses event
    * @return bool
    */
    bool HelpHandler( const NCursesEvent&  event );
    
   /**
    * Gets the required package info from package manager and shows it
    * (	called from NCPkgTable )
    * @param pkgPtr the data pointer
    * @return bool
    */
    bool showPackageInformation ( PMObjectPtr pkgPtr );

  /**
    * Shows the dependency of this package 
    * @param pkgPtr the data pointer
    * @return bool
    */
    bool showConcretelyDependency ( PMObjectPtr pkgPtr );

   /**
    * Gets the required patch info from you patch manager and shows it
    * @param pkgPtr the data pointer
    * @return bool
    */
    bool showPatchInformation ( PMObjectPtr pkgPtr );
    
   /**
    * Sets the member variable to the currently visible information
    * @param info
    */
    void setVisibleInfo( const YCPValue & info );

   /**
    * Fills the package table
    * @param label The selected selection (the label)
    * @param selPtr The selection
    * @return bool
    */
    bool showSelPackages( const YCPString & label, PMSelectionPtr selPtr );

   /**
    * Updates the status in list of packages
    */
    void updatePackageList();
    
    /**
     * Check if 'pkg' matches 'selectedRpmGroup'.
     * Returns true if there is a match, false otherwise or if 'pkg' is 0.
     **/
    bool check( PMPackagePtr pkg, YStringTreeItem * rpmGroup, int index );

};

///////////////////////////////////////////////////////////////////

#endif // PackageSelector_h
