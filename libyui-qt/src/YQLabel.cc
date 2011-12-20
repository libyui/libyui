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

  File:	      YQLabel.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlabel.h>

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YQLabel.h"


YQLabel::YQLabel( YWidget * 		parent,
		  const string &	text,
		  bool 			isHeading,
		  bool 			isOutputField )
    : QLabel( (QWidget *) parent->widgetRep() )
    , YLabel( parent, text, isHeading, isOutputField )
{
    setWidgetRep( this );

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


void YQLabel::setText( const string & newText )
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


#include "YQLabel.moc"
