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

  File:	      YQRichText.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQRichText_h
#define YQRichText_h

#include <qvbox.h>
#include <qtextbrowser.h>
#include <ycp/YCPString.h>

#include "YRichText.h"


class YQTextBrowser;

class YQRichText : public QVBox, public YRichText
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    YQRichText( QWidget * parent, YWidgetOpt & opt, const YCPString & text );

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
     * Change the RichText text.
     */
    void setText( const YCPString & RichText );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();


protected slots:

    /**
     * Notification that a hyperlink is clicked.
     **/
    void linkClicked( const QString & url );


protected:

    //
    // Data members
    //

    YQTextBrowser *	_textBrowser;
    bool 		_shrinkable;
};


/**
 * Helper class - needed to have the benefits of both QVBox as the base class
 * for YQRichText so uniform margins can be implemented (outside, not inside
 * the scroll window as QTextBrowser normally does it) and QTextBrowser's
 * featurs (enable hyperlinks) without removing the text each time the user
 * clicks.
 *
 * This class is required only to overwrite setSource().
 **/
class YQTextBrowser: public QTextBrowser
{
    Q_OBJECT


public:

    /**
     * Constructor
     **/
    YQTextBrowser( QWidget * parent = 0 ):
	QTextBrowser( parent ) {}

public slots:

    /**
     * Get the document pointed to by a hyperlink.
     *
     * Reimplemented from QTextBrowser to avoid having an empty text each time
     * the user clicks on a hyperlink.
     **/
    virtual void setSource( const QString & name ) {}
};


#endif // YQRichText_h
