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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

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


YQCheckBox::YQCheckBox( QWidget *		parent,
			const YWidgetOpt &	opt,
			const YCPString & 	label,
			bool 			initiallyChecked )
    : QGroupBox( parent )
    , YCheckBox( opt, label )
    , _dont_care( false )
{
    setWidgetRep( this );
    setFrameStyle( NoFrame );

    QBoxLayout * layout = new QBoxLayout( this, QBoxLayout::LeftToRight );

    _qt_checkbox = new QCheckBox( fromUTF8( label->value() ), this );
    layout->addSpacing( SPACING );
    layout->addWidget( _qt_checkbox );
    layout->addSpacing( SPACING );
    _qt_checkbox->setFont( opt.boldFont.value() ?
			   YQUI::ui()->boldFont() :
			   YQUI::ui()->currentFont() );
    _qt_checkbox->setChecked( initiallyChecked );

    connect( _qt_checkbox, 	SIGNAL( stateChanged( int ) ),
	     this, 		SLOT  ( stateChanged( int ) ) );
}


long
YQCheckBox::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return 2*SPACING + _qt_checkbox->sizeHint().width();
    else 			return _qt_checkbox->sizeHint().height();
}


void YQCheckBox::setSize( long newWidth, long newHeight )
{
    _qt_checkbox->resize( newWidth - 2*SPACING, newHeight );
    resize( newWidth, newHeight );
}


YCPValue
YQCheckBox::getValue()
{
    switch ( _qt_checkbox->state() )
    {
	case QButton::On:	return YCPBoolean( true  );
	case QButton::Off:	return YCPBoolean( false );
	case QButton::NoChange:	return YCPVoid();	// nil
    }
    
    y2error( "Unknown QCheckBox state: %d", (int) _qt_checkbox->state() );
    return YCPVoid();
}


void
YQCheckBox::setValue( const YCPValue & val )
{
    if ( val->isBoolean() )
    {
	setTristate( false );
	_qt_checkbox->setChecked( val->asBoolean()->value() );
    }
    else	// "Nil" -> set TriState: neither on nor off
    {
	setTristate( true );
    }
}


bool
YQCheckBox::isTristate()
{
    return _dont_care;
}


void
YQCheckBox::setTristate( bool tristate )
{
    _dont_care = tristate;
    _qt_checkbox->setTristate( tristate );

    if ( tristate )
	_qt_checkbox->setNoChange();
}


void YQCheckBox::setLabel( const YCPString & label )
{
    _qt_checkbox->setText( fromUTF8( label->value() ) );
    YCheckBox::setLabel( label );
}


void YQCheckBox::setEnabling( bool enabled )
{
    _qt_checkbox->setEnabled( enabled );
}


bool YQCheckBox::setKeyboardFocus()
{
    _qt_checkbox->setFocus();

    return true;
}



void YQCheckBox::stateChanged( int newState )
{
    int oldState = _qt_checkbox->state();

    y2milestone( "old: %d; new: %d", oldState, newState );
    
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQCheckBox.moc"
