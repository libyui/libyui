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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPushButton_h
#define NCPushButton_h

#include <iosfwd>

#include "YPushButton.h"
#include "NCWidget.h"

class NCPushButton;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPushButton
//
//	DESCRIPTION :
//
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

    NCPushButton( NCWidget * parent, YWidgetOpt & opt,
		  YCPString label );
    virtual ~NCPushButton();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual NCursesEvent wHandleInput( int key );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCPushButton_h
