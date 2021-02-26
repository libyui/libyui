/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YQBarGraph.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQBarGraph_h
#define YQBarGraph_h

#include "qframe.h"
#include "qevent.h"
#include "qtooltip.h"
#include <yui/YBarGraph.h>
#include <map>

using namespace std;

class QPainter;


class YQBarGraph : public QFrame, public YBarGraph
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQBarGraph( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQBarGraph();

    /**
     * Perform a visual update on the screen.
     *
     * Implemented from YBarGraph.
     **/
    virtual void doUpdate();

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


protected:

    virtual bool event( QEvent * event );
    /**
     * Draw the contents.
     *
     * Reimplemented from QFrame.
     **/
    virtual void paintEvent( QPaintEvent * painter );

    /**
     * Return one from a set of default segment background colors.
     **/
    YColor defaultSegmentColor( unsigned index );

    /**
     * Return one from a set of default text colors. This text color is
     * guaranteed to contrast with the defaultSegmentColor with the same index.
     **/
    YColor defaultTextColor( unsigned index );

private:
    map <int, QString> toolTips ;

};


#endif // YQBarGraph_h
