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

#include <qtextbrowser.h>
#include <ycp/YCPString.h>

#include "YRichText.h"


class YQRichText : public QTextBrowser, public YRichText
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

    
protected:
    
    /**
     * Inherited from QTextBrowser: Set the current document to 'name'.
     **/
    void setSource( const QString & name );
    

    //
    // Data members
    //
    
    bool _shrinkable;
};


#endif // YQRichText_h
