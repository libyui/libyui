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

   File:       NCPackageSelectorStart.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPackageSelectorStart_h
#define NCPackageSelectorStart_h

#include <iosfwd>

#include "YNCursesUI.h"
#include "YPackageSelector.h"
#include "YSplit.h"
#include "NCSplit.h"
#include "NCPackageSelector.h"
#include "NCPopupTable.h"



class NCPkgTable;


/**
 * @short the package selector widget
 */
class NCPackageSelectorStart : public NCSplit
{

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPackageSelectorStart & OBJ );

  NCPackageSelectorStart & operator=( const NCPackageSelectorStart & );
  NCPackageSelectorStart            ( const NCPackageSelectorStart & );

  private:
   
    YContainerWidget * widgetRoot; 	// root of the widget tree of the package selection dialog
    
    NCPkgTable * pkgList;		// the package table widget
    
    NCPackageSelector *packager;	// packager object contains the data and handles events

    bool youMode;
    bool updateMode;
    
  protected:

    virtual const char * location() const {
      return dimension() == YD_HORIZ ? "NC(H)PackageSelectorStart" : "NC(V)PackageSelectorStart" ;
    }
    
  public:

    /**
     * Constructor
     * creates the widget tree of the package selector
     */
    NCPackageSelectorStart( NCWidget * parent,
			    const YWidgetOpt & opt, YUIDimension dimension );

    /**
     * Destructor
     */
    virtual ~NCPackageSelectorStart();

    virtual long nicesize( YUIDimension dim ) { return NCSplit::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );

    /**
     * Fills the package table with packages belonging to the  
     * default filter (the filter which is selected when entering the
     * package selection).
     **/
    void showDefaultList();

    /**
     * Pass the event to the handleEvent method of the member variable
     * NCPackageSelector packager.
     * @param event The NCursesEvent
     * @return bool
     */
    bool handleEvent( const NCursesEvent&   event );

    /**
     * Returns the root of the widget tree.
     */
    YContainerWidget * root( ) const { return widgetRoot; };

        
   /**
    * Helper function; reads a file containing a YCP term
    * @param layoutFilename The filename
    * @return YCPTerm
    */
    YCPTerm readLayoutFile( const char * layoutFilename );

};

///////////////////////////////////////////////////////////////////

#endif // NCPackageSelectorStart_h
