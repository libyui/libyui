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

  File:		YQTimeField.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTimeField.h"
#include "YQWidgetCaption.h"



YQTimeField::YQTimeField( YWidget * parent, const string & label )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YTimeField( parent, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );

    _qt_timeEdit = new QTimeEdit( this );
    YUI_CHECK_NEW( _qt_timeEdit );

    _qt_timeEdit->setAutoAdvance( true );
    _caption->setBuddy( _qt_timeEdit );
}


YQTimeField::~YQTimeField()
{
    // NOP
}


string YQTimeField::value()
{
    return toUTF8( _qt_timeEdit->time().toString( Qt::ISODate ) );
}


void YQTimeField::setValue( const string & newValue )
{
    _qt_timeEdit->setTime(  QTime::fromString( fromUTF8( newValue ), Qt::ISODate ) );
}


void YQTimeField::setLabel( const string & newLabel )
{
    _caption->setText( fromUTF8( newLabel ) );
    YTimeField::setLabel( newLabel );
}


void YQTimeField::setEnabled( bool enabled )
{
    QVBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQTimeField::preferredWidth()
{
    return sizeHint().width();
}


int YQTimeField::preferredHeight()
{
    return sizeHint().height();
}


void YQTimeField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQTimeField::setKeyboardFocus()
{
    _qt_timeEdit->setFocus();

    return true;
}


#include "YQTimeField.moc"
