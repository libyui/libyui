
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

  File:	      YQDownloadProgress.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQDownloadProgress_h
#define YQDownloadProgress_h

#include <QFrame>
#include "YDownloadProgress.h"

class YQWidgetCaption;
class QProgressBar;


class YQDownloadProgress : public QFrame, public YDownloadProgress
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQDownloadProgress( YWidget *	parent,
			const string & 	label,
			const string &	filename,
			YFileSize_t	expectedSize );

    /**
     * Destructor.
     **/
    virtual ~YQDownloadProgress();

    /**
     * Set the label (the text above the progress bar).
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setLabel( const string & label );

    /**
     * Set the name of a new file to monitor.
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setFilename( const string & filename );

    /**
     * Change the expected file size.
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setExpectedSize( YFileSize_t expectedSize );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
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


public slots:
    /**
     * Slot for polling and displaying the file size.
     **/
    void pollFileSize( void );


protected:

    YQWidgetCaption *	_caption;
    QProgressBar *	_qt_progressBar;
    QTimer *		_timer;
};


#endif // YQDownloadProgress_h
