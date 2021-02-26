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

   File:       NCRadioButtonGroup.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCRadioButtonGroup.h"


NCRadioButtonGroup::NCRadioButtonGroup( YWidget * parent )
	: YRadioButtonGroup( parent )
	, NCWidget( parent )
	, focusId( 1 )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCRadioButtonGroup::~NCRadioButtonGroup()
{
    yuiDebug() << endl;
}


void NCRadioButtonGroup::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YRadioButtonGroup::setSize( newwidth, newheight );
}


void NCRadioButtonGroup::addRadioButton( YRadioButton *button )
{
    YRadioButtonGroup::addRadioButton( button );
}


void NCRadioButtonGroup::removeRadioButton( YRadioButton *button )
{
    YRadioButtonGroup::removeRadioButton( button );
}


void NCRadioButtonGroup::focusNextButton( )
{
    int n = 0;
    
    if ( focusId < radioButtonsCount() )
	focusId++;
    else if ( focusId == radioButtonsCount() )
	focusId = 0;

    for ( YRadioButtonListConstIterator it = radioButtonsBegin();
	  it != radioButtonsEnd();
	  ++it )
    {
	if ( ++n == focusId )
	{
	    NCRadioButton * radioButton = dynamic_cast<NCRadioButton *>( *it );

	    if ( radioButton )
		radioButton->setKeyboardFocus();
	}
    }
}


void NCRadioButtonGroup::focusPrevButton()
{
    int n = 0;

    if ( focusId > 0 )
	focusId--;
    else if ( focusId == 0 )
	focusId = radioButtonsCount() - 1;

    for ( YRadioButtonListConstIterator it = radioButtonsBegin();
	  it != radioButtonsEnd();
	  ++it )
    {
	n++;

	if ( n == focusId )
	{
	    NCRadioButton * radioButton = dynamic_cast<NCRadioButton *>( *it );

	    if ( radioButton )
		radioButton->setKeyboardFocus();
	}
    }
}


void NCRadioButtonGroup::setEnabled( bool do_bv )
{
    for ( YRadioButtonListConstIterator it = radioButtonsBegin();
	  it != radioButtonsEnd();
	  ++it )
    {
	NCRadioButton * radioButton = dynamic_cast<NCRadioButton *>( *it );

	if ( radioButton )
	    radioButton->setEnabled( do_bv );
    }

    YRadioButtonGroup::setEnabled( do_bv );
}
