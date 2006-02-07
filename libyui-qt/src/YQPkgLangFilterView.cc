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

  File:	      YQPkgLangFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQZypp.h"
#include <zypp/ui/ResPoolProxy.h>

#include "YQPkgLangFilterView.h"
#include "YQPkgLangList.h"
#include "YQi18n.h"



YQPkgLangFilterView::YQPkgLangFilterView( QWidget * parent )
    : QVBox( parent )
{
    _langList = new YQPkgLangList( this );
}


YQPkgLangFilterView::~YQPkgLangFilterView()
{
    // NOP
}






#include "YQPkgLangFilterView.moc"
