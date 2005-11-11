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

  File:	      YQPkgInstSrcFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgInstSrcFilterView.h"
#include "YQPkgInstSrcList.h"
#include "YQi18n.h"



YQPkgInstSrcFilterView::YQPkgInstSrcFilterView( QWidget * parent )
    : QVBox( parent )
{
    _instSrcList = new YQPkgInstSrcList( this );
}


YQPkgInstSrcFilterView::~YQPkgInstSrcFilterView()
{
    // NOP
}






#include "YQPkgInstSrcFilterView.moc"
