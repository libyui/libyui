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

  File:	      YQSplit.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQSplit.h"


YQSplit::YQSplit( QWidget * 	parent,
		  YWidgetOpt &	opt,
		  YUIDimension	dimension )
    : QWidget( parent )
    , YSplit( opt, dimension )
{
    setWidgetRep( this );
}


void YQSplit::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


void YQSplit::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    YSplit::setSize( newWidth, newHeight );
}


void YQSplit::moveChild( YWidget * child, long newX, long newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


#include "YQSplit.moc.cc"
