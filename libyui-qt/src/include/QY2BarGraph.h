/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt widgets		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2BarGraph.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/

// -*- c++ -*-


#ifndef QY2BarGraph_h
#define QY2BarGraph_h

#include <qframe.h>
#include <qarray.h>
#include <vector>

using std::vector;


class QY2BarGraph : public QFrame
{
    Q_OBJECT

public:

    QY2BarGraph( QWidget * parent=0, const char * name=0 );

    /**
     * Add one segment to the bar graph.
     */
    void addSegment( const QString &label=QString(), int value=0 );

    /**
     * Set the number of segments.
     * Initialize all values and labels to zero values.
     */
    void setSegments( int segmentCount );

    /**
     * Set the value of segment no. segmentNo (beginning with 0)
     * to value newValue.
     */
    void setValue( int segmentNo, int newValue );

    /**
     * Set the label of segment no. segmentNo (beginning with 0)
     * to newLabel. May contain newlines or %d for a reference to the current
     * value.
     */
    void setLabel( int segmentNo, const QString &newLabel );

    /**
     * Return the current number of segments.
     */
    int segments() const { return _values.size(); }

    /**
     * Return the current value of segment no. segmentNo.
     */
    int value( int segmentNo ) const;

    /**
     * Return the label of segment no. segmentNo.
     * Any occurence of "%d" etc. will not be expanded here but returned as
     * passed to setLabel().
     */
    const QString label( int segmentNo ) const;

    /**
     * Return the sum of all values,
     */
    int sumValues() const;


    /**
     * Choose a background color suitable for segment no. segmentNo.
     * Only a defined number of different colors are available.
     * After that, previously used colors will be recycled.
     */
    const QColor segmentBackgroundColor( int segmentNo );

    /**
     * Choose a foreground color suitable for segment no. segmentNo.  that
     * results in good contrast to the corresponding background color
     * (see abore)
     */
    const QColor segmentForegroundColor( int segmentNo );

    /**
     * Return a suitable size for the widget.
     */
    virtual QSize sizeHint() const;

protected:

    /**
     * Draw the graph bar contents.
     * Inherited from QFrame.
     */
    virtual void drawContents( QPainter * );


    /**
     * Sanity check for segment index.
     * Returns true if invalid, false if OK.
     */
    bool index_invalid( int i, const char * function_name ) const;

private:

    vector<int>		_values;
    vector<QString>	_labels;
};

#endif // QY2BarGraph_h
