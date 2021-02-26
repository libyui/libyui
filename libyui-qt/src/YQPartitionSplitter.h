/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  File:	      YQPartitionSplitter.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPartitionSplitter_h
#define YQPartitionSplitter_h

#include "qwidget.h"
#include "YPartitionSplitter.h"

class YLayoutBox;
class YQBarGraph;
class YQIntField;
class YQSlider;


class YQPartitionSplitter : public QWidget, public YPartitionSplitter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQPartitionSplitter( YWidget *	parent,
			 int 		usedSize,
			 int 		freeSize,
			 int 		newPartSize,
			 int 		minNewPartSize,
			 int 		minFreeSize,
			 const string &	usedLabel,
			 const string &	freeLabel,
			 const string &	newPartLabel,
			 const string &	freeFieldLabel,
			 const string &	newPartFieldLabel );

    /**
     * Destructor.
     **/
    ~YQPartitionSplitter();

    /**
     * Return the value (the size of the new partition).
     *
     * Implemented from YPartitionSplitter.
     **/
    virtual int value();
    
    /**
     * Set the value (the size of the new partition).
     *
     * Implemented from YPartitionSplitter.
     **/
    virtual void setValue( int newValue );
    
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

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


public slots:
    /**
     * Slot for setting the free size.
     **/
    void setFreeSizeSlot( int newFreeSize );

    /**
     * Slot for setting the new size.
     **/
    void setNewPartSizeSlot(  int newNewSize );


protected:

    enum
    {
	usedSegment    = 0,
	freeSegment    = 1,
	newPartSegment = 2
    };

    
    //
    // Widgets
    //

    YLayoutBox *	_vbox;
    YQBarGraph *	    _barGraph;
    YLayoutBox *	    _hbox;
    YQSlider   *		_freeSizeSlider;
    YQIntField  *		_newPartField;

};


#endif // YQPartitionSplitter_h
