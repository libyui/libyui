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

  File:	      YQSpacing.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQSpacing.h"


YQSpacing::YQSpacing( YWidget *		parent,
		      YUIDimension 	dim,
		      bool 		stretchable,
		      YLayoutSize_t 	layoutUnits )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YSpacing( parent, dim, stretchable, layoutUnits )
{
    setWidgetRep( this );
}


YQSpacing::~YQSpacing()
{
    // NOP
}


void YQSpacing::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}



#include "YQSpacing.moc"
