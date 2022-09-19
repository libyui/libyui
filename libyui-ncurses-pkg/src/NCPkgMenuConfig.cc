/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


  File:       NCPkgMenuConfig.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include "NCPkgMenuConfig.h"
#include "NCPackageSelector.h"


#define CHECK_BOX "[ ] "

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/


NCPkgMenuConfig::NCPkgMenuConfig (YWidget *parent, std::string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label)
	, pkg( pkger )
{
    createLayout();
}

NCPkgMenuConfig::~NCPkgMenuConfig()
{

}

void NCPkgMenuConfig::setSelected( YMenuItem *item, bool selected)
{
    std::string oldLabel = item->label();

    std::string newLabel = oldLabel.replace(1, 1, 1, selected ? 'x' : ' ');

    item->setLabel( newLabel);
}

void NCPkgMenuConfig::createLayout()
{
    exitAction = pkg->ActionAtExit();

    repoManager =  new YMenuItem( _( "Launch &Repository Manager") );
    onlineUpdate = new YMenuItem( _( "Launch &Online Update Configuration" ) );
    actionOnExit =  new YMenuItem( _( "&Action after Package Installation" ) );

    items.push_back( repoManager );
    items.push_back( onlineUpdate );

    if (! exitAction.empty())
    {
	items.push_back( actionOnExit );

	restart = new YMenuItem( actionOnExit, CHECK_BOX + _( "&Restart Package Manager" ) );
	close = new YMenuItem( actionOnExit, CHECK_BOX +  _( "&Close Package Manager" ) );
	showSummary = new YMenuItem( actionOnExit, CHECK_BOX +  _( "&Show Summary" ) );

	idToItemPtr["restart"] = restart;
	idToItemPtr["close"] = close;
	idToItemPtr["summary"] = showSummary;

	setSelected( idToItemPtr[ exitAction ], true);
    }

    addItems( items );

}

bool NCPkgMenuConfig::handleEvent( const NCursesEvent & event)
{
    if (!event.selection)
	return false;

    if ( event.selection == repoManager )
    {
	// return :repo_mgr symbol to Ruby module (FaTE #302517)
	const_cast<NCursesEvent &>(event).result = "repo_mgr";
	yuiMilestone() << "Launching repository manager " << endl;

        // and close the main loop
	return false;
    }
    else if ( event.selection == onlineUpdate )
    {
	// the same as above, return :online_update_config
	const_cast<NCursesEvent &>(event).result = "online_update_configuration";
	yuiMilestone() << "Launching YOU configuration " << endl;

	return false;
    }
    else
    {
	std::string old = exitAction;

	if ( event.selection == restart )
	{
	    exitAction = "restart";
	}
	else if ( event.selection == close )
	{
	    exitAction = "close";
	}
	else if ( event.selection == showSummary )
	{
	    exitAction = "summary";
	}

	setSelected(idToItemPtr[old], false);
	setSelected(idToItemPtr[exitAction], true);
	pkg->setActionAtExit( exitAction );
    }

    return true;
}

