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

  File:	      YQTextEntry.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQTextEntry_h
#define YQTextEntry_h

#include <qvbox.h>
#include <qlineedit.h>
#include <ycp/YCPString.h>

#include "YTextEntry.h"


class QString;
class QLabel;
class QLineEdit;
class QY2CharValidator;
class YQRawLineEdit;


class YQTextEntry : public QVBox, public YTextEntry
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQTextEntry( QWidget *		parent,
		 const YWidgetOpt &	opt,
		 const YCPString & 	label,
		 const YCPString & 	text );

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
    void setSize( long newWidth, long newHeight );

    /**
     * Set the text in the entry to a new value
     */
    void setText( const YCPString & text );

    /**
     * Gets the text currently entered in the text entry
     */
    YCPString getText();

    /**
     * Changes the label of the text entry.
     */
    void setLabel( const YCPString & label );

    /**
     * Change the valid input characters.
     *
     * Reimplemented from @ref YTextEntry.
     */
    void setValidChars( const YCPString & validChars );

    /**
     * Specify the amount of characters which can be inserted.
     *
     * Reimplemented from @ref YTextEntry.
     */
    void setInputMaxLength( const YCPInteger & numberOfChars );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Returns 'true' if a given text is valid according to ValidChars.
     **/
    bool isValidText( const QString & text ) const;


protected slots:
    /**
     * Triggered when the text in the TextEntry changes.
     * This _may_ be of interest to the module.
     */
    void changed( const QString & );

    /**
     * Display a warning that CapsLock is active:
     * Replace the label with "CapsLock!"
     **/
    void displayCapsLockWarning();

    /**
     * Clear the CapsLock warning: Restore old label
     **/
    void clearCapsLockWarning();


protected:

    //
    // Data members
    //

    QLabel *		_qt_label;
    YQRawLineEdit *	_qt_lineEdit;
    QY2CharValidator *	_validator;
    bool		_shrinkable;
    bool		_displayingCapsLockWarning;
};


/**
 * Helper class that can obtain the CapsLock status, too.
 * For some reason, Qt does not propagate that information from X11.
 **/
class YQRawLineEdit: public QLineEdit
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQRawLineEdit( QWidget * parent )
	: QLineEdit( parent )
	, _capsLockActive( false )
	{}

    /**
     * Destructor
     **/
    virtual ~YQRawLineEdit() {};

    /**
     * Check if CapsLock is active
     * (rather: was active at the time of the last key or focus event)
     **/
    bool isCapsLockActive() const { return _capsLockActive; }


signals:
    void capsLockActivated();
    void capsLockDeactivated();

protected:

    /**
     * X11 raw event handler. Propagates all events to the Qt event handlers,
     * but updates _capsLockActive for key events.
     *
     * Reimplemented from QWidget.
     **/
    bool x11Event( XEvent * event ) ;

private:

    bool _capsLockActive;
};

#endif // YQTextEntry_h
