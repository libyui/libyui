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

  File:	      YQPkgSelectionsFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define USE_TACKAT_ICONS 0

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgSelectionsFilterView.h"
#include "YQPkgSelList.h"
#include "YUIQt.h"
#include "YQi18n.h"



YQPkgSelectionsFilterView::YQPkgSelectionsFilterView( YUIQt *yuiqt, QWidget *parent )
    : QVBox( parent )
    , yuiqt( yuiqt )
{
    y2milestone( "Creating selections view" );

    _selList = new YQPkgSelList( yuiqt, this );
}


YQPkgSelectionsFilterView::~YQPkgSelectionsFilterView()
{
    // NOP
}






#include "YQPkgSelectionsFilterView.moc.cc"
