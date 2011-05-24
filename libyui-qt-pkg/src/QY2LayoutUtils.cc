/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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


#include <qapplication.h>
#include <qwidget.h>
#include <QDesktopWidget>
#include "QY2LayoutUtils.h"


QWidget * addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert

    return spacer;
}


QWidget * addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert

    return spacer;
}


QWidget * addVSpacing( QWidget * parent, int height )
{
    QWidget * spacer = new QWidget( parent );
    Q_CHECK_PTR( spacer );
    spacer->setFixedHeight( height );

    return spacer;
}


QWidget * addHSpacing( QWidget * parent, int width )
{
    QWidget * spacer = new QWidget( parent );
    Q_CHECK_PTR( spacer );
    spacer->setFixedWidth( width );

    return spacer;
}


QSize
limitToScreenSize( const QWidget * widget, int width, int height )
{
    return limitToScreenSize( widget, QSize( width, height ) );
}


QSize
limitToScreenSize( const QWidget * widget, const QSize & desiredSize )
{
    QSize availableSize = QApplication::desktop()->availableGeometry( const_cast<QWidget*> (widget) ).size();

    // Subtract WM decorations. There seems to be no reliable way to tell if
    // this is necessary at all (even fvwm2 claims it is a NETWM compliant
    // window manager) or how large the WM decorations are.
    // For the purpose of this function, let's assume we have to subtract the
    // common fvwm2 decoration size. This is simplistic and should be improved.
    availableSize -= QSize( 10, 35 );

    return desiredSize.boundedTo( availableSize );
}

