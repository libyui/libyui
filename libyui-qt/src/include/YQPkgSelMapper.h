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

  File:		YQPkgSelMapper.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPkgSelMapper_h
#define YQPkgSelMapper_h

#include "YQZypp.h"
#include <map>


/**
 * Find the corresponding ZyppSel to a ZyppPkg.
 * Returns 0 if there is no corresponding ZyppSel.
 *
 * WARNING: If there is no YQPkgSelMapper instantiated while this is called,
 * this will be very expensive!
 **/
ZyppSel findZyppSel( ZyppPkg pkg );


/**
 * Sentinel object that keeps a reference-counted cache attached while it
 * exists. Instantiating the first object of this class is expensive as this
 * will create the cache (a map from ZyppSel to ZyppPkg). While any one
 * instance of this class exists, the cache remains alive. When the last one is
 * destroyed, the cache is destroyed, too.
 *
 * Typical use: Instantiate an object of this class before doing a lot of
 * lookups with findZyppSel() and let it go out of scope when you are
 * done. Alternatively, you can add an object of this class to the members of
 * your class.
 **/
class YQPkgSelMapper
{
    friend ZyppSel findZyppSel( ZyppPkg pkg );

public:

    /**
     * Constructor. Builds a cache, if necessary.
     **/
    YQPkgSelMapper();

    /**
     * Destructor. Clears the cache if this was the last YQPkgSelMapper
     * (i.e. if refCount() reaches 0)
     **/
    virtual ~YQPkgSelMapper();

    /**
     * Reference count - indicates how many instances of this class are alive
     * right now.
     **/
    static int refCount() { return _refCount; }

    /**
     * Rebuild the cache. This is expensive. Call this only when the ZyppPool
     * has changed, i.e. after installation sources were added or removed.
     **/
    void rebuildCache();

    
protected:

    typedef std::map<ZyppPkg, ZyppSel>		Cache;
    typedef std::pair<ZyppPkg, ZyppSel>		CachePair;
    typedef Cache::iterator 			CacheIterator;

    static int		_refCount;
    static Cache	_cache;
};



#endif // YQPkgSelMapper_h
