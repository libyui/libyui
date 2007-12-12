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


#include "pkg/icons/autodel.xpm"
#include "pkg/icons/autodel_disabled.xpm"
#include "pkg/icons/autoinstall.xpm"
#include "pkg/icons/autoinstall_disabled.xpm"
#include "pkg/icons/autoupdate.xpm"
#include "pkg/icons/autoupdate_disabled.xpm"
#include "pkg/icons/del.xpm"
#include "pkg/icons/del_disabled.xpm"
#include "pkg/icons/install.xpm"
#include "pkg/icons/install_disabled.xpm"
#include "pkg/icons/keepinstalled.xpm"
#include "pkg/icons/keepinstalled_disabled.xpm"
#include "pkg/icons/noinst.xpm"
#include "pkg/icons/noinst_disabled.xpm"
#include "pkg/icons/protected.xpm"
#include "pkg/icons/protected_disabled.xpm"
#include "pkg/icons/sel_autodel.xpm"
#include "pkg/icons/sel_autodel_disabled.xpm"
#include "pkg/icons/sel_autoinstall.xpm"
#include "pkg/icons/sel_autoinstall_disabled.xpm"
#include "pkg/icons/sel_autoupdate.xpm"
#include "pkg/icons/sel_autoupdate_disabled.xpm"
#include "pkg/icons/taboo.xpm"
#include "pkg/icons/taboo_disabled.xpm"
#include "pkg/icons/update.xpm"
#include "pkg/icons/update_disabled.xpm"
#include "pkg/icons/tree_plus.xpm"
#include "pkg/icons/tree_minus.xpm"
#include "pkg/icons/warning_sign.xpm"
#include "pkg/icons/satisfied.xpm"

#include "pkg/icons/normal_conflict.xpm"
#include "pkg/icons/select_conflict.xpm"
#include "pkg/icons/delete_conflict.xpm"
#include "pkg/icons/unresolvable_conflict.xpm"

#include "pkg/icons/checklist_off.xpm"
#include "pkg/icons/checklist_on.xpm"

#include "pkg/icons/step_current.xpm"
#include "pkg/icons/step_todo.xpm"
#include "pkg/icons/step_done.xpm"
//Added by qt3to4:
#include <qpixmap.h>


YQIconPool * YQIconPool::_iconPool = 0;

QPixmap YQIconPool::pkgTaboo()			{ return iconPool()->cachedIcon( taboo_xpm );			}
QPixmap YQIconPool::pkgDel()			{ return iconPool()->cachedIcon( del_xpm );			}
QPixmap YQIconPool::pkgUpdate()			{ return iconPool()->cachedIcon( update_xpm );			}
QPixmap YQIconPool::pkgInstall()		{ return iconPool()->cachedIcon( install_xpm );			}
QPixmap YQIconPool::pkgAutoInstall()		{ return iconPool()->cachedIcon( autoinstall_xpm );		}
QPixmap YQIconPool::pkgAutoUpdate()		{ return iconPool()->cachedIcon( autoupdate_xpm );		}
QPixmap YQIconPool::pkgAutoDel()		{ return iconPool()->cachedIcon( autodel_xpm );			}
QPixmap YQIconPool::pkgSelAutoInstall()		{ return iconPool()->cachedIcon( sel_autoinstall_xpm );		}
QPixmap YQIconPool::pkgSelAutoUpdate()		{ return iconPool()->cachedIcon( sel_autoupdate_xpm );		}
QPixmap YQIconPool::pkgSelAutoDel()		{ return iconPool()->cachedIcon( sel_autodel_xpm );		}
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
QPixmap YQIconPool::disabledPkgSelAutoInstall()	{ return iconPool()->cachedIcon( sel_autoinstall_disabled_xpm );}
QPixmap YQIconPool::disabledPkgSelAutoUpdate()	{ return iconPool()->cachedIcon( sel_autoupdate_disabled_xpm );	}
QPixmap YQIconPool::disabledPkgSelAutoDel()	{ return iconPool()->cachedIcon( sel_autodel_disabled_xpm );	}
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

