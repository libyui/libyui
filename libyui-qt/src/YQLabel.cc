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

  File:	      YQLabel.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQLabel.h"


YQLabel::YQLabel( QWidget * 		parent,
		  const YWidgetOpt &	opt,
		  YCPString 		text )
    : QLabel( parent )
    , YLabel( opt, text )
{
    setWidgetRep( this );

    setTextFormat( QLabel::PlainText );
    setText( fromUTF8(text->value() ) );
    setIndent(0);

    if ( opt.isHeading.value() )
    {
	setFont( YQUI::ui()->headingFont() );
	
    }
    else
    {
	setFont( opt.boldFont.value() ?
		 YQUI::ui()->boldFont() :
		 YQUI::ui()->currentFont() );
    }
		

    if ( opt.isOutputField.value() )
    {
	setFrameStyle ( QFrame::Panel | QFrame::Sunken );
	setLineWidth(2);
	setMidLineWidth(2);
    }

    setMargin( YQWidgetMargin );
    setAlignment( AlignLeft | AlignTop );
}


void YQLabel::setEnabling( bool enabled )
{
    QLabel::setEnabled( enabled );
}


long YQLabel::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}


void YQLabel::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQLabel::setLabel( const YCPString & text )
{
    setText( fromUTF8(text->value() ) );
    YLabel::setLabel( text );
}


#include "YQLabel.moc"
