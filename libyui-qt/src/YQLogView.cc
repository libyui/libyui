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

  File:	      YQLogView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlineedit.h>
#include <qlabel.h>
#include <qstyle.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YUIQt.h"
#include "YQLogView.h"


YQLogView::YQLogView( QWidget * parent, YWidgetOpt & opt,
		     const YCPString & label, int visLines, int maxLines )
    : QVBox( parent )
    , YLogView( opt, label, visLines, maxLines )
{
    setWidgetRep( this );

    qt_label = new QLabel( fromUTF8(label->value() ), this );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont( YUIQt::ui()->currentFont() );

    qt_text = new QMultiLineEdit( this );
    qt_text->setFont( YUIQt::ui()->currentFont() );
    qt_text->setReadOnly( true );
    qt_text->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
}


void YQLogView::setEnabling(bool enabled)
{
    qt_text->setEnabled( enabled );
}


long YQLogView::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ)
    {
	long minSize   	 = 50;
	long hintWidth 	 = sizeHint().width();

	return max( minSize, hintWidth );
    }
    else
    {
	long minSize     = 10;
	long hintHeight	 = qt_label->sizeHint().height();
	hintHeight 	+= visibleLines() * qt_text->fontMetrics().lineSpacing();
	hintHeight	+= qt_text->style().scrollBarExtent().height();
	hintHeight	+= qt_text->frameWidth() * 2;

	return max( minSize, hintHeight );
    }
}


void YQLogView::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}


void YQLogView::setLogText( const YCPString & text)
{
    qt_text->setText( fromUTF8( text->value() ) );
    qt_text->scrollToBottom();
}


void YQLogView::setLabel(const YCPString & label)
{
    qt_label->setText( fromUTF8( label->value() ) );
    YLogView::setLabel(label);
}


bool YQLogView::setKeyboardFocus()
{
    qt_text->setFocus();

    return true;
}



#include "YQLogView.moc.cc"

