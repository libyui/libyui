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

  File:	      YQLogView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQLogView_h
#define YQLogView_h

#include <QFrame>
#include <QTextEdit>

#include "YLogView.h"

class YQWidgetCaption;


class YQLogView : public QFrame, public YLogView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQLogView( YWidget * 	parent,
	       const string & 	label,
	       int 		visibleLines,
	       int 		maxLines );

    /**
     * Destructor.
     **/
    virtual ~YQLogView();

protected:
    /**
     * Display the part of the log text that should be displayed.
     * 'text' contains the last 'visibleLines()' lines.
     * This is called whenever the log text changes. Note that the text might
     * also be empty, in which case the displayed log text should be cleared.
     *
     * Implemented from YLogView.
     **/
    virtual void displayLogText( const string & text );

public:

    /**
     * Set the label (the caption above the log text).
     *
     * Reimplemented from YLogView.
     **/
    virtual void setLabel( const string & label );

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


protected:

    YQWidgetCaption *	_caption;
    QTextEdit *	_qt_text;
};


#endif // YQLogView_h
