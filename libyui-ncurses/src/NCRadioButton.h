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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCRadioButton_h
#define NCRadioButton_h

#include <iosfwd>

#include "YRadioButton.h"
#include "NCWidget.h"

class NCRadioButton;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCRadioButton
//
//	DESCRIPTION :
//
class NCRadioButton : public YRadioButton, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCRadioButton & OBJ );

  NCRadioButton & operator=( const NCRadioButton & );
  NCRadioButton            ( const NCRadioButton & );

  private:

    bool     checked;
    NClabel  label;

  protected:

    virtual const char * location() const { return "NCRadioButton"; }

    virtual void setValue( const bool & checked );

    virtual void wRedraw();

  public:

    NCRadioButton( NCWidget * parent, const YWidgetOpt & opt,
		   YRadioButtonGroup * rbg,
		   const YCPString & label,
		   bool checked );
    virtual ~NCRadioButton();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setValue( const YCPBoolean & newval );

    virtual YCPBoolean getValue();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCRadioButton_h
