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


#include <QVBoxLayout>
#include <QTextEdit>
#include <qlabel.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQMultiLineEdit.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"


YQMultiLineEdit::YQMultiLineEdit( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMultiLineEdit( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_textEdit = new QTextEdit( this );
    YUI_CHECK_NEW( _qt_textEdit );
    layout->addWidget( _qt_textEdit );

    _qt_textEdit->setAcceptRichText( false );
    _qt_textEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    _caption->setBuddy( _qt_textEdit );

    connect( _qt_textEdit,	SIGNAL( textChanged( void ) ),
	     this, 		SLOT  ( changed    ( void ) ) );
}


YQMultiLineEdit::~YQMultiLineEdit()
{
    // NOP
}


string YQMultiLineEdit::value()
{
    return toUTF8( _qt_textEdit->document()->toPlainText() );
}


void YQMultiLineEdit::setValue( const string & text )
{
    YQSignalBlocker sigBlocker( _qt_textEdit );

    _qt_textEdit->setText( fromUTF8( text ) );
}


void YQMultiLineEdit::setLabel( const string & label )
{
    _caption->setText( label );
    YMultiLineEdit::setLabel( label );
}


void YQMultiLineEdit::setInputMaxLength( int newMaxLength )
{
    YMultiLineEdit::setInputMaxLength( newMaxLength );

    QString text = _qt_textEdit->document()->toPlainText();

    if ( (int) text.length() > inputMaxLength() )
    {
	text.truncate( inputMaxLength() );
	_qt_textEdit->setText(text);
    }
}


void YQMultiLineEdit::enforceMaxInputLength()
{
    if ( inputMaxLength() >= 0 && _qt_textEdit->toPlainText().length() > inputMaxLength() )
        _qt_textEdit->undo();
}


void YQMultiLineEdit::changed()
{
    enforceMaxInputLength();

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQMultiLineEdit::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_textEdit->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQMultiLineEdit::preferredWidth()
{
    return max( 30, sizeHint().width() );
}


int YQMultiLineEdit::preferredHeight()
{
    int hintHeight 	 = defaultVisibleLines() * _qt_textEdit->fontMetrics().lineSpacing();
    hintHeight		+= _qt_textEdit->frameWidth() * 2 + YQWidgetMargin * 2;

    if ( !_caption->isHidden() )
	hintHeight += _caption->sizeHint().height() + YQWidgetSpacing;

    return max( 10, hintHeight );
}


void YQMultiLineEdit::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQMultiLineEdit::setKeyboardFocus()
{
    _qt_textEdit->setFocus();

    return true;
}


#include "YQMultiLineEdit.moc"

