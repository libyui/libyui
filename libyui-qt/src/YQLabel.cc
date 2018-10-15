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

  File:	      YQLabel.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlabel.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YQLabel.h"


YQLabel::YQLabel( YWidget * 		parent,
		  const std::string &	text,
		  bool 			isHeading,
		  bool 			isOutputField )
    : QLabel( (QWidget *) parent->widgetRep() )
    , YLabel( parent, text, isHeading, isOutputField )
{
    setWidgetRep( this );

    setTextInteractionFlags( Qt::TextSelectableByMouse );
    setTextFormat( Qt::PlainText );
    QLabel::setText( fromUTF8( text ) );
    setIndent(0);

    if ( isHeading )
    {
	setFont( YQUI::yqApp()->headingFont() );
    }
    else if ( isOutputField )
    {
	setFrameStyle ( QFrame::Panel | QFrame::Sunken );
	setLineWidth(2);
	setMidLineWidth(2);
    }

    setMargin( YQWidgetMargin );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );
}


YQLabel::~YQLabel()
{
    // NOP
}


void YQLabel::setText( const std::string & newText )
{
    YLabel::setText( newText );
    QLabel::setText( fromUTF8( newText ) );
}


void YQLabel::setUseBoldFont( bool useBold )
{
    setFont( useBold ?
	     YQUI::yqApp()->boldFont() :
	     YQUI::yqApp()->currentFont() );

    YLabel::setUseBoldFont( useBold );
}


void YQLabel::setEnabled( bool enabled )
{
    QLabel::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQLabel::preferredWidth()
{
    return sizeHint().width();
}


int YQLabel::preferredHeight()
{
    return sizeHint().height();
}


void YQLabel::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}



