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

  File:	      YQGenericButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQGenericButton_h
#define YQGenericButton_h

#include <qwidget.h>
#include <ycp/YCPString.h>

#include "YPushButton.h"


class QPushButton;
class QObject;
class YQDialog;

/**
 * Abstract base class for push button and similar widgets -
 * all that can become a YQDialog's "default button".
 **/
class YQGenericButton : public QWidget, public YPushButton
{
    Q_OBJECT
    
protected:
    
    /**
     * Constructor.
     */
    YQGenericButton( QWidget *			parent,
		     YQDialog *			dialog,
		     const YWidgetOpt & 	opt,
		     YCPString 			label );

public:
    
    /**
     * Destructor.
     **/
    virtual ~YQGenericButton();

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. Only enabled widgets can take user input.
     */
    void setEnabling( bool enabled );
    
    /**
     * Returns 'true' if this button is enabled, 'false' otherwise.
     **/
    bool isEnabled() const;

    /**
     * Changes the label (the text) of the button.
     */
    void setLabel( const QString & label );

    /**
     * Changes the label (the text) of the button.
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
     * Returns the button's text ( label ) - useful for log messages etc.
     **/
    QString text() const;
    
    /**
     * Returns the internal Qt PushButton.
     **/
    QPushButton * qPushButton() const { return _qPushButton; }

    /**
     * Returns the internal parent dialog.
     **/
    YQDialog * yQDialog() const { return _dialog; }

    
public slots:

    /**
     * Activate (animated) this button.
     **/
    void activate();

    
protected:

    /**
     * Set the corresponding QPushButton.
     **/
    void setQPushButton( QPushButton * pb );
    
    /**
     * Redirect events from the _qPushButton member to this object.
     *
     * Overwritten from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * event );


private:
    
    // Data members

    YQDialog *		_dialog;
    QPushButton *	_qPushButton;
    bool		_isDefault;
};

#endif // YQGenericButton_h
