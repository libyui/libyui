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

  File:	      YQRichText.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YDialog.h"	// ???
#include "utf8.h"
#include "YUIQt.h"
#include "YQRichText.h"


YQRichText::YQRichText( QWidget *parent, YWidgetOpt &opt,
		       const YCPString& text)
    : QTextBrowser(parent)
    , YRichText(opt, text)
{
    setWidgetRep( this );
    setFont( YUIQt::ui()->currentFont() );
    setMargin(AlignRight);
    
    if ( opt.plainTextMode.value() )
    {
	setTextFormat( Qt::PlainText );
	setWordWrap( QTextEdit::NoWrap );
    }
    else
    {
	setTextFormat( Qt::RichText );
    }

    setText( text );


    // Set the text foreground color to black, regardless of its current
    // settings - it might be changed if this widget resides in a
    // warnColor dialog - which we cannot find right now out since our
    // parent is not set yet :-(

    QPalette pal(palette());
    QColorGroup normalColors(pal.normal());
    normalColors.setColor(QColorGroup::Text, black);
    pal.setNormal(normalColors);
    setPalette(pal);

    // Set the text background to a light grey

    setPaper(QColor( 234, 234, 234 ));

    // Very small default size if specified

    shrinkable = opt.isShrinkable.value();
}


void YQRichText::setEnabling(bool enabled)
{
    setEnabled(enabled);
}


long YQRichText::nicesize(YUIDimension dim)
{
    if (dim == YD_HORIZ) return shrinkable ? 10 : 100;
    else 		return shrinkable ? 10 :100;
}


void YQRichText::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}


void YQRichText::setText(const YCPString &text)
{
    if ( horizontalScrollBar() )
	horizontalScrollBar()->setValue(0);

    if ( ! autoScrollDown && verticalScrollBar() )
	verticalScrollBar()->setValue(0);
    
    QTextBrowser::setText( fromUTF8( text->value() ) );
    YRichText::setText( text );
    
    if ( autoScrollDown && verticalScrollBar() )
	verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
}


bool YQRichText::setKeyboardFocus()
{
    QTextBrowser::setFocus();

    return true;
}


void YQRichText::setSource( const QString & name )
{
    y2debug( "Selected hyperlink \"%s\"", (const char *) name );
    YUIQt::ui()->setMenuSelection( YCPString( (const char *) name ) );
    YUIQt::ui()->returnNow( YUIInterpreter::ET_MENU, this );
}


#include "YQRichText.moc.cc"
