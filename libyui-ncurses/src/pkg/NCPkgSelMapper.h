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

  File:		NCPkgSelMapper.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef NCPkgSelMapper_h
#define NCPkgSelMapper_h

#include "NCZypp.h"
#include <map>



/**
 * Mapping from ZyppPkg to the correspoinding ZyppSel.
 *
 * All instances of this class share the same cache. The cache remains alive as
 * long as any instance of this class exists.
 **/
class NCPkgSelMapper
{
public:

    /**
     * Constructor. Builds a cache, if necessary.
     **/
    NCPkgSelMapper();

    /**
     * Destructor. Clears the cache if this was the last NCPkgSelMapper
     * (i.e. if refCount() reaches 0)
     **/
    virtual ~NCPkgSelMapper();

    /**
     * Find the corresponding ZyppSel to a ZyppPkg.
     * Returns 0 if there is no corresponding ZyppSel.
     **/

    ZyppSel findZyppSel( ZyppPkg pkg );

    /**
     * Reference count - indicates how many instances of this class are alive
     * right now.
     **/
    static int refCount() { return _refCount; }

    /**
     * Rebuild the shared cache. This is expensive. Call this only when the
     * ZyppPool has changed, i.e. after installation sources were added or
     * removed. 
     *
     * Since the cache is shared, this affects all instances of this class.
     **/
    void rebuildCache();


protected:

    typedef std::map<ZyppPkg, ZyppSel>		Cache;
    typedef std::pair<ZyppPkg, ZyppSel>		CachePair;
    typedef Cache::iterator 			CacheIterator;

    static int		_refCount;
    static Cache	_cache;
};



#endif // NCPkgSelMapper_h
