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

  File:	      YQIconPool.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#include <qpixmap.h>
#include <QIcon>

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQIconPool.h"



YQIconPool * YQIconPool::_iconPool = 0;

QPixmap YQIconPool::pkgTaboo()			{ return iconPool()->cachedIcon( "package-available-locked", true );		}
QPixmap YQIconPool::pkgDel()			{ return iconPool()->cachedIcon( "package-remove", true );			}
QPixmap YQIconPool::pkgUpdate()			{ return iconPool()->cachedIcon( "package-upgrade", true );			}
QPixmap YQIconPool::pkgInstall()		{ return iconPool()->cachedIcon( "package-install", true );			}
QPixmap YQIconPool::pkgAutoInstall()		{ return iconPool()->cachedIcon( "package-install-auto", true );		}
QPixmap YQIconPool::pkgAutoUpdate()		{ return iconPool()->cachedIcon( "package-upgrade-auto", true );		}
QPixmap YQIconPool::pkgAutoDel()		{ return iconPool()->cachedIcon( "package-remove-auto", true );			}
QPixmap YQIconPool::pkgKeepInstalled()		{ return iconPool()->cachedIcon( "package-installed-updated", true );		}
QPixmap YQIconPool::pkgNoInst()			{ return iconPool()->cachedIcon( "package-available", true );			}
QPixmap YQIconPool::pkgProtected()		{ return iconPool()->cachedIcon( "package-installed-locked", true );		}

QPixmap YQIconPool::disabledPkgTaboo()		{ return iconPool()->cachedIcon( "package-available-locked", false );		}
QPixmap YQIconPool::disabledPkgDel()		{ return iconPool()->cachedIcon( "package-remove", false );			}
QPixmap YQIconPool::disabledPkgUpdate()		{ return iconPool()->cachedIcon( "package-upgrade", false );			}
QPixmap YQIconPool::disabledPkgInstall()	{ return iconPool()->cachedIcon( "package-install", false );			}
QPixmap YQIconPool::disabledPkgAutoInstall()	{ return iconPool()->cachedIcon( "package-install-auto", false );		}
QPixmap YQIconPool::disabledPkgAutoUpdate()	{ return iconPool()->cachedIcon( "package-upgrade-auto", false );		}
QPixmap YQIconPool::disabledPkgAutoDel()	{ return iconPool()->cachedIcon( "package-remove-auto", false );		}
QPixmap YQIconPool::disabledPkgKeepInstalled()	{ return iconPool()->cachedIcon( "package-installed-updated", false );		}
QPixmap YQIconPool::disabledPkgNoInst()		{ return iconPool()->cachedIcon( "package-available", false );			}
QPixmap YQIconPool::disabledPkgProtected()	{ return iconPool()->cachedIcon( "package-installed-locked", false );		}

QPixmap YQIconPool::normalPkgConflict()		{ return iconPool()->cachedIcon( "emblem-warning", true );			}

QPixmap YQIconPool::treePlus()			{ return iconPool()->cachedIcon( "list-add", true );				}
QPixmap YQIconPool::treeMinus()			{ return iconPool()->cachedIcon( "list-remove", true );				}

QPixmap YQIconPool::warningSign()		{ return iconPool()->cachedIcon( "emblem-warning", true );			}
QPixmap YQIconPool::pkgSatisfied()		{ return iconPool()->cachedIcon( "package-supported", true );			}

QPixmap YQIconPool::tabRemove()			{ return iconPool()->cachedIcon( "tab-close", true );				}
QPixmap YQIconPool::arrowLeft()			{ return iconPool()->cachedIcon( "arrow-left", true );				}
QPixmap YQIconPool::arrowRight()		{ return iconPool()->cachedIcon( "arrow-right", true );				}
QPixmap YQIconPool::arrowDown()			{ return iconPool()->cachedIcon( "arrow-down", true );				}

YQIconPool * YQIconPool::iconPool()
{
    if ( ! _iconPool )
	_iconPool = new YQIconPool();

    return _iconPool;
}


YQIconPool::YQIconPool()
{
}


YQIconPool::~YQIconPool()
{
    // NOP
}



QPixmap
YQIconPool::cachedIcon( const QString icon_name, const bool enabled )
{
    Q_INIT_RESOURCE(qt_pkg_icons);
    QPixmap iconPixmap = _iconCache[ icon_name + enabled ];

    if ( !iconPixmap )
    {
        if ( QIcon::hasThemeIcon(icon_name) )
        {
            QIcon icon = QIcon::fromTheme(icon_name, QIcon( ":/" + icon_name ));
            iconPixmap = icon.pixmap(QSize(16,16), enabled ? QIcon::Normal : QIcon::Disabled);
        }
        else
        {
            QIcon icon = QIcon( ":/" + icon_name );
            iconPixmap = icon.pixmap(QSize(16,16), enabled ? QIcon::Normal : QIcon::Disabled);
        }
    }

    _iconCache.insert( icon_name + enabled, iconPixmap);
    return iconPixmap;
}

