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
#include "YQIntField.h"


#define VSPACING 2
#define	HSPACING 2
#define MARGIN   2


YQIntField::YQIntField( QWidget *		parent,
			YWidgetOpt &		opt,
			const YCPString &	label,
			int 			minValue,
			int 			maxValue,
			int 			initialValue )

    : QWidget( parent )
    , YIntField( opt, label, minValue, maxValue, initialValue )
{
    setWidgetRep( this );

    _vbox = new QVBox( this );
    _vbox->setSpacing( VSPACING );
    _vbox->setMargin( MARGIN );
    _qt_label = new QLabel( fromUTF8( label->value() ), _vbox );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );
    _qt_label->setAlignment( Qt::AlignRight );

    _qt_spinbox = new QSpinBox( minValue, maxValue,
				1, // step
				_vbox );
    _qt_spinbox->setValue( initialValue );
    _qt_spinbox->setFont( YUIQt::ui()->currentFont() );

    _qt_label->setBuddy( _qt_spinbox );

    setValue( initialValue );

    connect( _qt_spinbox, SIGNAL( valueChanged(int) ),
	     this,  	SLOT  ( setValueSlot(int) ) );
}


void YQIntField::setEnabling( bool enabled )
{
    _qt_label->setEnabled  ( enabled );
    _qt_spinbox->setEnabled( enabled );
}


long YQIntField::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return _vbox->sizeHint().width();
    else			return _vbox->sizeHint().height();
}


void YQIntField::setSize( long newWidth, long newHeight )
{
    _vbox->resize( newWidth, newHeight );
    resize( newWidth, newHeight );
}

void YQIntField::setLabel( const YCPString & newLabel )
{
    _qt_label->setText( fromUTF8( newLabel->value() ) ) ;

    YIntField::setLabel( newLabel );
}


void YQIntField::setValue( int newValue )
{
    _qt_spinbox->setValue( newValue );
    YIntField::setValue( newValue );
}


void YQIntField::setValueSlot( int newValue )
{
    setValue( newValue );

    if ( getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


bool YQIntField::setKeyboardFocus()
{
    _qt_spinbox->setFocus();

    return true;
}


#include "YQIntField.moc.cc"
