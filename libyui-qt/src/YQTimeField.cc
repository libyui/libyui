/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                        ***  Qt4 plugin  ***                                         ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

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


YQTimeField::YQTimeField( YWidget * parent, const std::string & label )
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


void YQTimeField::setValue( const std::string & newValue )
{
    _qt_timeEdit->setTime(  QTime::fromString( fromUTF8( newValue ), Qt::ISODate ) );
}


void YQTimeField::setLabel( const std::string & newLabel )
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
