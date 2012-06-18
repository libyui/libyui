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

  File:	      YQLayoutBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include "YQLayoutBox.h"


YQLayoutBox::YQLayoutBox( YWidget * 	parent,
			  YUIDimension	dimension )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YLayoutBox( parent, dimension )
{
    setWidgetRep( this );
}


YQLayoutBox::~YQLayoutBox()
{
    // NOP
}


void YQLayoutBox::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQLayoutBox::setSize( int newWidth, int newHeight )
{
    // yuiDebug() << "Resizing " << this << " to " << newWidth << " x " << newHeight << std::endl;
    resize( newWidth, newHeight );
    YLayoutBox::setSize( newWidth, newHeight );
}


void YQLayoutBox::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


#include "YQLayoutBox.moc"
