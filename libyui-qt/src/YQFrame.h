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

  File:	      YQFrame.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQFrame_h
#define YQFrame_h


#include "YFrame.h"
#include <QGroupBox>

class QWidget;

class YQFrame : public QGroupBox, public YFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQFrame( YWidget * 		parent,
	     const string &	label );

    /**
     * Destructor.
     **/
    virtual ~YQFrame();

    /**
     * Change the frame label.
     **/
    virtual void setLabel( const string & newLabel );

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
};


#endif // YQFrame_h
