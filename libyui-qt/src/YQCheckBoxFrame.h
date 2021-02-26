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

  File:	      YQCheckBoxFrame.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQCheckBoxFrame_h
#define YQCheckBoxFrame_h

#include <qgroupbox.h>
#include "YCheckBoxFrame.h"


class QWidget;
class QCheckBox;

class YQCheckBoxFrame : public QGroupBox, public YCheckBoxFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQCheckBoxFrame( YWidget * 		parent,
		     const string &	label,
		     bool 		checked );

    /**
     * Change the label text on the CheckBoxFrame.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual void setLabel( const string & label );

    /**
     * Check or uncheck the CheckBoxFrame's check box.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual void setValue( bool isChecked );

    /**
     * Get the status of the CheckBoxFrame's check box.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual bool value();

    /**
     * Set enabled / disabled state.
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
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();


private slots:

    /**
     * Triggered when the on/off status is changed
     **/
    void stateChanged( bool newState );


protected:

    /**
     * Reimplemented from QGroupBox to prevent QGroupBox from disabling
     * children according to the check box status as the children are inserted.
     **/
    virtual void childEvent( QChildEvent * );

    virtual bool event(QEvent *e);
};


#endif // YQCheckBoxFrame_h
