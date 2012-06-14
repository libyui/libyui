/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

  File:		NCOptionalWidgetFactory.cc

  Author:	Gabriele Mohr <gs@suse.de>

/-*/

#include "NCOptionalWidgetFactory.h"
#include <yui/YUIException.h>


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



