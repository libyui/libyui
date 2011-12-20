
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

  File:	      YQWidgetCaption.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQWidgetCaption_h
#define YQWidgetCaption_h

#include <qlabel.h>
#include <string>
using std::string;


/**
 * Helper class for captions (labels) above a widget: Takes care of hiding
 * itself when its text is empty and showing when its text becomes non-empty.
 **/
class YQWidgetCaption: public QLabel
{
    Q_OBJECT

public:

    /**
     * Constuctors.
     *
     * If the text is empty, the widget will be created, but hidden right
     * away. It can later be made visible again with setText() with a non-empty
     * text.
     **/
    YQWidgetCaption( QWidget * parent, const string  & text );
    YQWidgetCaption( QWidget * parent, const QString & text );

    /**
     * Destructor.
     **/
    virtual ~YQWidgetCaption();

    /**
     * Change the text and handle visibility:
     * If the new text is empty, hide this widget.
     * If the new text is non-empty, show this widget.
     **/
    virtual void setText ( const string  & newText );
    virtual void setText ( const QString & newText );

private:

    /**
     * Make the widget visible if the text is non-empty or hide it if it is
     * empty. 
     **/
    void handleVisibility( const string  & text );
    void handleVisibility( const QString & text );
    void handleVisibility( bool textIsEmpty );
};


#endif // YQWidgetCaption_h
