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

  File:	      YQDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQDialog_h
#define YQDialog_h

#include <qwidget.h>
#include "YDialog.h"


class YQGenericButton;
class YQWizard;


class YQDialog : public QWidget, public YDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * 'dialogType' is one of YMainDialog or YPopupDialog.
     *
     * 'colorMode' can be set to YDialogWarnColor to use very bright "warning"
     * colors or YDialogInfoColor to use more prominent, yet not quite as
     * bright as "warning" colors. Use both only very rarely.
     **/
    YQDialog( QWidget *		qParent,
	      YDialogType 	dialogType,
	      YDialogColorMode	colorMode = YDialogNormalColor );

protected:

    /**
     * Destructor.
     * Don't delete a dialog directly, use YDialog::deleteTopmostDialog().
     **/
    virtual ~YQDialog();

public:
    /**
     * Makes this dialog	 active or inactive
     **/
    void activate( bool active );

    /**
     * Interited from QDialog: The window was closed via the window
     * manager close button.
     **/
    void closeEvent( QCloseEvent * ev );

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
     * Return this dialog's (first) default button or 0 if none
     **/
    YQGenericButton * findDefaultButton();

    /**
     * Returns whether or not the user has resized this dialog.
     **/
    bool userResized() { return _userResized; }

    /**
     * Returns the button that has the keyboard focus or 0 if no button has
     * the keyboard focus.
     **/
    YQGenericButton * focusButton() const { return _focusButton; }

    /**
     * Returns the dialog's default button - the button that is activated with
     * [Return] if no button has the keyboard focus.
     **/
    YQGenericButton * defaultButton() const { return _defaultButton; }

    /**
     * Notification that a button loses the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they lose focus so
     * the dialog can keep track of its focusButton.
     **/
    void losingFocus( YQGenericButton * button );

    /**
     * Notification that a button gets the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they gain focus so
     * the dialog can keep track of its focusButton.
     **/
    void gettingFocus( YQGenericButton * button );

    /**
     * Set the dialog's default button - the button that is activated with
     * [Return] if no other button has the keyboard focus.
     * 'newDefaultButton' may be 0 if the former default button is destroyed.
     **/
    void setDefaultButton( YQGenericButton * newDefaultButton );

    /**
     * Ensure presence of no more than one single default button.
     **/
    void ensureOnlyOneDefaultButton();

    /**
     * Activate ( i.e. click ) this dialog's default button, if there is any.
     * Issue a warning to the log file if 'warn' is true.
     **/
    bool activateDefaultButton( bool warn = true );

    /**
     * Find the first wizard in that dialog, if there is any.
     * Returns 0 if there is none.
     **/
    YQWizard * findWizard() const;

    /**
     * Find a wizard button that would make sense as a default button.
     * Return 0 if none can be found.
     **/
    YQGenericButton * wizardDefaultButton( YQWizard * wizard ) const;

    /**
     * Center a dialog relative to 'parent'.
     *
     * If 'parent' is 0, the dialog is centered relative to the application's
     * main widget. If 'dialog' is the main widget and 'parent' is 0, the
     * dialog is centered relative to the desktop.
     **/
    static void center( QWidget * dialog, QWidget * parent = 0 );


protected:

    /**
     * Return the (first) default button between 'begin' and 'end'
     * or 0 if there is none.
     **/
    YQGenericButton * findDefaultButton( YWidgetListConstIterator begin,
					 YWidgetListConstIterator end ) const;

    /**
     * Return the (first) wizard widget between 'begin' and 'end'
     * or 0 if there is none.
     **/
    YQWizard * findWizard( YWidgetListConstIterator begin,
			   YWidgetListConstIterator end ) const;

    /**
     * Helper function for ensureOnlyOneDefaultButton():
     * Recursively find all normal and wizard buttons between 'begin' and 'end'
     * and make sure that no more than one button is marked as default.
     * Return (the first) wizard widget found on the way.
     **/
    YQWizard * ensureOnlyOneDefaultButton( YWidgetListConstIterator begin,
					   YWidgetListConstIterator end );

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Inherited from QWidget.
     **/
    void keyPressEvent( QKeyEvent * e );


    /**
     * Event handler for focusIn event.
     *
     * Inherited from QWidget.
     **/
    void focusInEvent( QFocusEvent * event );

    /**
     * Event handler for window resize.
     *
     * Inherited from QWidget.
     **/
    void resizeEvent ( QResizeEvent * ev );


    /**
     * Inherited from QWidget: Called when the dialog is shown.
     **/
    void show();


    //
    // Data members
    //

    bool  		_userResized;
    QSize 		_userSize;

    YQGenericButton * 	_focusButton;
    YQGenericButton * 	_defaultButton;
};


#endif // YQDialog_h

