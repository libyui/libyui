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

  File:	      YQRadioButton.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qradiobutton.h>
#include <qlayout.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YEvent.h"
#include "YQRadioButton.h"

#define SPACING 8

// +----+----------------------------------+----+
// |	|(o) RadioButtonlabel		   |	|
// +----+----------------------------------+----+
// <----> SPACING			   <---->



YQRadioButton::YQRadioButton( QWidget * 		parent,
			      YWidgetOpt & 		opt,
			      YRadioButtonGroup *	rbg,
			      const YCPString & 	label,
			      bool 			checked )
    : QGroupBox( parent )
    , YRadioButton( opt, label, rbg )
{
    setWidgetRep( this );
    setFrameStyle( NoFrame );

    QBoxLayout * layout = new QBoxLayout( this, QBoxLayout::LeftToRight );

    _qt_radiobutton = new QRadioButton( fromUTF8( label->value() ), this );
    layout->addSpacing( SPACING );
    layout->addWidget( _qt_radiobutton );
    layout->addSpacing( SPACING );
    _qt_radiobutton->setFont( YUIQt::ui()->currentFont() );
    _qt_radiobutton->setChecked( checked );

    connect ( _qt_radiobutton, SIGNAL ( toggled ( bool ) ),
	      this, SLOT ( changed ( bool ) ) );
}


long YQRadioButton::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ ) return 2 * SPACING + _qt_radiobutton->sizeHint().width();
    else return _qt_radiobutton->sizeHint().height();
}


void YQRadioButton::setSize( long newWidth, long newHeight )
{
    _qt_radiobutton->resize( newWidth - 2*SPACING, newHeight );
    resize( newWidth, newHeight );
}


YCPBoolean YQRadioButton::getValue()
{
    return YCPBoolean( _qt_radiobutton->isChecked() );
}


void YQRadioButton::setValue( const YCPBoolean & c )
{
    _qt_radiobutton->setChecked( c->value() );
}


void YQRadioButton::setLabel( const YCPString & label )
{
    _qt_radiobutton->setText( fromUTF8(label->value() ) );
    YRadioButton::setLabel( label );
}


void YQRadioButton::setEnabling( bool enabled )
{
    _qt_radiobutton->setEnabled( enabled );
}


QRadioButton * YQRadioButton::getQtButton()
{
    return _qt_radiobutton;
}


bool YQRadioButton::setKeyboardFocus()
{
    _qt_radiobutton->setFocus();

    return true;
}


// slots

void YQRadioButton::changed( bool newState )
{
    if ( getNotify() && newState )
	YUIQt::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQRadioButton.moc.cc"
