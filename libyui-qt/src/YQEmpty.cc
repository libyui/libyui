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

  File:	      YQEmpty.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQEmpty.h"


YQEmpty::YQEmpty( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YEmpty( parent )
{
    setWidgetRep( this );
}


YQEmpty::~YQEmpty()
{
    // NOP
}


void YQEmpty::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


#include "YQEmpty.moc"
