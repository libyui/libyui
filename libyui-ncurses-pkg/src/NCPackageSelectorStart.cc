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
      , packager( 0 )
{
    // set the textdomain
    setTextdomain( "ncurses-pkg" );
    
    // NEW NEW
    packager = new NCPackageSelector( modeFlags );

    NCPkgTable::NCPkgTableType type;

    if ( packager )
    {
        if ( packager->isYouMode() )
	{
	    packager->createYouLayout( this );
            type = NCPkgTable::T_Patches;
	}
        else 
	{
	    if ( packager->isUpdateMode() )
                type = NCPkgTable::T_Update;
            else
	        type = NCPkgTable::T_Packages;
	    packager->createPkgLayout( this, type );
        }
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

	if ( packager->isYouMode() )
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


