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

  File:		NCOptionalWidgetFactory.cc

  Author:	Gabriele Mohr <gs@suse.de>

/-*/

#include "NCOptionalWidgetFactory.h"
#include <yui/YUIException.h>
#include <NCTimeField.h>
#include <NCDateField.h>


#define THROW_UNSUPPORTED( WIDGET_TYPE ) \
    YUI_THROW( YUIUnsupportedWidgetException( WIDGET_TYPE ) );	\
    return 0


NCOptionalWidgetFactory::NCOptionalWidgetFactory()
	: YOptionalWidgetFactory()
{
    // NOP
}


NCOptionalWidgetFactory::~NCOptionalWidgetFactory()
{
    // NOP
}

bool NCOptionalWidgetFactory::hasDumbTab()
{
    return true;
}

NCDumbTab *
NCOptionalWidgetFactory::createDumbTab( YWidget * parent )
{
    NCDumbTab * dumbTab = new NCDumbTab( parent );
    YUI_CHECK_NEW( dumbTab );

    return dumbTab;
}

bool NCOptionalWidgetFactory::hasTimeField()
{
	return true;
}

YTimeField* NCOptionalWidgetFactory::createTimeField( YWidget * parent, const std::string & label )
{
    NCTimeField * timeField = new NCTimeField( parent, label );
    YUI_CHECK_NEW( timeField );

    return timeField;
}


bool NCOptionalWidgetFactory::hasDateField()
{
	return true;
}

YDateField* NCOptionalWidgetFactory::createDateField( YWidget * parent, const std::string & label )
{
    NCDateField * dateField = new NCDateField( parent, label );
    YUI_CHECK_NEW( dateField );

    return dateField;
}

