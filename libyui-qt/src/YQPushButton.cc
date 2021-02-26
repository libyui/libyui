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

  File:	      YQPushButton.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQPushButton.h"


YQPushButton::YQPushButton( YWidget *		parent,
			    const std::string &	label )
    : YQGenericButton( parent, label )
{
    setWidgetRep( this );

    QPushButton * button = new QPushButton( fromUTF8( label ), this );
    Q_CHECK_PTR( button );

    setQPushButton( button );
    
    button->setMinimumSize( 2, 2 );
    button->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( button->minimumSize()
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );
    
    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( hit()     ) );
}


YQPushButton::~YQPushButton()
{
}


int YQPushButton::preferredWidth()
{
    return 2 * YQButtonBorder + qPushButton()->sizeHint().width();
}


int YQPushButton::preferredHeight()
{
    return 2 * YQButtonBorder + qPushButton()->sizeHint().height();
}


void YQPushButton::setSize( int newWidth, int newHeight )
{
    qPushButton()->resize( newWidth  - 2 * YQButtonBorder,
			   newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


void YQPushButton::hit()
{
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


#include "YQPushButton.moc"
