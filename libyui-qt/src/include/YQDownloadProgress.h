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

// -*- c++ -*-

#ifndef YQDownloadProgress_h
#define YQDownloadProgress_h

#include <qvbox.h>
#include <ycp/YCPString.h>

#include "YDownloadProgress.h"


class QLabel;
class QProgressBar;

class YQDownloadProgress : public QVBox, public YDownloadProgress
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQDownloadProgress( 			QWidget *	parent,
			YWidgetOpt &	opt,
			const YCPString & label,
			const YCPString & filename,
			int 		expectedSize );

    /**
     * Sets the widget's enabled state.
     * Inherited from YWidget.
     */
    void setEnabling( bool enabled );

    /**
     * Preferred size.
     * Inherited from YWidget.
     */
    long nicesize( YUIDimension dim );

    /**
     * Set the new size of the widget.
     * Inherited from YWidget.
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Change the label.
     * Calls YDownloadProgress::setLabel at the end.
     * Inherited from YDownloadProgress.
     */
    void setLabel( const YCPString & label );


    /**
     * Change the expected file size.
     * Calls YDownloadProgress::setExpectedSize() at the end.
     * Inherited from YDownloadProgress.
     */
    void setExpectedSize( int expectedSize );


public slots:
    /**
     * Slot for polling and displaying the file size.
     */
    void pollFileSize( void );


protected:

    QLabel *		_qt_label;
    QProgressBar *	_qt_progress;
    QTimer *		timer;
};


#endif // YQDownloadProgress_h
