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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <unistd.h>
#include <qpixmap.h>
#include <qmovie.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQImage.h"



YQImage::YQImage( QWidget * 		parent,
		  const YWidgetOpt & 	opt,
		  const YCPByteblock &	byteblock )
    : QLabel( parent )
    , YImage( opt )
{
    init( parent, opt );

    if ( _animated )
    {
	QMovie movie( byteblock->size() );
	movie.pushData( byteblock->value(), byteblock->size() );
	setMovie( movie );
    }
    else
    {
	QPixmap pixmap;
	pixmap.loadFromData( byteblock->value(), byteblock->size() );
	yqSetPixmap( pixmap );
    }
}


YQImage::YQImage( QWidget * 		parent,
		  const YWidgetOpt &	opt,
		  const YCPString & 	ycp_file_name )
    : QLabel( parent )
    , YImage( opt )
{
    init( parent, opt );
    QString file_name = fromUTF8( ycp_file_name->value() );
    y2debug( "Loading image from %s", (const char *) file_name );

    if ( _animated )
    {
	QMovie movie( file_name );
	
	if ( movie .isNull() )
	{
	    y2error( "Couldn't load animation from %s", (const char *) file_name );
	}
	else
	{
	    setMovie( movie );
	}
    }
    else
    {
	QPixmap pixmap( file_name );

	if ( pixmap.isNull() )
	{
	    y2error( "Couldn't load pixmap from %s", (const char *) file_name );
	}
	else
	{
	    yqSetPixmap( pixmap );
	}
    }
}


void
YQImage::init( QWidget * parent, const YWidgetOpt & opt )
{
    setWidgetRep( this );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );

    _zeroWidth	= opt.zeroWidth.value();
    _zeroHeight	= opt.zeroHeight.value();
    _animated	= opt.animated.value();
    _tiled	= opt.tiled.value();
    setScaledContents( opt.scaleToFit.value() );
    windowID	 = winId();
    _pixmapHeight = 0;
    _pixmapWidth  = 0;

    if ( _zeroWidth  )
	setStretchable( YD_HORIZ, true );

    if ( _zeroHeight )
	setStretchable( YD_VERT,  true );
}


void
YQImage::yqSetPixmap( const QPixmap & pixmap )
{
    if ( hasScaledContents() )
    {
	_pixmapWidth  = 0;
	_pixmapHeight = 0;
    }
    else
    {
	_pixmapWidth  = pixmap.size().width();
	_pixmapHeight = pixmap.size().height();
    }
	
    QLabel::setPixmap( pixmap );
}


long YQImage::nicesize( YUIDimension dim )
{
    if ( _animated )
    {
	// a QMovie doesn't have a size() method, thus use sizeHint() instead.
	
	if ( dim == YD_HORIZ )	return _zeroWidth  ? 0L : sizeHint().width();
	else			return _zeroHeight ? 0L : sizeHint().height();
    }
    else
    {
	// for non-animated images, the background pixmap is used, thus
	// sizeHint() will always return ( 0,0 ) - thus, use the internally
	// stored sizes instead.
	
	if ( dim == YD_HORIZ )	return _zeroWidth  ? 0L : _pixmapWidth;
	else			return _zeroHeight ? 0L : _pixmapHeight;
    }
}


void YQImage::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


#include "YQImage.moc"
