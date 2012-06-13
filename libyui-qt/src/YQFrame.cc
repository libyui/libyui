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

  File:	      YQFrame.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "YQUI.h"
#include "utf8.h"
#include <QDebug>

using std::max;

#include "YQFrame.h"


YQFrame::YQFrame( YWidget * 		parent,
		  const std::string &	initialLabel )
    : QGroupBox( (QWidget *) parent->widgetRep() )
    , YFrame( parent, initialLabel )
{
    setWidgetRep ( this );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


YQFrame::~YQFrame()
{
    // NOP
}


void YQFrame::setEnabled( bool enabled )
{
    QGroupBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void
YQFrame::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );

    if ( hasChildren() )
    {
        int left, top, right, bottom;
        getContentsMargins( &left, &top, &right, &bottom );
	int newChildWidth  = newWidth - left - right;
	int newChildHeight = newHeight - bottom - top;

	firstChild()->setSize( newChildWidth, newChildHeight );

	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( left, top );
    }
}


void
YQFrame::setLabel( const std::string & newLabel )
{
    YFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


int YQFrame::preferredWidth()
{
    int preferredWidth = hasChildren() ? firstChild()->preferredWidth() : 0;
    int left, top, right, bottom;
    getContentsMargins( &left, &top, &right, &bottom );

    preferredWidth += left + right;

    if ( minimumSizeHint().width() >  preferredWidth )
	preferredWidth = minimumSizeHint().width();

    return preferredWidth;
}


int YQFrame::preferredHeight()
{
    int preferredHeight = hasChildren() ? firstChild()->preferredHeight() : 0;
    int left, top, right, bottom;
    getContentsMargins( &left, &top, &right, &bottom );

    return preferredHeight + top + left;
}


#include "YQFrame.moc"
