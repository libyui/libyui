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


#include <qmultilineedit.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "Y2QtComponent.h"
#include "YEvent.h"
#include "YQMultiLineEdit.h"

#define VISIBLE_LINES	3


YQMultiLineEdit::YQMultiLineEdit( QWidget * parent, YWidgetOpt & opt,
				  const YCPString & label, const YCPString & initialText )
    : QVBox( parent )
    , YMultiLineEdit( opt, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    if ( label->value().length() > 0 )
    {
	_qt_label = new QLabel( fromUTF8( label->value() ), this );
	_qt_label->setTextFormat( QLabel::PlainText );
	_qt_label->setFont( Y2QtComponent::ui()->currentFont() );
    }
    else
	_qt_label = 0;

    _qt_textedit = new QTextEdit( this );
    _qt_textedit->setTextFormat( Qt::PlainText );
    _qt_textedit->setFont( Y2QtComponent::ui()->currentFont() );
    _qt_textedit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_textedit->setText( fromUTF8( initialText->value() ) );

    if ( _qt_label )
	_qt_label->setBuddy( _qt_textedit );

    connect( _qt_textedit, SIGNAL( textChanged( void ) ), this, SLOT( changed ( void ) ) );
}


void YQMultiLineEdit::setEnabling( bool enabled )
{
    _qt_textedit->setEnabled( enabled );
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
	long hintHeight	 = VISIBLE_LINES * _qt_textedit->fontMetrics().lineSpacing();
	hintHeight	+= _qt_textedit->frameWidth() * 2 + YQWidgetMargin * 2;

	if ( _qt_label && _qt_label->isShown() )
	    hintHeight	+= _qt_label->sizeHint().height() + YQWidgetSpacing;

	return max( minSize, hintHeight );
    }
}


void YQMultiLineEdit::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQMultiLineEdit::setText( const YCPString & text )
{
    _qt_textedit->blockSignals( true );
    _qt_textedit->setText( fromUTF8( text->value() ) );
    _qt_textedit->blockSignals( false );
}


YCPString YQMultiLineEdit::text()
{
    return YCPString( toUTF8( _qt_textedit->text() ) );
}


void YQMultiLineEdit::setLabel( const YCPString & label )
{
    if ( _qt_label )
	_qt_label->setText( fromUTF8( label->value() ) );
    YMultiLineEdit::setLabel( label );
}


bool YQMultiLineEdit::setKeyboardFocus()
{
    _qt_textedit->setFocus();

    return true;
}


void YQMultiLineEdit::changed()
{
    if ( getNotify() )
	Y2QtComponent::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQMultiLineEdit.moc"

