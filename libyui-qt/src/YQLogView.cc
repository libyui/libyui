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

    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );

    _qt_text = new QMultiLineEdit( this );
    _qt_text->setFont( YUIQt::ui()->currentFont() );
    _qt_text->setReadOnly( true );
    _qt_text->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
}


void YQLogView::setEnabling( bool enabled )
{
    _qt_text->setEnabled( enabled );
}


long YQLogView::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	long minSize   	 = 50;
	long hintWidth 	 = sizeHint().width();

	return max( minSize, hintWidth );
    }
    else
    {
	long minSize     = 10;
	long hintHeight	 = _qt_label->sizeHint().height();
	hintHeight 	+= visibleLines() * _qt_text->fontMetrics().lineSpacing();
	hintHeight	+= _qt_text->style().scrollBarExtent().height();
	hintHeight	+= _qt_text->frameWidth() * 2;

	return max( minSize, hintHeight );
    }
}


void YQLogView::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQLogView::setLogText( const YCPString & text )
{
    _qt_text->setText( fromUTF8( text->value() ) );
    _qt_text->scrollToBottom();
}


void YQLogView::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8( label->value() ) );
    YLogView::setLabel( label );
}


bool YQLogView::setKeyboardFocus()
{
    _qt_text->setFocus();

    return true;
}



#include "YQLogView.moc.cc"

