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

  File:	      YQImage.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQImage_h
#define YQImage_h

#include <qlabel.h>
#include <ycp/YCPString.h>

#include "YImage.h"
#include "Y2UIComponent.h"


class YQImage : public QLabel, public YImage
{
    Q_OBJECT

public:
    /**
     * Constructors
     */
    YQImage( QWidget * parent, YWidgetOpt & opt, Y2UIComponent::ImageType img );
    YQImage( QWidget * parent, YWidgetOpt & opt, const YCPByteblock & byteblock );
    YQImage( QWidget * parent, YWidgetOpt & opt, const YCPString & file_name );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     */
    void setSize( long newWidth, long newHeight );

protected:

    /**
     * Common init method for all constructors
     **/
    void init( QWidget * parent, YWidgetOpt & opt );

    /**
     * Set the pixmap contents. That weird name is to prevent overwriting a
     * QLabel virtual method of the same name.  
     */
    void yqSetPixmap( const QPixmap &pixmap );


    //
    // Data members
    //
    
    bool _zeroWidth;	// override nice width  with 0
    bool _zeroHeight;	// override nice height with 0
    bool _tiled;	// tile image ( repeat endlessly )
    bool _animated;	// animation, no static image

    int	_pixmapWidth;	// pixmap or movie width
    int _pixmapHeight;	// pixmap or movie height
};

#endif // YQImage_h
