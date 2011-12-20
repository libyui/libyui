
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

  File:	      YQAlignment.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQAlignment_h
#define YQAlignment_h

#include <qwidget.h>

#include "YAlignment.h"

class QWidget;

class YQAlignment : public QWidget, public YAlignment
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQAlignment( YWidget *		parent,
		 YAlignmentType 	horAlign,
		 YAlignmentType 	vertAlign );

    /**
     * Constructor for cases where the YWidget parent's widgetRep() isn't the
     * QWidget parent, for example in compound widgets like YQWizard where it
     * makes sense to pass a child of the YQWizard (the client area) as the
     * QWidget parent.
     *
     * Use this only if you know very well what you are doing.
     **/
    YQAlignment( YWidget *		yParent,
		 QWidget * 		qParent,
		 YAlignmentType 	horAlign,
		 YAlignmentType 	vertAlign );

    /**
     * Move the child widget to a new position.
     *
     * Implemented from YAlignment.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY );

    /**
     * Enable or disable this widget and its child.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Set the background pixmap.
     *
     * Reimplemented from YAlignment.
     **/
    virtual void setBackgroundPixmap( const string & pixmapFileName );

};


#endif // YQAlignment_h
