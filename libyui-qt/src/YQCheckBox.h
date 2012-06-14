/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

  File:	      YQCheckBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQCheckBox_h
#define YQCheckBox_h

#include <QCheckBox>
#include "YCheckBox.h"

class YQCheckBox : public QCheckBox, public YCheckBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQCheckBox( YWidget *	parent,
		const std::string &	label,
		bool 		checked );

    /**
     * Destructor.
     **/
    virtual ~YQCheckBox();

    /**
     * Get the current value:
     *
     * YCheckBox_on 		CheckBox is checked
     * YCheckBox_off 		CheckBox is unchecked
     *
     * YCheckBox_dont_care	tri-state: CheckBox is greyed out,
     *				neither checked nor unchecked
     *
     * Reimplemented from YCheckBox.
     **/
    virtual YCheckBoxState value();

    /**
     * Set the CheckBox value (on/off/don't care).
     *
     * Reimplemented from YCheckBox.
     **/
    virtual void setValue( YCheckBoxState state );

    /**
     * Change the label (the text) on the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Use a bold font.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setUseBoldFont( bool bold = true );

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
    void stateChanged ( int newState );

};

#endif // YQCheckBox_h
