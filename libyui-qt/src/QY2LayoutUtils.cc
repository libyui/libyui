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

  File:	      QY2LayoutUtils.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  These are pure Qt functions - they can be used independently of YaST2.

/-*/


#include <qwidget.h>
#include "QY2LayoutUtils.h"


void addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert
}


void addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
}


void addVSpacing( QWidget * parent, int height )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedHeight( height );
}


void addHSpacing( QWidget * parent, int width )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedWidth( width );
}

