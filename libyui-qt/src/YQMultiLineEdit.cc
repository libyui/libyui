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

  File:	      YQMultiLineEdit.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlineedit.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YUIQt.h"
#include "YQMultiLineEdit.h"


YQMultiLineEdit::YQMultiLineEdit( QWidget * parent, YWidgetOpt & opt,
				  const YCPString & label, const YCPString & initialText )
    : QVBox( parent )
    , YMultiLineEdit( opt, label )
{
    setWidgetRep( this );

    qt_label = new QLabel( fromUTF8(label->value() ), this );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont( YUIQt::ui()->currentFont() );

    qt_multiLineEdit = new QMultiLineEdit( this );
    qt_multiLineEdit->setFont( YUIQt::ui()->currentFont() );
    qt_multiLineEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ));
    qt_multiLineEdit->setText( fromUTF8( initialText->value() ) );
    qt_label->setBuddy( qt_multiLineEdit );

    connect(qt_multiLineEdit, SIGNAL( textChanged( void ) ), this, SLOT( changed ( void ) ) );
}


void YQMultiLineEdit::setEnabling(bool enabled)
{
    qt_multiLineEdit->setEnabled( enabled );
}


long YQMultiLineEdit::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ)
    {
	long minSize   	 = 30;
	long hintWidth 	 = sizeHint().width();

	return max( minSize, hintWidth );
    }
    else
    {
	long minSize    	 = 10;
	long hintHeight	 = qt_label->sizeHint().height();
	hintHeight 	+= visibleLines() * qt_multiLineEdit->fontMetrics().lineSpacing();
	hintHeight	+= qt_multiLineEdit->frameWidth() * 2;

	return max( minSize, hintHeight );
    }
}


void YQMultiLineEdit::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}


void YQMultiLineEdit::setText( const YCPString & text)
{
    qt_multiLineEdit->setText( fromUTF8( text->value() ) );
}


YCPString YQMultiLineEdit::text()
{
    return YCPString( toUTF8( qt_multiLineEdit->text() ) );
}


void YQMultiLineEdit::setLabel(const YCPString & label)
{
    qt_label->setText( fromUTF8( label->value() ) );
    YMultiLineEdit::setLabel(label);
}


bool YQMultiLineEdit::setKeyboardFocus()
{
    qt_multiLineEdit->setFocus();

    return true;
}


// slots

void YQMultiLineEdit::changed()
{
    if (getNotify())
	YUIQt::ui()->returnNow(YUIInterpreter::ET_WIDGET, this);
}




#include "YQMultiLineEdit.moc.cc"

