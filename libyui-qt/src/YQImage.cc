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
#include <QIcon>
//Added by qt3to4:
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

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
	    y2error( "Couldn't load animation from %s", imageFileName().c_str() );
	}
	else
	{
	    y2debug( "Loading animation from %s", imageFileName().c_str() );
	    QLabel::setMovie( &movie );
	}
    }
    else
    {
	QPixmap pixmap( fromUTF8( imageFileName() ) );

	if ( pixmap.isNull() )
	{
	    y2error( "Couldn't load pixmap from %s", imageFileName().c_str() );
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
	
	    y2debug( "Loading image from %s (%d x %d)",
		     imageFileName().c_str(),
		     pixmap.size().width(),
		     pixmap.size().height() );
	    
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
   qDebug("setEnabled %d", enable);
   if (enable)
      setImage( imageFileName(), animated() );
   else {
      // Trigger image re-display
      QPixmap pixmap( fromUTF8( imageFileName() ) );
      QIcon icon(pixmap);
      QLabel::setPixmap( icon.pixmap( pixmap.size(), QIcon::Disabled, QIcon::Off) );
   }
}

#include "YQImage.moc"
