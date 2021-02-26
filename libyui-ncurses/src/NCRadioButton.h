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

   File:       NCRadioButton.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCRadioButton_h
#define NCRadioButton_h

#include <iosfwd>

#include "YRadioButton.h"
#include "NCWidget.h"

class NCRadioButton;


class NCRadioButton : public YRadioButton, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCRadioButton & OBJ );

  NCRadioButton & operator=( const NCRadioButton & );
  NCRadioButton            ( const NCRadioButton & );

  private:

    bool     checked;
    NClabel  label;

  protected:

    virtual const char * location() const { return "NCRadioButton"; }

    // virtual void setValue( const bool & checked );

    virtual void wRedraw();

  public:

    NCRadioButton( YWidget * parent,
		   const string & label,
		   bool checked );
    
    virtual ~NCRadioButton();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setValue ( bool newval );
    virtual bool value() { return checked; }

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};


#endif // NCRadioButton_h
