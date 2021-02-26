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


YQSpacing::YQSpacing( QWidget * 		parent,
		      const YWidgetOpt & 	opt,
		      float 			size,
		      bool 			horizontal,
		      bool 			vertical )
    : QWidget( parent )
    , YSpacing( opt, size, horizontal, vertical )
{
    setWidgetRep( this );
}


void YQSpacing::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}



#include "YQSpacing.moc"
