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

  File:	      Y2CCQt.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/



#ifndef Y2CCQt_h
#define Y2CCQt_h

#include <stdio.h>
#include "YQUIComponent.h"
#include <Y2CCUI.h>

/**
 * Y2ComponentCreator that can create Qt user interfaces.
 *
 * A Y2ComponentCreator is an object that can create components.
 * It is given a component name and - if it knows how to create
 * such a component - returns a newly created component of this
 * type. The Y2CCQt can create components with the name "qt".
 */
class Y2CCQt : public Y2CCUI
{
public:
    /**
     * Constructor
     **/
    Y2CCQt() : Y2CCUI() {};

    /**
     * Returns true, since the qt component is a YaST2 server.
     **/
    bool isServerCreator() const { return true; };

    /**
     * Creates a new Qt UI component.
     **/
    Y2Component * create( const char * name ) const
    {
	if ( ! strcmp( name, "qt") )
	{
	    Y2Component * ret = YUIComponent::uiComponent ();
	    
    	    if ( ! ret || ret->name () != name )
    	    {
		ret = new YQUIComponent();
	    }
	    return ret;
	}
	else
	    return 0;
    }
};

#endif	// ifndef Y2CCQt_h


// EOF
