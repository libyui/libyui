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

  File:		YNCursesComponent.h

  Author:	Stefan Hundhammer <sh@suse.de>
  
  Maintainer:	Michael Andres <ma@suse.de>


/-*/

#ifndef YNCursesComponent_h
#define YNCursesComponent_h

#include <YUIComponent.h>
#include "YNCursesUI.h"


class YNCursesComponent :  public YUIComponent
{
public:

    YNCursesComponent(): YUIComponent() {}

    /**
     * Create a Qt UI. This is called from within setServerOptions().
     *
     * Reimplemented from YUIComponent.
     **/
    virtual YUI * createUI( bool withThreads )
    {
	return new YNCursesUI( withThreads );
    }

    /**
     * Returns the name of this YaST2 component.
     *
     * Reimplemented from YUIComponent.
     */
    virtual string name() const { return "ncurses"; }

};


#endif // YNCursesComponent_h
