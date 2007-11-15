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

  File:	      YQCheckBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qcheckbox.h>
#include <qlayout.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQCheckBox.h"


#define SPACING 8


YQCheckBox::YQCheckBox( YWidget *	parent,
			const string & 	label,
			bool 		checked )
    : QGroupBox( (QWidget *) parent->widgetRep() )
    , YCheckBox( parent, label )
{
    setWidgetRep( this );
    setFrameStyle( NoFrame );

    QBoxLayout * layout = new QBoxLayout( this, QBoxLayout::LeftToRight );

    _qt_checkBox = new QCheckBox( fromUTF8( label ), this );
    YUI_CHECK_NEW( _qt_checkBox );
    
    layout->addSpacing( SPACING );
    layout->addWidget( _qt_checkBox );
    layout->addSpacing( SPACING );

    _qt_checkBox->setChecked( checked );

    connect( _qt_checkBox, 	SIGNAL( stateChanged( int ) ),
	     this, 		SLOT  ( stateChanged( int ) ) );
}


YQCheckBox::~YQCheckBox()
{
    // NOP
}


YCheckBoxState
YQCheckBox::value()
{
    switch ( _qt_checkBox->state() )
    {
	case QButton::On:	return YCheckBox_on;
	case QButton::Off:	return YCheckBox_off;
	case QButton::NoChange:	return YCheckBox_dont_care;
    }

    return YCheckBox_off;
}


void
YQCheckBox::setValue( YCheckBoxState newValue )
{
    switch ( newValue )
    {
	case YCheckBox_on:
	    _qt_checkBox->setChecked( true );
	    _qt_checkBox->setTristate( false );
	    break;

	case YCheckBox_off:
	    _qt_checkBox->setChecked( false );
	    _qt_checkBox->setTristate( false );
	    break;

	case YCheckBox_dont_care:
	    _qt_checkBox->setTristate( true );
	    _qt_checkBox->setNoChange();
	    break;
    }
}


void YQCheckBox::setLabel( const string & label )
{
    _qt_checkBox->setText( fromUTF8( label ) );
    YCheckBox::setLabel( label );
}


void YQCheckBox::setUseBoldFont( bool useBold )
{
    _qt_checkBox->setFont( useBold ?
			   YQUI::ui()->boldFont() :
			   YQUI::ui()->currentFont() );
    
    YCheckBox::setUseBoldFont( useBold );
}


void YQCheckBox::setEnabled( bool enabled )
{
    _qt_checkBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQCheckBox::preferredWidth()
{
    return 2*SPACING + _qt_checkBox->sizeHint().width();
}


int YQCheckBox::preferredHeight()
{
    return _qt_checkBox->sizeHint().height();
}


void YQCheckBox::setSize( int newWidth, int newHeight )
{
    _qt_checkBox->resize( newWidth - 2*SPACING, newHeight );
    resize( newWidth, newHeight );
}


bool YQCheckBox::setKeyboardFocus()
{
    _qt_checkBox->setFocus();

    return true;
}


void YQCheckBox::stateChanged( int newState )
{
    // y2milestone( "new state: %d", newState );
    
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQCheckBox.moc"
