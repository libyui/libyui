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


#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTimeField.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>


YQTimeField::YQTimeField( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTimeField( parent, label )
{
    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_timeEdit = new QTimeEdit( this );
    YUI_CHECK_NEW( _qt_timeEdit );
    _qt_timeEdit->setDisplayFormat( "hh:mm:ss");
    layout->addWidget( _qt_timeEdit );

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
    QFrame::setEnabled( enabled );
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
