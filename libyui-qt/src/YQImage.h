
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

  File:	      YQImage.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQImage_h
#define YQImage_h

#include <qlabel.h>
#include <qmovie.h>

#include "YImage.h"
#include "YUI.h"


class YQImage : public QLabel, public YImage
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * 'animated' indicates if 'imageFileName' is an animated image format
     * (e.g., MNG).
     **/
    YQImage( YWidget *		parent,
	     const string &	imageFileName,
	     bool		animated = false );

    /**
     * Destructor.
     **/
    virtual ~YQImage();

    /**
     * Set and display a new image.
     *
     * Reimplemented from YImage.
     **/
    virtual void setImage( const string & imageFileName, bool animated = false );
    
    /**
     * Make the image fit into the available space.
     *
     * Reimplemented from YImage.
     **/
    virtual void setAutoScale( bool autoScale = true );

    /**
      * if false, the image will be displayed in gray
      */
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

protected:

    int		_pixmapWidth;
    int 	_pixmapHeight;
};

#endif // YQImage_h
