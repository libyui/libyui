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

  File:	      YQMultiProgressMeter.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQMultiProgressMeter_h
#define YQMultiProgressMeter_h


#include "YMultiProgressMeter.h"
#include <qwidget.h>
#include <qpainter.h>


class QPaintEvent;


class YQMultiProgressMeter : public QWidget, public YMultiProgressMeter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQMultiProgressMeter( QWidget * 		parent,
			  const YWidgetOpt & 	opt,
			  bool 			horizontal,
			  const YCPList &	maxValues );

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
     * Returns the margin around the widget contents.
     **/
    int margin() const { return _margin; }

    /**
     * Sets the margin around the widget contents.
     * Does not trigger an update.
     **/
    void setMargin( int value ) { _margin = value; }

    /**
     * Returns the spacing between segments in pixels.
     **/
    int spacing() const { return _spacing; }

    /**
     * Sets the spacing between segments in pixels.
     * Does not trigger an update.
     **/
    void setSpacing( int value ) { _spacing = value; }

    /**
     * Returns the minimal length of a segment in pixels.
     **/
    int segmentMinLength() const { return _segmentMinLength; }
    
    /**
     * Set the minimal length of a segment in pixels.
     **/
    void setSegmentMinLength( int val ) { _segmentMinLength = val; }

    
protected:

    /**
     * Perform a visual update on the screen.
     * Reimplemented from YMultiProgressMeter.
     */
    virtual void doUpdate();

    /**
     * Paint the widget's contents.
     * Reimplemented from QWidget.
     **/
    virtual void paintEvent ( QPaintEvent * );

    /**
     * Draw segment number 'segment' with pixel length 'length' from pixel
     * coordinate 'offset' on and fill it according to that segment's current
     * value. 'painter' is set up previously to take rotating into account
     * (horizontal / vertical).
     **/
    void drawSegment( int segment,
		      QPainter & painter,
		      int offset,
		      int length,
		      int thickness,
		      int indent );

    
private:

    int	_margin;
    int _spacing;
    int _segmentMinLength;
};


#endif // YQMultiProgressMeter_h
