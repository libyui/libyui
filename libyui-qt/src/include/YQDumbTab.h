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

  File:	      YQDumbTab.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQDumbTab_h
#define YQDumbTab_h

#include <qvbox.h>

#include "YDumbTab.h"

class QTabBar;
class YQAlignment;
class YWidget;

class YQDumbTab : public QVBox, public YDumbTab
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQDumbTab( QWidget * 		parent,
	       const YWidgetOpt & 	opt 	);

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
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
     * Event filter - inherited from QWidget
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    /**
     * Notification that a child widget has been added.
     * Reimplemented from YContainerWidget.
     **/
    void addChild( YWidget * child );


public slots:

    /**
     * Send an event that the tab with the specified index is selected.
     **/
    void sendTabSelectedEvent( int index );

    /**
     * Adapt the size of the client area to fit in its current space.
     **/
    void resizeClientArea();


protected:

    /**
     * Add a tab header with the specified label.
     *
     * Reimplemented from YDumbTab.
     **/
    void addTab( const YCPString & label );

    /**
     * Get the index (0..n) of the currently selected tab.
     *
     * Reimplemented from YDumbTab.
     **/
    int getSelectedTabIndex();

    /**
     * Select a tab by index (0..n) and send an according event.
     *
     * Reimplemented from YDumbTab.
     **/
    void setSelectedTab( int index );


    //
    // Data members
    //

    QTabBar *		_tabBar;
    QVBox *		_clientArea;
    YQAlignment *	    _contents;
};

#endif // YQDumbTab_h
