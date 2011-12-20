
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
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
  |*************************************************************************** */
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

  File:	      YQLayoutBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
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
    // yuiDebug() << "Resizing " << this << " to " << newWidth << " x " << newHeight << endl;
    resize( newWidth, newHeight );
    YLayoutBox::setSize( newWidth, newHeight );
}


void YQLayoutBox::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


#include "YQLayoutBox.moc"
