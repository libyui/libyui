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

  File:	      YQPushButton.cc

  Author:     Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQPushButton.h"


YQPushButton::YQPushButton( QWidget *		parent,
			    YQDialog *		dialog,
			    const YWidgetOpt &	opt,
			    const YCPString &	label )
    : YQGenericButton( parent, dialog, opt, label )
{
    setWidgetRep( this );

    QPushButton * button = new QPushButton( fromUTF8( label->value() ), this );
    CHECK_PTR( button );

    setQPushButton( button );
    
    button->setFont( YQUI::ui()->currentFont() );
    button->setMinimumSize( 2, 2 );
    button->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( button->minimumSize()
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );
    
    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( hit()     ) );
}


YQPushButton::~YQPushButton()
{
}


long YQPushButton::nicesize( YUIDimension dim )
{
    return 2 * YQButtonBorder + ( dim == YD_HORIZ
				  ? qPushButton()->sizeHint().width()
				  : qPushButton()->sizeHint().height() );
}


void YQPushButton::setSize( long newWidth, long newHeight )
{
    qPushButton()->resize( newWidth  - 2 * YQButtonBorder,
			   newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


void YQPushButton::hit()
{
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


#include "YQPushButton.moc"
