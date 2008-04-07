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

   File:       NCIntField.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCIntField_h
#define NCIntField_h

#include <iosfwd>

#include "YIntField.h"
#include "NCWidget.h"

class NCIntField;


class NCIntField : public YIntField, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCIntField & OBJ );

  NCIntField & operator=( const NCIntField & );
  NCIntField            ( const NCIntField & );

  private:

    static const unsigned taglen;

    NClabel label;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    int      cvalue;
    unsigned vlen;
    unsigned vstart;

    void setDefsze();
    void tUpdate();

  protected:

    virtual const char * location() const { return "NCIntField"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

    bool Increment( const bool bigstep = false );
    bool Decrement( const bool bigstep = false );

    int enterPopup( wchar_t first = L'\0' );

  public:

    NCIntField( YWidget * parent,
		const string & label,
		int minValue, int maxValue,
		int initialValue );
    virtual ~NCIntField();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setValueInternal( int newValue );

    virtual int value() { return cvalue; }
    virtual NCursesEvent wHandleInput( wint_t key );

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    virtual void setEnabled( bool do_bv );
};


#endif // NCIntField_h
