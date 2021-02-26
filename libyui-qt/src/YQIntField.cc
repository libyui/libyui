/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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


#include <qspinbox.h>

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQIntField.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>

YQIntField::YQIntField( YWidget *	parent,
			const string &	label,
			int 		minValue,
			int 		maxValue,
			int 		initialValue )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YIntField( parent, label, minValue, maxValue )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption    = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_spinBox = new QSpinBox(this);
    _qt_spinBox->setMinimum(minValue);
    _qt_spinBox->setMaximum(maxValue);
    _qt_spinBox->setSingleStep(1);

    YUI_CHECK_NEW( _qt_spinBox );
    layout->addWidget( _qt_spinBox );

    _qt_spinBox->setValue( initialValue );

    _caption->setBuddy( _qt_spinBox );

    setValue( initialValue );

    connect( _qt_spinBox, SIGNAL( valueChanged    ( int ) ),
	     this,  	  SLOT  ( valueChangedSlot( int ) ) );
}


YQIntField::~YQIntField()
{
    // NOP
}


int
YQIntField::value()
{
    return _qt_spinBox->value();
}


void
YQIntField::setValueInternal( int newValue )
{
    YQSignalBlocker sigBlocker( _qt_spinBox );
    _qt_spinBox->setValue( newValue );
}


void
YQIntField::valueChangedSlot( int newValue )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
    emit valueChanged( newValue );
}


void
YQIntField::setLabel( const string & newLabel )
{
    YIntField::setLabel( newLabel );
    _caption->setText( newLabel );
}


void
YQIntField::setEnabled( bool enabled )
{
    _caption->setEnabled  ( enabled );
    _qt_spinBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQIntField::preferredWidth()
{
    return sizeHint().width();
}


int
YQIntField::preferredHeight()
{
    return sizeHint().height();
}


void
YQIntField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQIntField::setKeyboardFocus()
{
    _qt_spinBox->setFocus();

    return true;
}


#include "YQIntField.moc"
