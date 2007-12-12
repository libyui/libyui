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

  File:	      YQMultiLineEdit.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQMultiLineEdit_h
#define YQMultiLineEdit_h

#include <QFrame>
#include "YMultiLineEdit.h"

class YQWidgetCaption;
class QTextEdit;


/**
 * MultiLineEdit - an input area for multi-line text.
 **/
class YQMultiLineEdit : public QFrame, public YMultiLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQMultiLineEdit( YWidget * parent, const string & label );

    /**
     * Destructor.
     **/
    virtual ~YQMultiLineEdit();

    /**
     * Get the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Implemented from YMultiLineEdit.
     **/
    virtual string value();

    /**
     * Set the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Implemented from YMultiLineEdit.
     **/
    virtual void setValue( const string & text );

    /**
     * Set the label (the caption above the MultiLineEdit).
     *
     * Reimplemented from YMultiLineEdit.
     **/
    virtual void setLabel( const string & label );

    /**
     * Set the maximum input length, i.e., the maximum number of characters the
     * user can enter. -1 means no limit.
     *
     * Reimplemented from YMultiLineEdit.
     **/
    virtual void setInputMaxLength( int numberOfChars );

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
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


protected slots:

    /**
     * Triggered when the text changes.
     **/
    void changed();

    /**
     * Enforce the maximum input length: If the text becomes too long, remove
     * the just-entered character at the current cursor position.
     *
     * Note that this is a lot more user friendly than silently truncating at
     * the end of the text: In the latter case, chances are that the user never
     * gets to know that text was truncated. Removing the just-typed character
     * OTOH is something he will notice very quickly.
     **/
    void enforceMaxInputLength();


protected:

    YQWidgetCaption *	_caption;
    QTextEdit *		_qt_textEdit;
};


#endif // YQMultiLineEdit_h
