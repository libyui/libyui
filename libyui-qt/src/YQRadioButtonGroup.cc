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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qradiobutton.h>
#include <qbuttongroup.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQRadioButtonGroup.h"
#include "YQRadioButton.h"


YQRadioButtonGroup::YQRadioButtonGroup( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YRadioButtonGroup( parent )
    , _recursive( false )
{
    setWidgetRep( this );
}


YQRadioButtonGroup::~YQRadioButtonGroup()
{
    // NOP
}


void
YQRadioButtonGroup::addRadioButton( YRadioButton * button )
{
    YRadioButtonGroup::addRadioButton( button );

    if ( button->value() )		// if this new button is active
    {
	uncheckOtherButtons( button );	// make it the only active
    }

    QRadioButton * radio_button = ( ( YQRadioButton * ) button )->getQtButton();

    connect ( radio_button,	SIGNAL ( toggled           ( bool ) ),
	      this, 		SLOT   ( radioButtonClicked( bool ) ) );
}


void
YQRadioButtonGroup::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void
YQRadioButtonGroup::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YRadioButtonGroup::setSize( newWidth, newHeight );
}


void
YQRadioButtonGroup::radioButtonClicked( bool newState )
{
    // Prevent infinite recursion: YQRadioButton::setValue() might cause Qt
    // signals that would cause recursion to this place.

    if ( _recursive )
	return;

    _recursive = true;

    QRadioButton * senderButton = (QRadioButton *) sender();

    // Implement radio box behaviour: Uncheck all other radio buttons

    for ( YRadioButtonListConstIterator it = radioButtonsBegin();
	  it != radioButtonsEnd();
	  ++it )
    {
	YQRadioButton * radioButton = dynamic_cast<YQRadioButton *> (*it);

	if ( radioButton )
	{
	    if ( radioButton->getQtButton() == senderButton )
	    {
		// If this button has been clicked, it is to be the RadioBox's
		// active button - regardless of newState. This is to avoid
		// RadioBoxes where no single button is active; otherwise the
		// second click would deactivate the only active button.

		radioButton->setValue( true );
	    }
	    else
	    {
		radioButton->setValue( false );
	    }
	}
    }

    _recursive = false;
}


#include "YQRadioButtonGroup.moc"
