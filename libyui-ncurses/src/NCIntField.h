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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCIntField_h
#define NCIntField_h

#include <iosfwd>

#include "YIntField.h"
#include "NCWidget.h"

class NCIntField;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCIntField
//
//	DESCRIPTION :
//
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

    int enterPopup( char first = '\0' );

  public:

    NCIntField( NCWidget * parent, YWidgetOpt & opt,
		const YCPString & label,
		int minValue, int maxValue,
		int initialValue );
    virtual ~NCIntField();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setValue( int newValue );

    virtual NCursesEvent wHandleInput( int key );

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCIntField_h
