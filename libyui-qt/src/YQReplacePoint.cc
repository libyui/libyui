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

  File:	      YQReplacePoint.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qwidget.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQReplacePoint.h"


YQReplacePoint::YQReplacePoint( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YReplacePoint( parent )
{
    setWidgetRep( this );
}


void YQReplacePoint::showChild()
{
    YWidget * child = firstChild();

    if ( child )
    {
	QWidget * qChild = (QWidget *) child->widgetRep();
	qChild->show();
    }
}


void YQReplacePoint::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQReplacePoint::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YReplacePoint::setSize( newWidth, newHeight );
}

#include "YQReplacePoint.moc"
