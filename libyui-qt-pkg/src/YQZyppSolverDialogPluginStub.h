/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		YQZyppSolverDialogPluginStub.h

  Authors:	Stefan Schubert <schubi@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQZyppSolverDialogPluginStub_h
#define YQZyppSolverDialogPluginStub_h


#include "YUIPlugin.h"

#include "YQZyppSolverDialogPluginIf.h"
#include <zypp/PoolItem.h>

class YWidget;

/**
 * Simplified access to the QZyppSolverDialog plugin.
 **/
class YQZyppSolverDialogPluginIf;


class YQZyppSolverDialogPluginStub: public YUIPlugin
{
public:
    /**
     * Constructor: Load the plugin library for the QZyppDialog widget
     **/
    YQZyppSolverDialogPluginStub();

    /**
     * Destructor.
     **/
    virtual ~YQZyppSolverDialogPluginStub();

    /**
     * Create and show a QZyppDialog widget
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual bool createZyppSolverDialog( const zypp::PoolItem item );


    YQZyppSolverDialogPluginIf * impl ;
};


#endif // YQZyppSolverDialogPluginStub_h
