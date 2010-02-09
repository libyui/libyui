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

  File:	      YQImage.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <unistd.h>
#include <qpixmap.h>
#include <qmovie.h>
#include <qlabel.h>
#include <QIcon>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQImage.h"



YQImage::YQImage( YWidget *		parent,
		  const string &	imageFileName,
		  bool 			animated )
    : QLabel( (QWidget *) parent->widgetRep() )
    , YImage( parent, imageFileName, animated )
{
    setWidgetRep( this );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );

    setScaledContents( false );
    _pixmapHeight = 0;
    _pixmapWidth  = 0;

    setImage( imageFileName, animated );
}


YQImage::~YQImage()
{
    // NOP
}


void
YQImage::setImage( const string & fileName, bool animated )
{
    YImage::setImage( fileName, animated );
    
    if ( animated )
    {
	QMovie movie( fromUTF8( imageFileName() ) );
	
	if ( movie.isValid() )
	{
	    yuiError() << "Couldn't load animation from " << imageFileName() << endl;
	}
	else
	{
	    yuiDebug() << "Loading animation from " << imageFileName() << endl;
	    QLabel::setMovie( &movie );
	}
    }
    else
    {
	QPixmap pixmap( fromUTF8( imageFileName() ) );

	if ( pixmap.isNull() )
	{
	    yuiError() << "Couldn't load pixmap from " << imageFileName() << endl;
	}
	else
	{
	    if ( autoScale() )
	    {
		_pixmapWidth  = 0;
		_pixmapHeight = 0;
	    }
	    else
	    {
		_pixmapWidth  = pixmap.size().width();
		_pixmapHeight = pixmap.size().height();
	    }
	
	    yuiDebug() << "Loading image from " << imageFileName()
		       << " (" << pixmap.size().width() << " x " << pixmap.size().height() << ")"
		       << endl;
	    
	    QLabel::setPixmap( pixmap );
	}
    }
}

void YQImage::setAutoScale( bool newAutoScale )
{
    if ( autoScale() == newAutoScale )
	return;

    YImage::setAutoScale( newAutoScale );
    setScaledContents( newAutoScale );

    // Trigger image re-display
    setImage( imageFileName(), animated() );
}


int YQImage::preferredWidth()
{
    if ( hasZeroSize( YD_HORIZ ) )
	return 0;

    if ( animated() )
    {
	// a QMovie doesn't have a size() method, thus use sizeHint() instead.
	
	return sizeHint().width();
    }
    else
    {
	// for non-animated images, the background pixmap is used, thus
	// sizeHint() will always return ( 0,0 ) - thus, use the internally
	// stored sizes instead.
	
	return _pixmapWidth;
    }
}


int YQImage::preferredHeight()
{
    if ( hasZeroSize( YD_VERT ) )
	return 0;

    if ( animated() )
    {
	// a QMovie doesn't have a size() method, thus use sizeHint() instead.

	return sizeHint().height();
    }
    else
    {
	// for non-animated images, the background pixmap is used, thus
	// sizeHint() will always return ( 0,0 ) - thus, use the internally
	// stored sizes instead.
	
	return _pixmapHeight;
    }
}


void YQImage::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}

void YQImage::setEnabled( bool enable )
{
    yuiDebug() << "setEnabled: " << enable << endl;
    
    if (enable)
    {
	setImage( imageFileName(), animated() );
    }
    else
    {
	// Trigger image re-display
	QPixmap pixmap( fromUTF8( imageFileName() ) );
	QIcon icon(pixmap);
	QLabel::setPixmap( icon.pixmap( pixmap.size(), QIcon::Disabled, QIcon::Off) );
    }
}

#include "YQImage.moc"
