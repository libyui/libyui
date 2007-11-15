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

  File:	      YQLayoutBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQLayoutBox.h"


YQLayoutBox::YQLayoutBox( YWidget * 	parent,
			  YUIDimension	dimension )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YLayoutBox( parent, dimension )
{
    setWidgetRep( this );
}


void YQLayoutBox::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQLayoutBox::setSize( int newWidth, int newHeight )
{
    // y2debug( "Resizing %s to %d x %d", widgetClass(), newWidth, newHeight );
    resize( newWidth, newHeight );
    YLayoutBox::setSize( newWidth, newHeight );
}


void YQLayoutBox::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


#include "YQLayoutBox.moc"
