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

  File:	      YQRadioButtonGroup.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qradiobutton.h>
#include <qbuttongroup.h>
#define y2log_component "qt-ui"
#include <ycp/YCPBoolean.h>
#include <ycp/y2log.h>

#include "YQRadioButtonGroup.h"
#include "YQRadioButton.h"


YQRadioButtonGroup::YQRadioButtonGroup( QWidget * parent, YWidgetOpt & opt )
    : QWidget( parent )
    , YRadioButtonGroup( opt )
{
    setWidgetRep( this );
    recursionCounter = 0;
}


YQRadioButtonGroup::~YQRadioButtonGroup()
{
    // NOP
}


void YQRadioButtonGroup::addRadioButton( YRadioButton * button )
{
    YRadioButtonGroup::addRadioButton( button );

    if ( button->getValue()->value() )	// if this new button is active
    {
	uncheckOtherButtons( button );	// make it the only active
    }

    QRadioButton * radio_button = ( ( YQRadioButton * ) button )->getQtButton();

    connect ( radio_button,	SIGNAL ( toggled           ( bool ) ),
	      this, 		SLOT   ( radioButtonClicked( bool ) ) );
}


void YQRadioButtonGroup::removeRadioButton( YRadioButton * button )
{
    YRadioButtonGroup::removeRadioButton( button );
}


void YQRadioButtonGroup::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}

void YQRadioButtonGroup::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    YRadioButtonGroup::setSize( newWidth, newHeight );
}


void YQRadioButtonGroup::radioButtonClicked( bool newState )
{
    // Prevent infinite recursion: YQRadioButton::setValue() might cause Qt
    // signals that would cause recursion to this place.

    if ( recursionCounter > 0 )
	return;

    recursionCounter++;

    QRadioButton * sender_button = ( QRadioButton * ) sender();

    // Implement radio box behaviour: Uncheck all other radio buttons

    for ( unsigned i=0; i < buttonlist.size(); i++ )
    {
	YQRadioButton * current_button = ( YQRadioButton * ) buttonlist[i];

	if ( current_button->getQtButton() == sender_button )
	{
	    // If this button has been clicked, it is to be the RadioBox's
	    // active button - regardless of newState. This is to avoid
	    // RadioBoxes where no single button is active; otherwise the
	    // second click would deactivate the only active button.

	    current_button->setValue( YCPBoolean( true ) );
	}
	else
	{
	    current_button->setValue( YCPBoolean( false ) );
	}
    }

    recursionCounter--;
}


#include "YQRadioButtonGroup.moc.cc"
