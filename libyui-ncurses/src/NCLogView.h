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

   File:       NCLogView.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCLogView_h
#define NCLogView_h

#include <iosfwd>

#include "YLogView.h"
#include "NCPadWidget.h"

class NCLogView;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCLogView
//
//	DESCRIPTION :
//
class NCLogView : public YLogView, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCLogView & OBJ );

  NCLogView & operator=( const NCLogView & );
  NCLogView            ( const NCLogView & );

  private:

    NCtext text;

  protected:

    virtual const char * location() const { return "NCLogView"; }

    virtual void wRedraw();
    virtual void wRecoded();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

  public:

    NCLogView( NCWidget * parent, const YWidgetOpt & opt,
	       const YCPString & label,
	       int visibleLines,
	       int maxLines );
    virtual ~NCLogView();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setLogText( const YCPString & ntext);

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCLogView_h
