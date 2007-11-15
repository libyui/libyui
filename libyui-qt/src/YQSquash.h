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

  File:	      YQSquash.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQSquash_h
#define YQSquash_h

#include <qwidget.h>

#include "YSquash.h"

class QWidget;

class YQSquash : public QWidget, public YSquash
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQSquash( YWidget * parent, bool horSquash, bool vertSquash );

    /**
     * Destructor.
     **/
    virtual ~YQSquash();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQSquash_h
