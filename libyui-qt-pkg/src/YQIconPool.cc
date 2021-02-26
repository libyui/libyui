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

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQIconPool.h"

#include "icons/autodel.xpm"
#include "icons/autodel_disabled.xpm"
#include "icons/autoinstall.xpm"
#include "icons/autoinstall_disabled.xpm"
#include "icons/autoupdate.xpm"
#include "icons/autoupdate_disabled.xpm"
#include "icons/del.xpm"
#include "icons/del_disabled.xpm"
#include "icons/install.xpm"
#include "icons/install_disabled.xpm"
#include "icons/keepinstalled.xpm"
#include "icons/keepinstalled_disabled.xpm"
#include "icons/noinst.xpm"
#include "icons/noinst_disabled.xpm"
#include "icons/protected.xpm"
#include "icons/protected_disabled.xpm"
#include "icons/taboo.xpm"
#include "icons/taboo_disabled.xpm"
#include "icons/update.xpm"
#include "icons/update_disabled.xpm"
#include "icons/tree_plus.xpm"
#include "icons/tree_minus.xpm"
#include "icons/warning_sign.xpm"
#include "icons/satisfied.xpm"

#include "icons/normal_conflict.xpm"
#include "icons/select_conflict.xpm"
#include "icons/delete_conflict.xpm"
#include "icons/unresolvable_conflict.xpm"

#include "icons/checklist_off.xpm"
#include "icons/checklist_on.xpm"

#include "icons/step_current.xpm"
#include "icons/step_todo.xpm"
#include "icons/step_done.xpm"

#include "icons/tab_remove.xpm"
#include "icons/arrow_left.xpm"
#include "icons/arrow_right.xpm"
#include "icons/arrow_up.xpm"
#include "icons/arrow_down.xpm"



YQIconPool * YQIconPool::_iconPool = 0;

QPixmap YQIconPool::pkgTaboo()			{ return iconPool()->cachedIcon( taboo_xpm );			}
QPixmap YQIconPool::pkgDel()			{ return iconPool()->cachedIcon( del_xpm );			}
QPixmap YQIconPool::pkgUpdate()			{ return iconPool()->cachedIcon( update_xpm );			}
QPixmap YQIconPool::pkgInstall()		{ return iconPool()->cachedIcon( install_xpm );			}
QPixmap YQIconPool::pkgAutoInstall()		{ return iconPool()->cachedIcon( autoinstall_xpm );		}
QPixmap YQIconPool::pkgAutoUpdate()		{ return iconPool()->cachedIcon( autoupdate_xpm );		}
QPixmap YQIconPool::pkgAutoDel()		{ return iconPool()->cachedIcon( autodel_xpm );			}
QPixmap YQIconPool::pkgKeepInstalled()		{ return iconPool()->cachedIcon( keepinstalled_xpm );		}
QPixmap YQIconPool::pkgNoInst()			{ return iconPool()->cachedIcon( noinst_xpm );			}
QPixmap YQIconPool::pkgProtected()		{ return iconPool()->cachedIcon( protected_xpm );		}

QPixmap YQIconPool::disabledPkgTaboo()		{ return iconPool()->cachedIcon( taboo_disabled_xpm );		}
QPixmap YQIconPool::disabledPkgDel()		{ return iconPool()->cachedIcon( del_disabled_xpm );		}
QPixmap YQIconPool::disabledPkgUpdate()		{ return iconPool()->cachedIcon( update_disabled_xpm );		}
QPixmap YQIconPool::disabledPkgInstall()	{ return iconPool()->cachedIcon( install_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgAutoInstall()	{ return iconPool()->cachedIcon( autoinstall_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgAutoUpdate()	{ return iconPool()->cachedIcon( autoupdate_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgAutoDel()	{ return iconPool()->cachedIcon( autodel_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgKeepInstalled()	{ return iconPool()->cachedIcon( keepinstalled_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgNoInst()		{ return iconPool()->cachedIcon( noinst_disabled_xpm );		}
QPixmap YQIconPool::disabledPkgProtected()	{ return iconPool()->cachedIcon( protected_disabled_xpm );	}

QPixmap YQIconPool::checkMarkOn()		{ return iconPool()->cachedIcon( checklist_on_xpm );		}
QPixmap YQIconPool::checkMarkOff()		{ return iconPool()->cachedIcon( checklist_off_xpm );		}

QPixmap YQIconPool::normalPkgConflict()		{ return iconPool()->cachedIcon( normal_conflict_xpm );		}
QPixmap YQIconPool::severePkgConflict()		{ return iconPool()->cachedIcon( normal_conflict_xpm );		}
QPixmap YQIconPool::unresolvablePkgConflict()	{ return iconPool()->cachedIcon( unresolvable_conflict_xpm );	}
QPixmap YQIconPool::deletePkgConflict()		{ return iconPool()->cachedIcon( delete_conflict_xpm );		}
QPixmap YQIconPool::tabooPkgConflict()		{ return iconPool()->cachedIcon( normal_conflict_xpm );		}
QPixmap YQIconPool::selectPkgConflict()		{ return iconPool()->cachedIcon( select_conflict_xpm );		}

QPixmap YQIconPool::treePlus()			{ return iconPool()->cachedIcon( tree_plus_xpm  );		}
QPixmap YQIconPool::treeMinus()			{ return iconPool()->cachedIcon( tree_minus_xpm );		}

QPixmap YQIconPool::warningSign()		{ return iconPool()->cachedIcon( warning_sign_xpm );		}
QPixmap YQIconPool::pkgSatisfied()		{ return iconPool()->cachedIcon( satisfied_xpm );		}

QPixmap YQIconPool::stepCurrent()		{ return iconPool()->cachedIcon( step_current_xpm );		}
QPixmap YQIconPool::stepToDo()			{ return iconPool()->cachedIcon( step_todo_xpm );		}
QPixmap YQIconPool::stepDone()			{ return iconPool()->cachedIcon( step_done_xpm );		}

QPixmap YQIconPool::tabRemove()			{ return iconPool()->cachedIcon( tab_remove_xpm );		}
QPixmap YQIconPool::arrowLeft()			{ return iconPool()->cachedIcon( arrow_left_xpm );		}
QPixmap YQIconPool::arrowRight()		{ return iconPool()->cachedIcon( arrow_right_xpm );		}
QPixmap YQIconPool::arrowUp()			{ return iconPool()->cachedIcon( arrow_up_xpm );		}
QPixmap YQIconPool::arrowDown()			{ return iconPool()->cachedIcon( arrow_down_xpm );		}


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
YQIconPool::cachedIcon( const char ** xpm_data )
{
    QPixmap iconPtr = _iconCache[ xpm_data ];

    if ( iconPtr.isNull() )
    {
	iconPtr = QPixmap( xpm_data );
	_iconCache.insert( xpm_data, iconPtr );
    }

    return iconPtr;
}

