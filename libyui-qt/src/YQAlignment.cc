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

  File:	      YQAlignment.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQAlignment.h"


YQAlignment::YQAlignment( QWidget *parent,
			  YWidgetOpt &opt,
			  YAlignmentType horAlign,
			  YAlignmentType vertAlign )
    : QWidget( parent )
    , YAlignment( opt, horAlign, vertAlign )
{
    setWidgetRep( this );
}


void YQAlignment::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


void YQAlignment::moveChild( YWidget *child, long newX, long newY )
{
    QWidget *qw = (QWidget *) ( child->widgetRep() );
    qw->move( newX, newY );
}


void YQAlignment::setSize( long newWidth, long newHeight )
{
    resize(newWidth, newHeight);
    YAlignment::setSize(newWidth, newHeight);
}


#include "YQAlignment.moc.cc"
