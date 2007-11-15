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

  File:	      YQSquash.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQSquash.h"


YQSquash::YQSquash( YWidget *	parent,
		    bool 	horSquash,
		    bool 	vertSquash )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YSquash( parent, horSquash, vertSquash )
{
    setWidgetRep( this );
}


YQSquash::~YQSquash()
{
    // NOP
}


void YQSquash::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQSquash::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YSquash::setSize( newWidth, newHeight );
}


#include "YQSquash.moc"
