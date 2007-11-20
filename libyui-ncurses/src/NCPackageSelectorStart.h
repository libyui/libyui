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
#include "YLayoutBox.h"
#include "NCLayoutBox.h"
#include "NCPackageSelector.h"
#include "NCPopupTable.h"


class NCPkgTable;
class NCPackageSelector;
class NCPushButton;

/**
 * @short the package selector widget
 */
class NCPackageSelectorStart : public NCLayoutBox
{

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPackageSelectorStart & OBJ );

  NCPackageSelectorStart & operator=( const NCPackageSelectorStart & );
  NCPackageSelectorStart            ( const NCPackageSelectorStart & );

  private:
   
    YWidget * widgetRoot; 	// root of the widget tree of the package selection dialog
    
    NCPackageSelector *packager;	// packager object contains the data and handles events

    bool youMode;
    bool updateMode;
    
  protected:

    virtual const char * location() const {
      return primary() == YD_HORIZ ? "NC(H)PackageSelectorStart" : "NC(V)PackageSelectorStart" ;
    }
    
  public:

    /**
     * Constructor
     * creates the widget tree of the package selector
     */
    NCPackageSelectorStart( YWidget * 	parent,
			    long 	modeFlags,
			    YUIDimension dimension );

    /**
     * Destructor
     */
    virtual ~NCPackageSelectorStart();

    virtual int preferredWidth() { return NCLayoutBox::preferredWidth(); }
    virtual int preferredHeight() { return NCLayoutBox::preferredHeight(); }
 
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

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
    YWidget * root( ) const { return widgetRoot; };

        
};

///////////////////////////////////////////////////////////////////

#endif // NCPackageSelectorStart_h
