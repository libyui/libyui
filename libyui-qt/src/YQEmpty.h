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

  File:	      YQEmpty.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQEmpty_h
#define YQEmpty_h

#include <qwidget.h>
#include "YEmpty.h"


class YQEmpty : public QWidget, public YEmpty
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQEmpty( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQEmpty();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQEmpty_h
