/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  File:	      YQPkgSelMapper.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include "YQPkgSelMapper.h"



int			YQPkgSelMapper::_refCount = 0;
YQPkgSelMapper::Cache	YQPkgSelMapper::_cache;


YQPkgSelMapper::YQPkgSelMapper()
{
    if ( ++_refCount == 1 )
	rebuildCache();
}


YQPkgSelMapper::~YQPkgSelMapper()
{
    if ( --_refCount == 0 )
    {
	yuiDebug() << "Destroying pkg -> selectable cache"  << endl;
	_cache.clear();
    }
}


void YQPkgSelMapper::rebuildCache()
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
YQPkgSelMapper::findZyppSel( ZyppPkg pkg )
{
    YQPkgSelMapper mapper; // This will build a cache, if there is none yet
    ZyppSel sel;

    YQPkgSelMapper::CacheIterator it = YQPkgSelMapper::_cache.find( pkg );

    if ( it != YQPkgSelMapper::_cache.end() )
	sel = it->second;
    else
	yuiWarning() << "No selectable found for package " << pkg->name() << endl;

    return sel;
}

