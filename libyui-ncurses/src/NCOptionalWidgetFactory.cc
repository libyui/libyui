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

  File:		NCOptionalWidgetFactory.cc

  Author:	Gabriele Mohr <gs@suse.de>

/-*/

#include "NCOptionalWidgetFactory.h"
#include "YUIException.h"


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



