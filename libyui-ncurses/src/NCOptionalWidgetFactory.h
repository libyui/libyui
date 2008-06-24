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
