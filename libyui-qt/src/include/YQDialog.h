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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQDialog_h
#define YQDialog_h

#include <qwidget.h>
#include "YDialog.h"


class YQPushButton;
class QFrame;


class YQDialog : public QWidget, public YDialog
{
    Q_OBJECT

public:
    /**
     * Constructor: Constructor.
     */
    YQDialog( YWidgetOpt &	opt,
	      QWidget *		_qt_parent	= 0,
	      bool		default_size	= false );

    /**
     * Destructor: Cleans up.
     */
    ~YQDialog();

    /**
     * Makes this dialog	 active or inactive
     */
    void activate(bool active);

    /**
     * Interited from QDialog: The window was closed via the window
     * manager close button.
     */
    void closeEvent( QCloseEvent * ev);

    /**
     * Returns the nice size of this dialog (may depend on -geometry)
     */
    long nicesize(YUIDimension dim);

    /**
     * Returns the size of (artificial) window manager decorations in
     * the requested dimension (YD_HORIZ or YD_VERT), depending on the
     * value of YDialog::isDecorated().
     */
    long decorationWidth(YUIDimension dim);

    /**
     * Return this dialog's (first) default button or 0 if none
     **/
    YQPushButton * findDefaultButton();

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Sets the new size of the widget.
     */
    void setSize(long newWidth, long newHeight);

    /**
     * Returns whether or not the user has resized this dialog.
     **/
    bool userResized() { return _userResized; }

    /**
     * Returns the button that has the keyboard focus or 0 if no button has
     * the keyboard focus.
     **/
    YQPushButton * focusButton() const { return _focusButton; }

    /**
     * Returns the dialog's default button - the button that is activated with
     * [Return] if no button has the keyboard focus.
     **/
    YQPushButton * defaultButton() const { return _defaultButton; }

    /**
     * Notification that a button loses the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they lose focus so
     * the dialog can keep track of its focusButton.
     **/
    void losingFocus( YQPushButton * button );
    
    /**
     * Notification that a button gets the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they gain focus so
     * the dialog can keep track of its focusButton.
     **/
    void gettingFocus( YQPushButton * button );

    /**
     * Set the dialog's default button - the button that is activated with
     * [Return] if no other button has the keyboard focus.
     * 'newDefaultButton' may be 0 if the former default button is destroyed.
     **/
    void setDefaultButton( YQPushButton * newDefaultButton );

    /**
     * Ensure presence of no more than one single default button.
     **/
    void ensureOnlyOneDefaultButton();
    
    /**
     * Activate (i.e. click) this dialog's default button, if there is any.
     * Issue a warning to the log file if 'warn' is true.
     **/
    bool activateDefaultButton( bool warn = true );


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
     * Callback function that reports to the ui specific
     * widget that a child has been added.
     */
    void childAdded(YWidget * child);

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Inherited from QWidget.
     */
    void keyPressEvent( QKeyEvent * e );


    /**
     * Event handler for focusIn event.
     *
     * Inherited from QWidget.
     **/
    void focusInEvent( QFocusEvent * event);

    /**
     * Event handler for window resize.
     *
     * Inherited from QWidget.
     */
    void resizeEvent ( QResizeEvent * ev );


    /**
     * Inherited from QWidget: Called when the dialog is shown.
     **/
    void show();


    //
    // Data members
    //

    QFrame *		_qFrame;

    bool  		_userResized;
    QSize 		_userSize;

    YQPushButton * 	_focusButton;
    YQPushButton * 	_defaultButton;
};


#endif // YQDialog_h

