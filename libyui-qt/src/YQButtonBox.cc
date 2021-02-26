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

  File:	      YQButtonBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "YQButtonBox.h"


YQButtonBox::YQButtonBox( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YButtonBox( parent )
{
    setWidgetRep( this );
}


YQButtonBox::~YQButtonBox()
{
    // NOP
}


void YQButtonBox::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQButtonBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YButtonBox::setSize( newWidth, newHeight );
}


void YQButtonBox::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


#include "YQButtonBox.moc"
