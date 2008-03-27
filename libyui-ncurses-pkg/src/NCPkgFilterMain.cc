/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgFilterMain.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterMain.h"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgFilterMain::NCPkgFilterMain (YWidget *parent, string label, NCPackageSelector *pkger )
	:NCComboBox(parent, label, false)
	,pkg (pkger)
{
    createLayout();
    setNotify( true );
}

NCPkgFilterMain::~NCPkgFilterMain()
{

}

void NCPkgFilterMain::createLayout()
{
    patterns = new YItem( _("Patterns") );
    items.push_back( patterns );

    languages = new YItem( _("Languages") );
    items.push_back( languages );

    rpmgroups = new YItem( _("RPM Groups") );
    items.push_back( rpmgroups );

    repositories = new YItem( _("Repositories") );
    items.push_back( repositories );

    search = new YItem( _("Search") );
    items.push_back( search );

    inst_summary = new YItem( _("Installation Summary") );
    items.push_back( inst_summary );

    addItems( items );
    
}

bool NCPkgFilterMain::handleEvent ( )
{

    YItem *currentItem = selectedItem();

    if ( currentItem == rpmgroups )
	pkg->replaceFilter ( NCPackageSelector::RPMGroups ); 
    else if ( currentItem == patterns )
	pkg->replaceFilter ( NCPackageSelector::Patterns ); 
    else if ( currentItem == languages )
	pkg->replaceFilter ( NCPackageSelector::Languages ); 
    else if ( currentItem == repositories )
	pkg->replaceFilter ( NCPackageSelector::Repositories ); 
    else if ( currentItem == search )
	pkg->replaceFilter ( NCPackageSelector::Search ); 
    else if ( currentItem == inst_summary )
	pkg->replaceFilter ( NCPackageSelector::Summary ); 
    else
	yuiError() << "zatim nic" << endl;
    return true;
}
