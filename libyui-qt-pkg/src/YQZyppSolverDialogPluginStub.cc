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

  File:		YQZyppSolverDialogPluginStub.cc

  Authors:	Stefan Schubert <schubi@suse.de>

  Textdomain "qt-pkg"


/-*/

#include <qmessagebox.h>
#include "YQZyppSolverDialogPluginStub.h"

#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "YQi18n.h"

#define PLUGIN_BASE_NAME "qt_zypp_solver_dialog"

YQZyppSolverDialogPluginStub::YQZyppSolverDialogPluginStub()
    : YUIPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
                       << " plugin successfully from " << pluginLibFullPath()
                       << endl;
    }


    impl = (YQZyppSolverDialogPluginIf*) locateSymbol("ZYPPDIALOGP");
    
    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide ZYPPP symbol" << endl;
    }
}


YQZyppSolverDialogPluginStub::~YQZyppSolverDialogPluginStub()
{
    // NOP
}


bool
YQZyppSolverDialogPluginStub::createZyppSolverDialog( const zypp::PoolItem item )
{
    if ( ! impl )
    {
	QMessageBox::information( 0,
			       _("Missing package") ,
			       _("Package libqdialogsolver is required for this feature."));	
	return false;
    }
    
    return impl->createZyppSolverDialog( item );
}


