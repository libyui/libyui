/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      	YQDateField.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <QCalendarWidget>
#include <QVBoxLayout>

#include "utf8.h"
#include "YQUI.h"
#include "YQDateField.h"
#include "YQWidgetCaption.h"


YQDateField::YQDateField( YWidget * parent, const std::string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YDateField( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_calendar = new QCalendarWidget( this );
    YUI_CHECK_NEW( _qt_calendar );
    layout->addWidget( _qt_calendar );

    _caption->setBuddy( _qt_calendar );
}


YQDateField::~YQDateField()
{
    // NOP
}


string YQDateField::value()
{
    return toUTF8( _qt_calendar->selectedDate().toString( Qt::ISODate ) );
}


void YQDateField::setValue( const std::string & newValue )
{
    _qt_calendar->setSelectedDate( QDate::fromString( fromUTF8( newValue ), Qt::ISODate ) );

}


void YQDateField::setLabel( const std::string & newLabel )
{
    _caption->setText( fromUTF8( newLabel ) );
    YDateField::setLabel( newLabel );
}


void YQDateField::setEnabled( bool enabled )
{
    QFrame::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQDateField::preferredWidth()
{
    return sizeHint().width();
}


int YQDateField::preferredHeight()
{
    return sizeHint().height();
}


void YQDateField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQDateField::setKeyboardFocus()
{
    _qt_calendar->setFocus();

    return true;
}



#include "YQDateField.moc"
