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

/**
 * This file is part of YaST2.
 *
 * $Id$
 *
 * Author: Waldo Bastian <bastian@suse.de>
 * Maintainer: Stefan Hundhammer <sh@suse.de>
 *
 * Description: YaST2 Qt Component Creator
 *
 * Copyright:
 *
 * Licence:
 *
 */


#ifndef _Y2CCQt_h
#define _Y2CCQt_h

#include "Y2QtComponent.h"

/**
 * @short Y2ComponentCreator that can create Qt user interfaces
 * A Y2ComponentCreator is an object, that can create components.
 * It is given a component name and - if it knows how to create
 * such a component - returns a newly created component of this
 * type. The Y2CCQt can create components with the name "qt".
 */
class Y2CCQt : public Y2ComponentCreator
{
public:
    /**
     * Creates a Qt component creator
     */
    Y2CCQt() : Y2ComponentCreator(Y2ComponentBroker::BUILTIN) { };

    /**
     * Returns true, since the qt component is a
     * YaST2 server.
     */
    bool isServerCreator() const { return true; };

    /**
     * Creates a new Qt UI component.
     */
    Y2Component *create(const char * name) const
    {
	if (!strcmp(name, "qt") ) return new Y2QtComponent();
	else return 0;
    }
};

#endif	// ifndef _Y2CCQt_h


// EOF
