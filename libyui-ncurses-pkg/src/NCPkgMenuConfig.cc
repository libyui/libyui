/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgMenuConfig.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuConfig.h"
#include "NCPackageSelector.h"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuConfig::NCPkgMenuConfig (YWidget *parent, string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label)
	,pkg( pkger ) 
{
    createLayout();
}

NCPkgMenuConfig::~NCPkgMenuConfig()
{

}

void NCPkgMenuConfig::createLayout()
{
    repoManager =  new YMenuItem( _( "Launch Repository Manager") );
    onlineUpdate = new YMenuItem( _( "Launch Online Update Configuration" ) );
    actionOnExit =  new YMenuItem( _( "Action on Exit" ) );

    items.push_back( repoManager );
    items.push_back( onlineUpdate );
    items.push_back( actionOnExit );

    restart = new YMenuItem( actionOnExit, _( "[x] Close Package Manager" ) );
    close = new YMenuItem( actionOnExit, _( "[ ] Restart Package Manager" ) );
    showSummary = new YMenuItem( actionOnExit,  _( "[ ] Show Summary" ) );

    addItems( items );
   
}

bool NCPkgMenuConfig::handleEvent( const NCursesEvent & event)
{
    if (!event.selection)
	return false;

    if ( event.selection == repoManager )
    {
	//return `repo_mgr symbol to YCP module (FaTE #302517)
	const_cast<NCursesEvent &>(event).result = "repo_mgr";
	yuiMilestone() << "Launching repository manager " << endl;

        //and close the main loop
	return false;
    }
    else if ( event.selection == onlineUpdate )
    {	
	//the same as above, return `online_update_config
	const_cast<NCursesEvent &>(event).result = "online_update_configuration";
	yuiMilestone() << "Launching YOU configuration " << endl;

	return false;

    }

    return true;
}

