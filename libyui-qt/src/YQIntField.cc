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


YQIntField::YQIntField( YUIQt *			yuiqt,
			QWidget *		parent,
			YWidgetOpt &		opt,
			const YCPString &	label,
			int 			minValue,
			int 			maxValue,
			int 			initialValue )

    : QWidget( parent )
    , YIntField( opt, label, minValue, maxValue, initialValue )
    , yuiqt( yuiqt )
{
    setWidgetRep( (QWidget *) this );

    vbox = new QVBox( this );
    vbox->setSpacing( VSPACING );
    vbox->setMargin( MARGIN );
    qt_label = new QLabel( fromUTF8(label->value() ), vbox );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont(yuiqt->currentFont());
    qt_label->setAlignment( Qt::AlignRight );

    qt_spinbox = new QSpinBox( minValue, maxValue,
			       1, // step
			       vbox );
    qt_spinbox->setValue( initialValue );
    qt_spinbox->setFont( yuiqt->currentFont() );

    qt_label->setBuddy( qt_spinbox );

    setValue( initialValue );

    connect( qt_spinbox, SIGNAL( valueChanged(int) ),
	     this,  	SLOT  ( setValueSlot(int) ) );
}


void YQIntField::setEnabling( bool enabled )
{
    qt_label->setEnabled  ( enabled );
    qt_spinbox->setEnabled( enabled );
}


long YQIntField::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ)	return vbox->sizeHint().width();
    else			return vbox->sizeHint().height();
}


void YQIntField::setSize( long newWidth, long newHeight )
{
    vbox->resize( newWidth, newHeight );
    resize( newWidth, newHeight );
}

void YQIntField::setLabel( const YCPString &newLabel )
{
    qt_label->setText( fromUTF8( newLabel->value() ) ) ;

    YIntField::setLabel( newLabel );
}


void YQIntField::setValue( int newValue )
{
    qt_spinbox->setValue( newValue );
    YIntField::setValue( newValue );
}


void YQIntField::setValueSlot( int newValue )
{
    setValue( newValue );

    if ( getNotify() )
	yuiqt->returnNow( YUIInterpreter::ET_WIDGET, this );
}


bool YQIntField::setKeyboardFocus()
{
    qt_spinbox->setFocus();

    return true;
}


#include "YQIntField.moc.cc"
