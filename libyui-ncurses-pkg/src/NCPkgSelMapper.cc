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


#define y2log_component "ncurses-pkg"
#include <ycp/y2log.h>

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
	y2debug( "Destroying pkg -> selectable cache" );
	_cache.clear();
    }
}


void NCPkgSelMapper::rebuildCache()
{
    _cache.clear();
    y2debug( "Building pkg -> selectable cache" );

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

    y2debug( "Building pkg -> selectable cache done" );
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
	y2warning( "No selectable found for package %s", pkg->name().c_str() );

    return sel;
}

