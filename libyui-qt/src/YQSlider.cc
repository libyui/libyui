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

  File:	      YQSlider.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQSlider.h"


#define VSPACING 2
#define	HSPACING 2
#define MARGIN   2


YQSlider::YQSlider( 		    QWidget *		parent,
		    YWidgetOpt &	opt,
		    const YCPString &	label,
		    int 		minValue,
		    int 		maxValue,
		    int 		initialValue )

    : QWidget( parent )
    , YSlider( opt, label, minValue, maxValue, initialValue )
{
    setWidgetRep( this );

    vbox = new QVBox( this );
    vbox->setSpacing( VSPACING );
    vbox->setMargin( MARGIN );
    qt_label = new QLabel( fromUTF8(label->value() ), vbox );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont(YUIQt::ui()->currentFont());
    qt_label->setAlignment( Qt::AlignRight );

    hbox = new QHBox( vbox );
    hbox->setSpacing( HSPACING );

    qt_slider = new QSlider( minValue, maxValue,
			     1, // pageStep
			     initialValue,
			     QSlider::Horizontal, hbox );
    qt_slider->setFont( YUIQt::ui()->currentFont() );

    qt_spinbox = new QSpinBox( minValue, maxValue,
			       1, // step
			       hbox );
    qt_spinbox->setValue( initialValue );
    qt_spinbox->setFont( YUIQt::ui()->currentFont() );

    qt_label->setBuddy( qt_spinbox );

    setValue( initialValue );

    connect( qt_spinbox, SIGNAL( valueChanged(int) ),
	     qt_slider,  SLOT  ( setValue    (int) ) );

    connect( qt_slider, 	SIGNAL( valueChanged(int) ),
	     qt_spinbox, SLOT  ( setValue    (int) ) );

    connect( qt_spinbox, SIGNAL( valueChanged(int) ),
	     this,  	SLOT  ( setValueSlot(int) ) );
}


void YQSlider::setEnabling( bool enabled )
{
    qt_label->setEnabled  ( enabled );
    qt_slider->setEnabled ( enabled );
    qt_spinbox->setEnabled( enabled );
}


long YQSlider::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ) return 200;
    else return vbox->sizeHint().height();
}


void YQSlider::setSize( long newWidth, long newHeight )
{
    vbox->resize( newWidth, newHeight );
    resize( newWidth, newHeight );
}

void YQSlider::setLabel( const YCPString & newLabel )
{
    qt_label->setText( fromUTF8( newLabel->value() ) ) ;

    YSlider::setLabel( newLabel );
}


void YQSlider::setValue( int newValue )
{
    qt_slider->setValue( newValue );
    YSlider::setValue( newValue );
}


void YQSlider::setValueSlot( int newValue )
{
    setValue( newValue );

    if ( getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


bool YQSlider::setKeyboardFocus()
{
    qt_spinbox->setFocus();

    return true;
}


#include "YQSlider.moc.cc"
