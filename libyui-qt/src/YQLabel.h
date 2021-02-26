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

  File:	      YQLabel.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQLabel_h
#define YQLabel_h

#include <qlabel.h>
#include "YLabel.h"


class YQLabel : public QLabel, public YLabel
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQLabel( YWidget * 		parent,
	     const string &	text,
	     bool 		isHeading	= false,
	     bool 		isOutputField	= false );

    /**
     * Destructor.
     **/
    virtual ~YQLabel();

    /**
     * Set the text the widget displays.
     *
     * Reimplemented from YLabel.
     **/
    virtual void setText( const string & newText );

    /**
     * Switch bold font on or off.
     *
     * Reimplemented from YLabel.
     **/
    virtual void setUseBoldFont( bool bold );
    
    /**
     * Set enabled / disabled state.
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
};


#endif // YQLabel_h
