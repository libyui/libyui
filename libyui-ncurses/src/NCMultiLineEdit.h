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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCMultiLineEdit_h
#define NCMultiLineEdit_h

#include <iosfwd>

#include "YMultiLineEdit.h"
#include "NCPadWidget.h"
#include "NCTextPad.h"

class NCMultiLineEdit;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCMultiLineEdit
//
//	DESCRIPTION :
//
class NCMultiLineEdit : public YMultiLineEdit, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiLineEdit & OBJ );

  NCMultiLineEdit & operator=( const NCMultiLineEdit & );
  NCMultiLineEdit            ( const NCMultiLineEdit & );

  private:

    NCstring ctext;

  protected:

    NCTextPad *& pad;

  protected:

    virtual const char * location() const { return "NCMultiLineEdit"; }

    virtual void wRedraw();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

  public:

    NCMultiLineEdit( NCWidget * parent, const YWidgetOpt & opt,
		     const YCPString & label,
		     const YCPString & initialText );
    virtual ~NCMultiLineEdit();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setText( const YCPString & ntext);

    virtual YCPString text();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    // sets the maximum number of characters of the NCTextPad
    void setInputMaxLength( const YCPInteger & numberOfChars);

};

///////////////////////////////////////////////////////////////////

#endif // NCMultiLineEdit_h
