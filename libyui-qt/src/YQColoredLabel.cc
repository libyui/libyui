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

  File:	      YQColoredLabel.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQColoredLabel.h"


YQColoredLabel::YQColoredLabel( QWidget * parent, YWidgetOpt & opt,
				YCPString text,
				YColor & fg, YColor & bg,
				int margin )
    : QLabel( parent )
    , YLabel( opt, text )
{
    setWidgetRep( this );

    setTextFormat( QLabel::PlainText );
    setText( fromUTF8(text->value() ) );
    setIndent(0);
    setPaletteForegroundColor( QColor( fg.red, fg.green, fg.blue ) );
    setPaletteBackgroundColor( QColor( bg.red, bg.green, bg.blue ) );
    setMargin( margin );
    setAlignment( AlignLeft | AlignVCenter );

    setFont( YUIQt::ui()->currentFont() );

}


void YQColoredLabel::setEnabling( bool enabled )
{
    QLabel::setEnabled( enabled );
}


long YQColoredLabel::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}


void YQColoredLabel::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQColoredLabel::setLabel( const YCPString & text )
{
    setText( fromUTF8(text->value() ) );
    YLabel::setLabel( text );
}


#include "YQColoredLabel.moc.cc"
