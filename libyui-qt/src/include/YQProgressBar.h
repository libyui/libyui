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

  File:	      YQProgressBar.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQProgressBar_h
#define YQProgressBar_h

#include <qvbox.h>
#include <ycp/YCPString.h>

#include "YProgressBar.h"


class QString;
class QLabel;
class QProgressBar;

class YQProgressBar : public QVBox, public YProgressBar
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQProgressBar( QWidget *		parent,
		   YWidgetOpt &		opt,
		   const YCPString  & 	label,
		   const YCPInteger & 	maxProgress,
		   const YCPInteger & 	initialProgress );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. Disabling a progress bar makes it grey
     */
    void setEnabling( bool enabled );

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

    /**
     * Change the ProgressBar label. Calls
     * YProgressBar::setLabel at the end.
     */
    void setLabel( const YCPString & label );

    /**
     * Change the progress. Calls
     * YProgressBar::setProgress at the end.
     */
    void setProgress( const YCPInteger & progress );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    
protected:

    //
    // Data members
    //
    
    QLabel *		_qt_label;
    QProgressBar *	_qt_progressbar;
};

#endif // YQProgressBar_h
