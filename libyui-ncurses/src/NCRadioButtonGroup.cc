/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCRadioButtonGroup.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCRadioButtonGroup.h"


NCRadioButtonGroup::NCRadioButtonGroup( YWidget * parent )
	: YRadioButtonGroup( parent )
	, NCWidget( parent )
	, focusId( 1 )
{
    yuiDebug() << std::endl;
    wstate = NC::WSdumb;
}


NCRadioButtonGroup::~NCRadioButtonGroup()
{
    yuiDebug() << std::endl;
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


void NCRadioButtonGroup::focusNextButton()
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
