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
#include "YEvent.h"
#include "YQMultiLineEdit.h"


YQMultiLineEdit::YQMultiLineEdit( QWidget * parent, YWidgetOpt & opt,
				  const YCPString & label, const YCPString & initialText )
    : QVBox( parent )
    , YMultiLineEdit( opt, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );

    _qt_multiLineEdit = new QMultiLineEdit( this );
    _qt_multiLineEdit->setFont( YUIQt::ui()->currentFont() );
    _qt_multiLineEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_multiLineEdit->setText( fromUTF8( initialText->value() ) );
    _qt_label->setBuddy( _qt_multiLineEdit );

    connect( _qt_multiLineEdit, SIGNAL( textChanged( void ) ), this, SLOT( changed ( void ) ) );
}


void YQMultiLineEdit::setEnabling( bool enabled )
{
    _qt_multiLineEdit->setEnabled( enabled );
}


long YQMultiLineEdit::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	long minSize   	 = 30;
	long hintWidth 	 = sizeHint().width();

	return max( minSize, hintWidth );
    }
    else
    {
	long minSize     = 10;
	long hintHeight	 = _qt_label->sizeHint().height();
	hintHeight 	+= visibleLines() * _qt_multiLineEdit->fontMetrics().lineSpacing();
	hintHeight	+= _qt_multiLineEdit->frameWidth() * 2;

	return max( minSize, hintHeight );
    }
}


void YQMultiLineEdit::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQMultiLineEdit::setText( const YCPString & text )
{
    _qt_multiLineEdit->setText( fromUTF8( text->value() ) );
}


YCPString YQMultiLineEdit::text()
{
    return YCPString( toUTF8( _qt_multiLineEdit->text() ) );
}


void YQMultiLineEdit::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8( label->value() ) );
    YMultiLineEdit::setLabel( label );
}


bool YQMultiLineEdit::setKeyboardFocus()
{
    _qt_multiLineEdit->setFocus();

    return true;
}


// slots

void YQMultiLineEdit::changed()
{
    if ( getNotify() )
	YUIQt::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQMultiLineEdit.moc.cc"

