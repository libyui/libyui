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

  File:		NCPackageSelectorPluginStub.cc

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/

#include "NCPackageSelectorPluginStub.h"

#define YUILogComponent "ncurses-ui"
#include "YUILog.h"
#include "NCWidget.h"
#include "NCLabel.h"
#include "NCDialog.h"
#include "NCPackageSelectorPluginIf.h"
#include "YNCursesUI.h" // NCtoY2Event

#define PLUGIN_BASE_NAME "ncurses_pkg"


NCPackageSelectorPluginStub::NCPackageSelectorPluginStub()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
	<< " plugin successfully from " << pluginLibFullPath()
	<< endl;
    }


    impl = ( NCPackageSelectorPluginIf* ) locateSymbol( "PSP" );

    if ( !impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

}


NCPackageSelectorPluginStub::~NCPackageSelectorPluginStub()
{
    // NOP
}


YPackageSelector * NCPackageSelectorPluginStub::createPackageSelector( YWidget * parent,
								       long modeFlags )
{
    return impl->createPackageSelector( parent, modeFlags );
}


YWidget * NCPackageSelectorPluginStub::createPkgSpecial( YWidget *parent, const string &subwidget )
{
    return impl->createPkgSpecial( parent, subwidget );
}


YEvent * NCPackageSelectorPluginStub::runPkgSelection( YDialog * dialog,
						       YWidget * selector )
{
    return impl->runPkgSelection( dialog, selector );
}
