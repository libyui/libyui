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
class QMouseEvent;


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

    /**
     * Returns the thickness (base to point) of the small triangles next to the
     * spacing between individual segments. The base length of those small
     * triangles is double this value minus one. A value <1 indicates no
     * such triangles will be drawn.
     **/
    int triThickness() const { return _triThickness; }

    /**
     * Set the thickness (base to point) of the small triangles next to the
     * spacing between individual segments. The base length of those small
     * triangles is double this value minus one.
     * 
     * Use -1 to switch off those triangles - in which case triSpacing (see
     * below) will be set to 0.
     *
     * This call does not trigger a screen update.
     **/
    void setTriThickness( int value );

    /**
     * Returns the spacing between the segment indicators and the small
     * triangles next to the spacing between segments.
     **/
    int triSpacing() const { return _triSpacing; }

    /**
     * Sets the spacing between the segment indicators and the small
     * triangles next to the spacing between segments.
     **/
    void setTriSpacing( int value ) { _triSpacing = value; }

    /**
     * Returns "true" if the segments of this widget are triangular shaped.
     **/
    bool triangularShaped() const { return _triangularShaped; }

    /**
     * Set triangular shape (true) or normal rectangular shape.
     * This call does not trigger a screen update.
     **/
    void setTriangularShaped( bool triangular = true )
	{ _triangularShaped = triangular; }

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
     * Mouse double click -
     *
     * reimplemented from QWidget.
     **/
    virtual void mouseDoubleClickEvent ( QMouseEvent * e );
    
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

    /**
     * Draw markers between segments (or beside that spacing).
     **/
    void drawMarkers( QPainter & painter, int offset, int thickness );


private:

    bool	_triangularShaped;
    int		_margin;
    int 	_spacing;
    int 	_segmentMinLength;
    int 	_triThickness;
    int		_triSpacing;
};


#endif // YQMultiProgressMeter_h
