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

   File:       NCCheckBox.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCCheckBox_h
#define NCCheckBox_h

#include <iosfwd>

#include "YCheckBox.h"
#include "NCWidget.h"

class NCCheckBox;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCCheckBox
//
//	DESCRIPTION :
//
class NCCheckBox : public YCheckBox, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBox & OBJ );

  NCCheckBox & operator=( const NCCheckBox & );
  NCCheckBox            ( const NCCheckBox & );

  protected:

    enum State {
      S_DC  = 0,
      S_OFF = 1,
      S_ON  = 2
    };

  private:

    static unsigned char statetag[3];

    bool     tristate;
    State    checkstate;
    NClabel  label;

  protected:

    virtual const char * location() const { return "NCCheckBox"; }

    virtual void wRedraw();

  public:

    NCCheckBox( NCWidget * parent, const YWidgetOpt & opt,
		const YCPString & label,
		bool checked );
    virtual ~NCCheckBox();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setValue( const YCPValue & newval );
    virtual YCPValue getValue();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
	return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCCheckBox_h
