
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
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
  |*************************************************************************** */

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

  File:	      YQWizardButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQWizardButton_h
#define YQWizardButton_h

#include <qwidget.h>
#include "YQGenericButton.h"

using std::string;
class YQDialog;
class YQWizard;


class YQWizardButton : public YQGenericButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQWizardButton( YQWizard *		wizard,
		    QWidget *		buttonParent,
		    const string &	label );

    /**
     * Destructor.
     **/
    virtual ~YQWizardButton();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char *widgetClass() const { return "YQWizardButton"; }

    /**
     * Hide the associated QPushButton.
     **/
    void hide();

    /**
     * Show the associated QPushButton - not this widget itself (!).
     * This widget itself will never become visible.
     **/
    void show();

    /**
     * Returns 'true' if the associated QPushButton (!) is shown.
     **/
    bool isShown() const;
	
    /**
     * Returns 'true' if the associated QPushButton (!) is hidden.
     **/
    bool isHidden() const;
	
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
     * Returns the wizard this wizard button belongs to.
     **/
    YQWizard * wizard() const { return _wizard; }

signals:

    /**
     * Emitted when the button is clicked or otherwise activated.
     **/
    void clicked();

    
private:

    YQWizard * _wizard;
};

#endif // YQWizardButton_h
