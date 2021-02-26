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
#include <ycp/YCPString.h>

#include "YQGenericButton.h"


class YQDialog;
class YQWizard;


class YQWizardButton : public YQGenericButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQWizardButton( YQWizard *		wizard,
		    YQDialog *		dialog,
		    QWidget *		buttonParent,
		    const YCPString & 	label,
		    const YCPValue & 	id );

    /**
     * Destructor.
     **/
    virtual ~YQWizardButton();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     */
    virtual char *widgetClass() { return "YQWizardButton"; }

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
     * Preferred size of the widget.
     * Reimplemented from YWidget.
     * Does nothing since this widget has no visual representation.
     */
    long nicesize( YUIDimension dim ) { return 0; }

    /**
     * Sets the new size of the widget.
     * Reimplemented from YWidget.
     * Does nothing since this widget has no visual representation.
     */
    void setSize( long newWidth, long newHeight ) {}

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
