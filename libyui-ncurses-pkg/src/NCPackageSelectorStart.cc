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


  File:       NCPackageSelectorStart.cc
  Author:     Gabriele Strattner <gs@suse.de>

*/

#include <libintl.h>

#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include <yui/YPackageSelector.h>

#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCLabel.h>
#include <yui/ncurses/NCSpacing.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCurses.h>

#include "NCPackageSelectorStart.h"
#include "NCPkgStrings.h"
#include "NCPkgTable.h"


using std::endl;

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


