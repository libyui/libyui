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

   File:       NCComboBox.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCComboBox_h
#define NCComboBox_h

#include <iosfwd>

#include <list>
using namespace std;

#include "YComboBox.h"
#include "NCWidget.h"

class NCComboBox;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCComboBox
//
//	DESCRIPTION :
//
class NCComboBox : public YComboBox, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCComboBox & OBJ );

  NCComboBox & operator=( const NCComboBox & );
  NCComboBox            ( const NCComboBox & );

  private:

    bool     mayedit;
    NClabel  label;
    NCstring text;
    wstring   buffer;
    bool     modified;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    string::size_type fldstart;
    string::size_type fldlength;
    string::size_type curpos;

    NCstring validChars;

    list<YCPString> deflist;
    int             index;

    void setDefsze();
    void tUpdate();

  protected:

    virtual const char * location() const { return "NCComboBox"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();
    virtual void wRecoded();

    int listPopup();

    bool validKey( wint_t key ) const;

  public:

    NCComboBox( NCWidget * parent, YWidgetOpt & opt,
		const YCPString & label );
    virtual ~NCComboBox();

    virtual void itemAdded( const YCPString & string,
			    int index,
			    bool selected );

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setValue( const YCPString & ntext );
    virtual YCPString getValue() const;

    virtual void setValidChars( const YCPString & validchars );

    virtual void setCurrentItem( int index );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    unsigned int getListSize( ) { return deflist.size(); }
};

///////////////////////////////////////////////////////////////////

#endif // NCComboBox_h
