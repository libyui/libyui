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

  File:	      YQIconPool.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQIconPool_h
#define YQIconPool_h

#include <qpixmap.h>
#include <qptrdict.h>

class YQIconPool
{
public:

    static QPixmap pkgTaboo();
    static QPixmap pkgDel();
    static QPixmap pkgUpdate();
    static QPixmap pkgInstall();
    static QPixmap pkgAutoInstall();
    static QPixmap pkgAutoUpdate();
    static QPixmap pkgAutoDel();
    static QPixmap pkgKeepInstalled();
    static QPixmap pkgNoInst();

    static QPixmap disabledPkgTaboo();
    static QPixmap disabledPkgDel();
    static QPixmap disabledPkgUpdate();
    static QPixmap disabledPkgInstall();
    static QPixmap disabledPkgAutoInstall();
    static QPixmap disabledPkgAutoUpdate();
    static QPixmap disabledPkgAutoDel();
    static QPixmap disabledPkgKeepInstalled();
    static QPixmap disabledPkgNoInst();

    static QPixmap checkMarkOn();
    static QPixmap checkMarkOff();
    
    static QPixmap normalPkgConflict();
    static QPixmap severePkgConflict();
    static QPixmap unresolvablePkgConflict();
    static QPixmap deletePkgConflict();
    static QPixmap tabooPkgConflict();
    static QPixmap selectPkgConflict();


protected:

    /**
     * Return the global icon pool.
     **/
    static YQIconPool * iconPool();

    /**
     * Return the cached icon for 'xpm_data' (an included XPM file).
     * If the icon isn't in the cache yet, create it and store it in the
     * cache.
     **/
    QPixmap cachedIcon( const char ** xpm_data );

private:

    /**
     * (Private!) Constructor
     * Singleton object - use the static methods only!
     **/
    YQIconPool();

    /**
     * Destructor
     **/
    virtual ~YQIconPool();

    // Data members

    static YQIconPool * _iconPool;
    QPtrDict<QPixmap>	_iconCache;
};


#endif // ifndef YQIconPool_h
