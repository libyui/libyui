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

  File:	      YQPushButton.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPushButton_h
#define YQPushButton_h

#include <qwidget.h>
#include <ycp/YCPString.h>

#include "YPushButton.h"


class QPushButton;
class YQDialog;

class YQPushButton : public QWidget, public YPushButton
{
    Q_OBJECT
    
public:
    /**
     * Constructor.
     */
    YQPushButton( 		  QWidget * 	parent,
		  YQDialog *	dialog,
		  YWidgetOpt & 	opt,
		  YCPString 	label );

    /**
     * Destructor.
     **/
    virtual ~YQPushButton();

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling( bool enabled );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     */
    void setSize(long newWidth, long newHeight);

    /**
     * Changes the label of the button
     */
    void setLabel( const YCPString & label );

    /**
     * Show this button as the dialog's default button. The button never calls
     * this by itself - the parent dialog is responsible for that.
     **/
    void showAsDefault( bool show = true );

    /**
     * Returns 'true' if this button is shown as a default button - which may
     * mean that this really is the dialogs's default button or it is the
     * dialog's focus button (a button that currently has the keyboard focus).
     *
     * Don't confuse this with isDefault()!
     **/
    bool isShownAsDefault() const;

    /**
     * Returns 'true' if this is the dialog's default button.
     *
     * Don't confuse this with isShownAsDefault()!
     **/
    bool isDefault() const { return _isDefault; }

    /**
     * Set this button's default button state.
     **/
    void setDefault( bool def = true ) { _isDefault = def; }

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Set this button's icon.
     * Inherited from YPushButton.
     **/
    virtual void setIcon( const YCPString & icon_name );

    /**
     * Returns the button's text (label) - useful for log messages etc.
     **/
    QString text() const;
    
    /**
     * Returns the internal Qt PushButton.
     **/
    QPushButton * qPushButton() const { return _qPushButton; }

    
public slots:

    /**
     * Triggered when the button is activated (clicked or via keyboard). Informs
     * the ui about this.
     */
    void hit();

    /**
     * Activate (animated) this button.
     **/
    void activate();

    
protected:

    /**
     * Redirect events from the _qPushButton member to this object.
     *
     * Overwritten from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * event );


    // Data members

    YQDialog *		_dialog;
    QPushButton *	_qPushButton;
    bool		_isDefault;

};

#endif // YQPushButton_h
