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

// -*- c++ -*-

#ifndef YQPartitionSplitter_h
#define YQPartitionSplitter_h

#include <qslider.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <ycp/YCPString.h>

#include "QY2BarGraph.h"
#include "YPartitionSplitter.h"


class YQPartitionSplitter : public QWidget, public YPartitionSplitter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQPartitionSplitter( QWidget *		parent,
			 YWidgetOpt &		opt,
			 int 			usedSize,
			 int 			freeSize,
			 int 			newPartSize,
			 int 			minNewPartSize,
			 int 			minFreeSize,
			 const YCPString &	usedLabel,
			 const YCPString &	freeLabel,
			 const YCPString &	newPartLabel,
			 const YCPString &	freeFieldLabel,
			 const YCPString &	newPartFieldLabel );

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
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Change the value.
     * Calls YPartitionSplitter::setProgress() internally.
     * Inherited from YPartitionSplitter.
     */
    void setValue( int newValue );

public slots:
    /**
     * Slot for setting the free size.
     */
    void setFreeSizeSlot( int newFreeSize );

    /**
     * Slot for setting the new size.
     */
    void setNewPartSizeSlot(  int newNewSize );


protected:

    /*
     * Widgets
     *
     * Indentation indicates widget hierarchy
     */

    QVBox *		_vbox;
    QY2BarGraph *		_barGraph;
    QHBox *			_labels_hbox;
    QLabel *				_qt_freeFieldLabel;
    QLabel *				_qt_newPartFieldLabel;
    QHBox *			_fields_hbox;
    QSpinBox *				_qt_freeSizeField;
    QSlider *				_qt_freeSizeSlider;
    QSpinBox *				_qt_newPartSizeField;

    bool _countShowDelta;
};


#endif // YQPartitionSplitter_h
