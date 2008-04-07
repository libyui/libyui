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

   File:       NCPushButton.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCPushButton_h
#define NCPushButton_h

#include <iosfwd>

#include "YPushButton.h"
#include "NCWidget.h"

class NCPushButton;


class NCPushButton : public YPushButton, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPushButton & OBJ );

  NCPushButton & operator=( const NCPushButton & );
  NCPushButton            ( const NCPushButton & );

  private:

    NClabel label;

  protected:

    virtual const char * location() const { return "NCPushButton"; }

    virtual void wRedraw();

  public:

    NCPushButton( YWidget * parent, const string & label );
    virtual ~NCPushButton();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setLabel( const string & nlabel );
 
    virtual void setEnabled( bool do_bv );
    
    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

};


#endif // NCPushButton_h
