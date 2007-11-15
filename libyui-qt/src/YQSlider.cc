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


#include <qslider.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qvbox.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQSlider.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"


YQSlider::YQSlider( YWidget *		parent,
		    const string &	label,
		    int 		minValue,
		    int 		maxValue,
		    int 		initialValue,
		    bool		reverseLayout )

    : QVBox( (QWidget *) parent->widgetRep() )
    , YSlider( parent, label, minValue, maxValue )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, toUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    
    _hbox = new QHBox( this );
    YUI_CHECK_NEW( _hbox );
    
    _hbox->setSpacing( YQWidgetSpacing );

    if ( reverseLayout )
    {
	_qt_spinBox = new QSpinBox( minValue, maxValue,
				    1, // step
				    _hbox );
    }
    else
    {
	_caption->setAlignment( Qt::AlignRight );
    }
    
    _qt_slider = new QSlider( minValue, maxValue,
			      1, // pageStep
			      initialValue,
			      QSlider::Horizontal, _hbox );
    YUI_CHECK_NEW( _qt_slider );

    if ( ! reverseLayout )
    {
	_qt_spinBox = new QSpinBox( minValue, maxValue,
				    1, // step
				    _hbox );
    }
    YUI_CHECK_NEW( _qt_spinBox );
    
    _qt_spinBox->setValue( initialValue );
    _caption->setBuddy( _qt_spinBox );

    setValue( initialValue );

    connect( _qt_spinBox, SIGNAL( valueChanged(int) ),
	     _qt_slider,  SLOT  ( setValue    (int) ) );

    connect( _qt_slider,  SIGNAL( valueChanged(int) ),
	     _qt_spinBox, SLOT  ( setValue    (int) ) );

    connect( _qt_spinBox, SIGNAL( valueChanged    (int) ),
	     this,  	  SLOT  ( valueChangedSlot(int) ) );
}


YQSlider::~YQSlider()
{
    // NOP
}


int
YQSlider::value()
{
    return _qt_spinBox->value();
}


void
YQSlider::setValueInternal( int newValue )
{
    YQSignalBlocker sigBlocker1( _qt_spinBox );
    YQSignalBlocker sigBlocker2( _qt_slider  );
    _qt_slider->setValue ( newValue );
    _qt_spinBox->setValue( newValue );
}


void
YQSlider::valueChangedSlot( int newValue )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
    
    emit valueChanged( newValue );
}


void
YQSlider::setEnabled( bool enabled )
{
    _caption->setEnabled  ( enabled );
    _qt_slider->setEnabled ( enabled );
    _qt_spinBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQSlider::preferredWidth()
{
    int hintWidth = _caption->isShown() ? _caption->sizeHint().width() : 0;
    
    // Arbitrary value - there is no really good default
    return max( 200, hintWidth );
}


int
YQSlider::preferredHeight()
{
    return sizeHint().height();
}


void
YQSlider::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void
YQSlider::setLabel( const string & newLabel )
{
    _caption->setText( newLabel );
    YSlider::setLabel( newLabel );
}


bool
YQSlider::setKeyboardFocus()
{
    _qt_spinBox->setFocus();

    return true;
}


#include "YQSlider.moc"
