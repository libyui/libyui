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

  File:	      YQApplication.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQApplication_h
#define YQApplication_h

#include <qobject.h>
#include "YApplication.h"


class YQApplication: public QObject, public YApplication
{
    Q_OBJECT
    
protected:

    friend class YQUI;
    
    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    YQApplication();

    /**
     * Destructor.
     **/
    virtual ~YQApplication();
};


#endif // YQApplication_h
