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

   File:       NCMenuButton.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCMenuButton_h
#define NCMenuButton_h

#include <iosfwd>

#include "YMenuButton.h"
#include "NCWidget.h"

class NCMenuButton;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCMenuButton
//
//	DESCRIPTION :
//
class NCMenuButton : public YMenuButton, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCMenuButton & OBJ );

  NCMenuButton & operator=( const NCMenuButton & );
  NCMenuButton            ( const NCMenuButton & );

  private:

    NClabel label;

  protected:

    virtual const char * location() const { return "NCMenuButton"; }

    virtual void wRedraw();

    virtual void createMenu();
    NCursesEvent postMenu();

  public:

    NCMenuButton( NCWidget * parent, const YWidgetOpt & opt,
		  YCPString label );
    virtual ~NCMenuButton();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCMenuButton_h
