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

  File:	      YQAlignment.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include <qpixmap.h>
#include "YQAlignment.h"

using std::string;


YQAlignment::YQAlignment( YWidget *	  	parent,
			  YAlignmentType 	horAlign,
			  YAlignmentType 	vertAlign )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YAlignment( parent, horAlign, vertAlign )
{
    setWidgetRep( this );
}


YQAlignment::YQAlignment( YWidget *	  	yParent,
			  QWidget *		qParent,
			  YAlignmentType 	horAlign,
			  YAlignmentType 	vertAlign )
    : QWidget( qParent )
    , YAlignment( yParent, horAlign, vertAlign )
{
    setWidgetRep( this );
}


void YQAlignment::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQAlignment::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *) ( child->widgetRep() );
    qw->move( newX, newY );
}


void YQAlignment::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YAlignment::setSize( newWidth, newHeight );
}


void YQAlignment::setBackgroundPixmap( const string & pixmapFileName )
{
    string pixmapName = pixmapFileName;

    YAlignment::setBackgroundPixmap( pixmapName );	// Prepend path etc.
    pixmapName = YAlignment::backgroundPixmap();

    if ( pixmapName.empty() )	// Delete any old background pixmap
    {
            QPalette pal = palette();
            pal.setBrush(backgroundRole(), QBrush());
            setPalette(pal);
    }
    else			// Set a new background pixmap
    {
	QPixmap pixmap( pixmapName.c_str() );

	if ( pixmap.isNull() )
	{
	    yuiError() << "Can't load background pixmap \"" << pixmapName << "\"" << endl;
	}
	else
	{
            QPalette pal = palette();
            pal.setBrush(backgroundRole(), QBrush(pixmap));
            setPalette(pal);
	}
    }
}

#include "YQAlignment.moc"
