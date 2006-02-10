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


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPkgSelectionsFilterView.h"
#include "YQPkgSelList.h"
#include "YQi18n.h"



YQPkgSelectionsFilterView::YQPkgSelectionsFilterView( QWidget * parent )
    : QVBox( parent )
{
    _selList = new YQPkgSelList( this );
}


YQPkgSelectionsFilterView::~YQPkgSelectionsFilterView()
{
    // NOP
}






#include "YQPkgSelectionsFilterView.moc"
