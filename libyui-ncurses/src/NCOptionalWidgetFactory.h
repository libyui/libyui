/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

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

  File:		NCOptionalWidgetFactory.h

  Author:	Gabriele Mohr <gs@suse.de>

/-*/

#ifndef NCOptionalWidgetFactory_h
#define NCOptionalWidgetFactory_h

#include "YOptionalWidgetFactory.h"
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
