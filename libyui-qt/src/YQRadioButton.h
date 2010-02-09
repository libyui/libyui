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

  File:	      YQRadioButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQRadioButton_h
#define YQRadioButton_h

#include "YRadioButton.h"
#include <QRadioButton>

using std::string;


class YQRadioButton : public QRadioButton, public YRadioButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQRadioButton( YWidget *		parent,
		   const string &	label,
		   bool 		checked );

    /**
     * Return the "checked" state of the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual bool value();

    /**
     * Set the "checked" state of the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setValue( bool checked );

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

protected slots:
    /**
     * Triggered when the RadioButton is toggled.
     **/
    void changed( bool newState );

    /**
     * Redirect events to this object.
     **/
    bool eventFilter( QObject * obj, QEvent * event );

};

#endif // YQRadioButton_h
