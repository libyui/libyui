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



#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>


class NCPkgTable;
class NCPopupTree;
class NCPopupSelection;
class PMSelectionPtr;
class PMPackagePtr;

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

    NCPkgTable * packageList;		// the package list
    
    YCPValue visibleInfo;		// visible package info (description, file list, ...)

    NCPopupTree * filterPopup;		// the rpm group tags popup

    NCPopupSelection * selectionPopup; 	// the selections popup

    vector<PMSelectionPtr> selections; 	// the available selections
    
    // internal helper functions (format list of string) 
    string createRelLine( list<PkgRelation> info );
    string createDescrText( list<string> info );
    string createText( list<string> info, bool oneline );
    // add a line to the package list
    bool   createListEntry ( NCPkgTable *table, PMPackagePtr pkgPtr, unsigned int index );

    bool selOk;
    
  protected:
 
  /**
   * Get selections from the instsource manager
   */
  bool getSelections( );
    
  public:
    
    /**
     * The package selector class handles the events and holds the
     * data needed for the package selection.
     * @param ui The NCurses UI
     */
    PackageSelector( Y2NCursesUI * ui );

    /**
     * Destructor
     */ 
    virtual ~PackageSelector();

    /**
    * Fills the package table
    * @param table  The table widget
    * @param label The selected RPM group (the label)
    * @param filter The complete path od the RPM group (the filter criteria)
    * @return bool
    */
    bool fillPackageList( NCPkgTable *table, const YCPString & label, string filter );

   /**
    * Fills the package table
    * @param table  The table widget
    * @param label The selected selection (the label)
    * @param index Internal index
    * @return bool
    */
    bool showSelectionPackages( NCPkgTable *table, const YCPString & detail, int index );
    
   /**
    * Fills the list of available packages
    * @param table  The table widget
    * @param pkgPtr Show all available versions of this package 
    * @return bool
    */
    bool fillAvailableList( NCPkgTable *table, PMObjectPtr pkgPtr );    

   /**
    * Fills the header of the package table
    * @param table The table widget
    * @return void
    */
    void fillHeader( NCPkgTable *table );
    
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
    * Sets the member variable to the currently visible information
    * @param info
    */
    void setVisibleInfo( const YCPValue & info );

   /**
    * Sets the member variable package list
    * @param info
    */
    void setPackageList( NCPkgTable * pkgList ) { packageList = pkgList; }

    /**
     * Returns a list of the names of all available selections
     */
    std::vector<std::string> selectionNames( );

    /**
     * Returns the origin name (the id) of the selection
     */
    std::string selectionId( unsigned int index );

};

///////////////////////////////////////////////////////////////////

#endif // PackageSelector_h
