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

#include <qdialog.h>
#include <qframe.h>
#include "YDialog.h"

class YUIQt;
class YQPushButton;

class YQDialog : public QWidget, public YDialog
{
    Q_OBJECT

private:

    /**
     * Pointer to the YaST2 qt user interface. The widget
     * must access this when it handles an event during
     * the event loop of the ui. It then calls @ref YUIQt#returnNow
     * to tell the ui that it should leave the event loop.
     */
    YUIQt *yuiqt;

    /**
     * Frame around the dialog. Used only if the `decorated option is set.
     * Otherwise, this might be 0.
     */
    QFrame *qt_frame;

    /**
     * are penguins walking?
     */
    bool penguins;

public:
    /**
     * Constructor: Creates a new Qt dialog.
     */
    YQDialog( YUIQt *		yuiqt,
	      YWidgetOpt &	opt,
	      QWidget *		qt_parent	= 0,
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
    void closeEvent( QCloseEvent *ev);

    /**
     * Inherited from QDialog: Escape pressed.
     * The Qt UI simply ignores this.
     */
    void reject();

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
    YQPushButton *findDefaultButton();


    /**
     * Ensure presence of no more than one single default button
     **/
    void ensureOnlyOneDefaultButton();


    /**
     * Make the specified button the default button
     **/
    void makeDefaultButton( YQPushButton *new_default_button );

    
    /**
     * Activate (i.e. click) this dialog's default button, if there is any.
     * Issue a warning to the log file if 'warn' is true.
     **/
    bool activateDefaultButton( bool warn = true );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Sets the new size of the widget.
     */
    void setSize(long newwidth, long newheight);


    /**
     * Returns whether or not the user has resized this dialog.
     **/
    bool userResized() { return _userResized; }


protected:

    /**
     * Callback function that reports to the ui specific
     * widget that a child has been added. The default implementation
     * does nothing.
     */
    void childAdded(YWidget *child);

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Inherited from QWidget.
     */
    void keyPressEvent( QKeyEvent *e );


    /**
     * Event handler for focusIn event.
     *
     * Inherited from QWidget.
     **/
    void focusInEvent( QFocusEvent *event);
    
    /**
     * Event handler for window resize.
     *
     * Inherited from QWidget.
     */
    void resizeEvent ( QResizeEvent *ev );


    /**
     * Inherited from QWidget: Called when the dialog is shown.
     **/
    void show();

    //
    // Data members
    //
    
    bool  _userResized;
    QSize _userSize;
};


#endif // YQDialog_h

