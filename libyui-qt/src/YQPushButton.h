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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPushButton_h
#define YQPushButton_h

#include "YQGenericButton.h"


class YQDialog;

class YQPushButton : public YQGenericButton
{
    Q_OBJECT
    
public:
    /**
     * Constructor.
     **/
    YQPushButton( YWidget * 		parent,
		  const std::string & 	label );

    /**
     * Destructor.
     **/
    virtual ~YQPushButton();

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

    
public slots:

    /**
     * Triggered when the button is activated (clicked or via keyboard).
     * Notifies the the ui about this.
     **/
    void hit();
};

#endif // YQPushButton_h
