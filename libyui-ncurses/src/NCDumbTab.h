/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCDumbTab.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCDumbTab_h
#define NCDumbTab_h

#include <iosfwd>

#include "YDumbTab.h"
#include "NCWidget.h"


class NCDumbTab : public YDumbTab, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCDumbTab & OBJ );

    NCDumbTab & operator=( const NCDumbTab & );
    NCDumbTab( const NCDumbTab & );

    unsigned int currentIndex;
    wint_t hotKey;
    
protected:

    virtual const char * location() const { return "NCDumbTab"; }

    virtual void wRedraw();
    void redrawChild( YWidget *widget );
    
public:

    NCDumbTab( YWidget * parent );
    virtual ~NCDumbTab();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void addItem( YItem * item );
    virtual void selectItem( YItem * item, bool selected );
    
    virtual void setSize( int newWidth, int newHeight );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual void shortcutChanged();

    virtual bool HasHotkey( int key );
    
    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    NCursesEvent createMenuEvent( unsigned int index);

    void setCurrentTab( wint_t key );
};


#endif // NCDumbTab_h
