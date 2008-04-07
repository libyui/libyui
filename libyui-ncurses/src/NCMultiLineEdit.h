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

   File:       NCMultiLineEdit.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCMultiLineEdit_h
#define NCMultiLineEdit_h

#include <iosfwd>

#include "YMultiLineEdit.h"
#include "NCPadWidget.h"
#include "NCTextPad.h"

class NCMultiLineEdit;


class NCMultiLineEdit : public YMultiLineEdit, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiLineEdit & OBJ );

  NCMultiLineEdit & operator=( const NCMultiLineEdit & );
  NCMultiLineEdit            ( const NCMultiLineEdit & );

  private:

    NCstring ctext;

  protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTextPad * myPad () const
        { return dynamic_cast<NCTextPad*> ( NCPadWidget::myPad () ); }

  protected:

    virtual const char * location() const { return "NCMultiLineEdit"; }

    virtual void wRedraw();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

  public:

    NCMultiLineEdit( YWidget * parent, const string & label );

    virtual ~NCMultiLineEdit();

    virtual int preferredWidth();
    virtual int preferredHeight();
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );
    virtual void setValue( const string & ntext);

    virtual string value();
    
    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    // sets the maximum number of characters of the NCTextPad
    void setInputMaxLength( int numberOfChars);

};


#endif // NCMultiLineEdit_h
