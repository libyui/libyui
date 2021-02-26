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

   File:       NCRadioButton.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCRadioButton.h"
#include "NCRadioButtonGroup.h"


NCRadioButton::NCRadioButton( YWidget * parent,
			      const string & nlabel,
			      bool check )
	: YRadioButton( parent, nlabel )
	, NCWidget( parent )
	, checked( false )
{
    yuiDebug() << endl;
    setLabel( nlabel );
    hotlabel = &label;
    setValue( check );
}


NCRadioButton::~NCRadioButton()
{
    yuiDebug() << endl;
}


int NCRadioButton::preferredWidth()
{
    return wGetDefsze().W;
}


int NCRadioButton::preferredHeight()
{
    return wGetDefsze().H;
}


void NCRadioButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YRadioButton::setEnabled( do_bv );
}


void NCRadioButton::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCRadioButton::setLabel( const string & nlabel )
{
    label  = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 4 );
    YRadioButton::setLabel( nlabel );
    Redraw();
}


void NCRadioButton::setValue( bool newval )
{
    if ( newval != checked )
    {
	checked = newval;

	if ( checked && buttonGroup() )
	{
	    buttonGroup()->uncheckOtherButtons( this );
	}

	Redraw();
    }
}


void NCRadioButton::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgdset( style.plain );

    win->printw( 0, 0, "( ) " );

    label.drawAt( *win, style, wpos( 0, 4 ) );

    win->bkgdset( style.data );

    win->printw( 0, 1, "%c", ( checked ? 'x' : ' ' ) );
}


NCursesEvent NCRadioButton::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool oldChecked = checked;
    NCRadioButtonGroup * group;

    switch ( key )
    {
	case KEY_HOTKEY:
	case KEY_SPACE:
	case KEY_RETURN:
	    setValue( true );

	    if ( notify() && oldChecked != checked )
		ret = NCursesEvent::ValueChanged;

	    break;

	case KEY_UP:
	    group = dynamic_cast<NCRadioButtonGroup *>( buttonGroup() );

	    if ( group )
		group->focusPrevButton();

	    break;

	case KEY_DOWN:
	    group = dynamic_cast<NCRadioButtonGroup *>( buttonGroup() );

	    if ( group )
		group->focusNextButton();

	    break;
    }

    return ret;
}

