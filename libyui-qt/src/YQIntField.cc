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

  File:	      YQIntField.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YEvent.h"
#include "YQIntField.h"


YQIntField::YQIntField( QWidget *		parent,
			YWidgetOpt &		opt,
			const YCPString &	label,
			int 			minValue,
			int 			maxValue,
			int 			initialValue )

    : QVBox( parent )
    , YIntField( opt, label, minValue, maxValue, initialValue )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );
    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );
    _qt_label->setAlignment( Qt::AlignLeft );

    _qt_spinbox = new QSpinBox( minValue, maxValue,
				1, // step
				this );
    _qt_spinbox->setValue( initialValue );
    _qt_spinbox->setFont( YUIQt::ui()->currentFont() );

    _qt_label->setBuddy( _qt_spinbox );

    setValue( initialValue );

    connect( _qt_spinbox, SIGNAL( valueChanged(int) ),
	     this,  	  SLOT  ( setValueSlot(int) ) );
}


void YQIntField::setEnabling( bool enabled )
{
    _qt_label->setEnabled  ( enabled );
    _qt_spinbox->setEnabled( enabled );
}


long YQIntField::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return sizeHint().width();
    else			return sizeHint().height();
}


void YQIntField::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}

void YQIntField::setLabel( const YCPString & newLabel )
{
    _qt_label->setText( fromUTF8( newLabel->value() ) ) ;

    YIntField::setLabel( newLabel );
}


void YQIntField::setValue( int newValue )
{
    _qt_spinbox->blockSignals( true );
    _qt_spinbox->setValue( newValue );
    YIntField::setValue( newValue );
    _qt_spinbox->blockSignals( false );
}

int YQIntField::value()
{ 
    YIntField::setValue( _qt_spinbox->value() );
    return YIntField::value(); 
}


void YQIntField::setValueSlot( int newValue )
{
    setValue( newValue );

    if ( getNotify() )
	YUIQt::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQIntField::setKeyboardFocus()
{
    _qt_spinbox->setFocus();

    return true;
}


#include "YQIntField.moc.cc"
