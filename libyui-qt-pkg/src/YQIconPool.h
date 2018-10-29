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

  File:	      YQIconPool.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQIconPool_h
#define YQIconPool_h

#include <qpixmap.h>
#include <QHash>

class YQIconPool
{
public:

    static QPixmap pkgAutoDel();
    static QPixmap pkgAutoInstall();
    static QPixmap pkgAutoUpdate();
    static QPixmap pkgDel();
    static QPixmap pkgInstall();
    static QPixmap pkgKeepInstalled();
    static QPixmap pkgNoInst();
    static QPixmap pkgProtected();
    static QPixmap pkgTaboo();
    static QPixmap pkgUpdate();

    static QPixmap disabledPkgAutoDel();
    static QPixmap disabledPkgAutoInstall();
    static QPixmap disabledPkgAutoUpdate();
    static QPixmap disabledPkgDel();
    static QPixmap disabledPkgInstall();
    static QPixmap disabledPkgKeepInstalled();
    static QPixmap disabledPkgNoInst();
    static QPixmap disabledPkgProtected();
    static QPixmap disabledPkgTaboo();
    static QPixmap disabledPkgUpdate();
    
    static QPixmap normalPkgConflict();

    static QPixmap treePlus();
    static QPixmap treeMinus();
    
    static QPixmap warningSign();
    static QPixmap pkgSatisfied();

    static QPixmap tabRemove();
    static QPixmap arrowLeft();
    static QPixmap arrowRight();
    static QPixmap arrowDown();

protected:

    /**
     * Return the global icon pool.
     **/
    static YQIconPool * iconPool();

    /**
     * Return the cached icon for 'xpm_data' ( an included XPM file ).
     * If the icon isn't in the cache yet, create it and store it in the
     * cache.
     **/
    QPixmap cachedIcon(const QString icon_name, const bool enabled );

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

    
    //
    // Data members
    //

    static YQIconPool * _iconPool;
    QHash< const QString, QPixmap >	_iconCache;
};


#endif // ifndef YQIconPool_h
