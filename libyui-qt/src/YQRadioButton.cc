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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qradiobutton.h>
#include <qlayout.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YEvent.h"
#include "YQRadioButton.h"
#include "YRadioButtonGroup.h"
#include "YQSignalBlocker.h"

using std::string;

#define SPACING 8

// +----+----------------------------------+----+
// |	|(o) RadioButtonlabel		   |	|
// +----+----------------------------------+----+
// <----> SPACING			   <---->



YQRadioButton::YQRadioButton( YWidget * 	parent,
			      const string & 	label,
			      bool 		checked )
    : QGroupBox( (QWidget *) (parent->widgetRep() ) )
    , YRadioButton( parent, label )
{
    setWidgetRep( this );
    setFrameStyle( NoFrame );

    QBoxLayout * layout = new QBoxLayout( this, QBoxLayout::LeftToRight );

    _qt_radioButton = new QRadioButton( fromUTF8( label ), this );
    
    layout->addSpacing( SPACING );
    layout->addWidget( _qt_radioButton );
    layout->addSpacing( SPACING );
    _qt_radioButton->setChecked( checked );

    installEventFilter(this);

    connect ( _qt_radioButton,	SIGNAL( toggled ( bool ) ),
	      this,		SLOT  ( changed ( bool ) ) );
}


void
YQRadioButton::setUseBoldFont( bool useBold )
{
    _qt_radioButton->setFont( useBold ?
			      YQUI::yqApp()->boldFont() :
			      YQUI::yqApp()->currentFont() );
    
    YRadioButton::setUseBoldFont( useBold );
}


int YQRadioButton::preferredWidth()
{
    return 2 * SPACING + _qt_radioButton->sizeHint().width();
}


int YQRadioButton::preferredHeight()
{
    return _qt_radioButton->sizeHint().height();
}


void YQRadioButton::setSize( int newWidth, int newHeight )
{
    _qt_radioButton->resize( newWidth - 2*SPACING, newHeight );
    resize( newWidth, newHeight );
}


bool YQRadioButton::value()
{
    return _qt_radioButton->isChecked();
}


void YQRadioButton::setValue( bool newValue )
{
    YQSignalBlocker sigBlocker( _qt_radioButton );

    _qt_radioButton->setChecked( newValue );

    if ( newValue )
    {
	YRadioButtonGroup * group = buttonGroup();

	if ( group )
	    group->uncheckOtherButtons( this );
    }
}


void YQRadioButton::setLabel( const string & label )
{
    _qt_radioButton->setText( fromUTF8( label ) );
    YRadioButton::setLabel( label );
}


void YQRadioButton::setEnabled( bool enabled )
{
    _qt_radioButton->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


QRadioButton * YQRadioButton::getQtButton()
{
    return _qt_radioButton;
}


bool YQRadioButton::setKeyboardFocus()
{
    _qt_radioButton->setFocus();

    return true;
}


// slots

void YQRadioButton::changed( bool newState )
{
    if ( notify() && newState )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQRadioButton::eventFilter( QObject * obj, QEvent * event )
{
    if ( event && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (event);

        if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
        {
             y2milestone( "Right click on button detected" );
             YQUI::ui()->maybeLeftHandedUser();
        }
    }

    return QObject::eventFilter( obj, event );
}


#include "YQRadioButton.moc"
