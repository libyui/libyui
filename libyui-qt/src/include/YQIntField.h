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

  File:	      YQIntField.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQIntField_h
#define YQIntField_h

#include <qslider.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <ycp/YCPString.h>

#include "YIntField.h"


class QString;
class QLabel;
class QIntField;

class YQIntField : public QWidget, public YIntField
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQIntField( 		QWidget *		parent,
		YWidgetOpt &		opt,
		const YCPString &	label,
		int 			minValue,
		int 			maxValue,
		int 			initialValue );

    /**
     * Sets the widget's enabled state.
     * Inherited from YWidget.
     */
    void setEnabling(bool enabled);

    /**
     * Preferred size.
     * Inherited from YWidget.
     */
    long nicesize(YUIDimension dim);

    /**
     * Set the new size of the widget.
     * Inherited from YWidget.
     */
    void setSize(long newWidth, long newHeight);

    /**
     * Change the IntField label.
     * Calls YIntField::setLabel at the end.
     * Inherited from YIntField.
     */
    void setLabel( const YCPString & label );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Change the value.
     * Calls YIntField::setProgress at the end.
     * Inherited from YIntField.
     */
    void setValue( int newValue );

public slots:
    /**
     * Slot for setting the value.
     * This is needed separately from setValue() to avoid infinite loops via
     * ChangeWidget().
     */
    void setValueSlot( int newValue );


protected:

    QLabel *	_qt_label;
    QSpinBox *	_qt_spinbox;
    QVBox *	vbox;
    QHBox *	hbox;
};


#endif // YQIntField_h
