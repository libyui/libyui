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


  File:	      NCPkgSelMapper.cc
  Author:     Stefan Hundhammer <shundhammer@suse.de>

*/


#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include "NCPkgSelMapper.h"


using std::endl;


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

