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
#include "YUIInterpreter.h"
#include "YUIQt.h"


class YUIQt;

class YQImage : public QLabel, public YImage
{
    Q_OBJECT

public:
    /**
     * Creates a new image widget from a set of predefined
     * images.
     */
    YQImage(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, YUIInterpreter::ImageType img);

    /**
     * Creates a new image widget from a byteblock
     */
    YQImage(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, const YCPByteblock & byteblock);

    /**
     * Creates a new image widget from a image file name
     */
    YQImage(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, const YCPString & file_name);

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize(YUIDimension dim);

    /**
     * Sets the new size of the widget.
     */
    void setSize(long newwidth, long newheight);

private:

    /**
     * Common init method for all constructors
     **/
    void init( YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt );

    /**
     * Set the pixmap contents. That weird name is to prevent overwriting a
     * QLabel virtual method of the same name.  
     */
    void yqSetPixmap( const QPixmap &pixmap );

    
    bool zeroWidth;	// override nice width  with 0
    bool zeroHeight;	// override nice height with 0
    bool tiled;		// tile image (repeat endlessly)
    bool animated;	// animation, no static image

    int pixmapWidth;	// pixmap or movie width
    int pixmapHeight;	// pixmap or movie height
};

#endif // YQImage_h
