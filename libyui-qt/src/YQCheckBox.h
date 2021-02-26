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
		const string &	label,
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
    virtual void setLabel( const string & label );

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
