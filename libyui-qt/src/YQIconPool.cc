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

  File:	      YQIconPool.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQIconPool.h"


#define USE_TACKAT_ICONS 0

#include "checklist_on.xpm"
#include "checklist_off.xpm"
#include "dep.xpm"	// bad icon name (auto_inst etc.)


#if USE_TACKAT_ICONS
#include "dontrm.xpm"
#include "stop.xpm"
#include "trash.xpm"
#else
#include "trashcan.xpm"
#include "recycle.xpm"
#include "taboo.xpm"
#endif



YQIconPool * YQIconPool::_iconPool = 0;

QPixmap YQIconPool::pkgTaboo()		{ return iconPool()->cachedIcon( taboo_xpm );		}
QPixmap YQIconPool::pkgDel()		{ return iconPool()->cachedIcon( trashcan_xpm );	}
QPixmap YQIconPool::pkgUpdate()		{ return iconPool()->cachedIcon( recycle_xpm );		}
QPixmap YQIconPool::pkgInstall()	{ return iconPool()->cachedIcon( checklist_on_xpm );	}
QPixmap YQIconPool::pkgAuto()		{ return iconPool()->cachedIcon( dep_xpm );		}
QPixmap YQIconPool::pkgKeepInstalled()	{ return iconPool()->cachedIcon( checklist_on_xpm );	}
QPixmap YQIconPool::pkgNoInst()		{ return iconPool()->cachedIcon( checklist_off_xpm );	}

QPixmap YQIconPool::checkMarkOn()	{ return iconPool()->cachedIcon( checklist_on_xpm );	}
QPixmap YQIconPool::checkMarkOff()	{ return iconPool()->cachedIcon( checklist_off_xpm );	}



YQIconPool * YQIconPool::iconPool()
{
    if ( ! _iconPool )
	_iconPool = new YQIconPool();

    return _iconPool;
}


YQIconPool::YQIconPool()
{
    // y2milestone( "Creating icon pool" );
    _iconCache.setAutoDelete( true );
}


YQIconPool::~YQIconPool()
{
    // NOP
}



QPixmap
YQIconPool::cachedIcon( const char ** xpm_data )
{
    QPixmap * iconPtr = _iconCache[ xpm_data ];

    if ( ! iconPtr )
    {
	// y2milestone( "Creating icon" );
	iconPtr = new QPixmap( xpm_data );
	_iconCache.insert( xpm_data, iconPtr );
    }
	
    return *iconPtr;
}

