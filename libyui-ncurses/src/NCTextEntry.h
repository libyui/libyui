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

   File:       NCTextEntry.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCTextEntry_h
#define NCTextEntry_h

#include <iosfwd>

#include "YTextEntry.h"
#include "NCWidget.h"

class NCTextEntry;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTextEntry
//
//	DESCRIPTION :
//
class NCTextEntry : public YTextEntry, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCTextEntry & OBJ );

  NCTextEntry & operator=( const NCTextEntry & );
  NCTextEntry            ( const NCTextEntry & );

  public:

    enum FTYPE {
      PLAIN,
      NUMBER
    };

  private:
    
    bool     mayedit;
    bool     passwd;
    NClabel  label;
    wstring   buffer;

    NCursesWindow * lwin;
    NCursesWindow * twin;

    unsigned maxFldLength;
    unsigned maxInputLength;

    unsigned fldstart;
    unsigned fldlength;
    unsigned curpos;

    FTYPE    fldtype;
    NCstring validChars;

    bool     returnOnReturn_b;

    void setDefsze();
    void tUpdate();

    bool     bufferFull() const;
    unsigned maxCursor() const;

  protected:

    virtual const char * location() const { return "NCTextEntry"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

    bool validKey( wint_t key ) const;

  public:

    NCTextEntry( NCWidget * parent, const YWidgetOpt & opt,
		 const YCPString & label,
		 const YCPString & text,
		 unsigned maxInput = 0,
		 unsigned maxFld   = 0 );
    virtual ~NCTextEntry();

    void setFldtype( FTYPE t )           { fldtype = t; }
    void setReturnOnReturn( bool on_br ) { returnOnReturn_b = on_br; }

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setText( const YCPString & ntext );
    virtual YCPString getText();

    virtual void setValidChars( const YCPString & validchars );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCTextEntry_h
