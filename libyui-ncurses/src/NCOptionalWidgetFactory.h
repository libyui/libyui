/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		NCOptionalWidgetFactory.h

  Author:	Gabriele Mohr <gs@suse.de>

/-*/

#ifndef NCOptionalWidgetFactory_h

#define NCOptionalWidgetFactory_h

#include <yui/YOptionalWidgetFactory.h>
#include "NCDumbTab.h"


/**
 * Widget factory for optional ("special") widgets.
 *
 * Remember to always check with the corresponding "has..()" method if the
 * current UI actually provides the requested widget. Otherwise the
 * "create...()" method will throw an exception.
 **/
class NCOptionalWidgetFactory: public YOptionalWidgetFactory
{

public:

    //
    // Optional Widgets
    //

    // Currently none for this UI

protected:

    friend class YNCursesUI;

    /**
     * Constructor.
     *
     * Use YUI::optionalWidgetFactory() to get the singleton for this class.
     **/
    NCOptionalWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~NCOptionalWidgetFactory();

    bool hasDumbTab();

    NCDumbTab *createDumbTab( YWidget * parent );

}; // class NCOptionalWidgetFactory



#endif // NCOptionalWidgetFactory_h
