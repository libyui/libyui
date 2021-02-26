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

   File:       NCPackageSelectorStart.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCurses.h"
#include "NCPackageSelectorStart.h"
#include "NCPushButton.h"
#include "NCSpacing.h"
#include "NCPkgTable.h"
#include "NCLabel.h"
#include "NCPkgStrings.h"
#include "NCi18n.h"

#include <libintl.h>

#include "YPackageSelector.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::NCPackageSelectorStart
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPackageSelectorStart::NCPackageSelectorStart( YWidget * parent,
						long modeFlags,
						YUIDimension dimension )
    : NCLayoutBox( parent, dimension )
      , widgetRoot( 0 )
      , packager( 0 )
      , youMode ( false )
      , updateMode ( false )
{
    YNCursesUI * ui = YNCursesUI::ui();
    
    // set the textdomain
    setTextdomain( "ncurses-pkg" );
    
    // get the mode (the mode is also available in PackageSelector via modeFlags)
    if ( modeFlags & YPkg_OnlineUpdateMode )
	youMode = true;

    if ( modeFlags & YPkg_UpdateMode )
	updateMode = true;

    // NEW NEW
    packager = new NCPackageSelector( ui, widgetRoot, modeFlags );

    NCPkgTable::NCPkgTableType type;

    if ( youMode )
	type = NCPkgTable::T_Patches;
    else if ( updateMode )
	type = NCPkgTable::T_Update;
    else
	type = NCPkgTable::T_Packages;

    if ( packager )
    {
	yuiMilestone() << "YouMode: " <<  (youMode?"true":"false") << endl;
	if ( !youMode )
	    packager->createPkgLayout( this, type );
	else
	    packager->createYouLayout( this );
    }
    
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::~NCPackageSelectorStart
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPackageSelectorStart::~NCPackageSelectorStart()
{
    if ( packager )
    {
	delete packager;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPackageSelectorStart::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YLayoutBox::setSize( newwidth, newheight );
}


///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelectorStart::showDefaultList
//      METHOD TYPE : void
//
//      DESCRIPTION : fill up the package table with default data
//		     
//
void NCPackageSelectorStart::showDefaultList()
{
    // fill the package table with packages belonging to the default filter

    if ( packager )
    {
        // always do an initial dependency solving - BEFORE filling the default list
	packager->showPackageDependencies( true );
	
	// fill package list with packages belonging to default RPM group
	packager->fillDefaultList();

	if ( youMode )
	{
	    // show download size
	    packager->showDownloadSize();
	}
	else
	{
	    // show the required diskspace
	    packager->showDiskSpace();
	}
    }
}

///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelectorStart::handleEvent
//      METHOD TYPE : bool
//
//      DESCRIPTION : passes the event to the handleEvent method
//		      of the member variable PackageSelector packager
//
bool NCPackageSelectorStart::handleEvent ( const NCursesEvent & event )
{
    if ( !packager )
	return false;
    
    return packager->handleEvent( event );
}


