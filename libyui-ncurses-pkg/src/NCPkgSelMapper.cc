/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/



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

  File:	      NCPkgSelMapper.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "ncurses-pkg"
#include "YUILog.h"

#include "NCPkgSelMapper.h"



int			NCPkgSelMapper::_refCount = 0;
NCPkgSelMapper::Cache	NCPkgSelMapper::_cache;


NCPkgSelMapper::NCPkgSelMapper()
{
    if ( ++_refCount == 1 )
	rebuildCache();
}


NCPkgSelMapper::~NCPkgSelMapper()
{
    if ( --_refCount == 0 )
    {
	yuiDebug() << "Destroying pkg -> selectable cache" << endl;
	_cache.clear();
    }
}


void NCPkgSelMapper::rebuildCache()
{
    _cache.clear();
    yuiDebug() << "Building pkg -> selectable cache" << endl;

    for ( ZyppPoolIterator sel_it = zyppPkgBegin();
	  sel_it != zyppPkgEnd();
	  ++sel_it )
    {
	ZyppSel sel = *sel_it;

	if ( sel->installedObj() )
	{
	    // The installed package (if there is any) may or may not be in the list
	    // of available packages. Better make sure to insert it.

	    ZyppPkg installedPkg = tryCastToZyppPkg( sel->installedObj() );

	    if ( installedPkg )
		_cache.insert( CachePair( installedPkg, sel ) );
	}

	zypp::ui::Selectable::available_iterator it = sel->availableBegin();

	while ( it != sel->availableEnd() )
	{
	    ZyppPkg pkg = tryCastToZyppPkg( *it );

	    if ( pkg )
		_cache.insert( CachePair( pkg, sel ) );

	    ++it;
	}
    }

    yuiDebug() << "Building pkg -> selectable cache done" << endl;
}


ZyppSel
NCPkgSelMapper::findZyppSel( ZyppPkg pkg )
{
    NCPkgSelMapper mapper; // This will build a cache, if there is none yet
    ZyppSel sel;

    NCPkgSelMapper::CacheIterator it = NCPkgSelMapper::_cache.find( pkg );

    if ( it != NCPkgSelMapper::_cache.end() )
	sel = it->second;
    else
	yuiWarning() << "No selectable found for package %s" << pkg->name().c_str() << endl;

    return sel;
}

