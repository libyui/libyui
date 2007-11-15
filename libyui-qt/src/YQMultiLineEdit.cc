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
#include "YQUI.h"
#include "YEvent.h"
#include "YQMultiLineEdit.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"


YQMultiLineEdit::YQMultiLineEdit( YWidget * parent, const string & label )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YMultiLineEdit( parent, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    
    _qt_textEdit = new QTextEdit( this );
    YUI_CHECK_NEW( _qt_textEdit );
    
    _qt_textEdit->setTextFormat( Qt::PlainText );
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
    return toUTF8( _qt_textEdit->text() );
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

    QString text = _qt_textEdit->text();

    if ( (int) text.length() > inputMaxLength() )
    {
	text.truncate( inputMaxLength() );
	_qt_textEdit->setText(text);
    }
}


void YQMultiLineEdit::enforceMaxInputLength()
{
    if ( inputMaxLength() >= 0 && _qt_textEdit->length() > inputMaxLength() )
    {
	int index;
	int para;

	_qt_textEdit->getCursorPosition( &para, &index);

	QString text = _qt_textEdit->text();

	int pos = 0; // current positon in text
	int section =0; // section in text;
	// iterate over the string
	
	while ( pos != (int) text.length()+1 )
	{
	    // we reached the paragraph where the user entered
	    // a character
	    if ( section == para )
	    {
		// remove that character
		text.remove( pos+index-1, 1 );
		break;
	    }

	    // new paragraph begins
	    if ( text[pos] == '\n' )
		section++;

	    pos++;
	}

	_qt_textEdit->setText( text );

	// user removed a paragraph
	if ( index == 0 )
	{
	    --para;
	    // the new index is the end of the previous paragraph
	    index = _qt_textEdit->paragraphLength(para) + 1;
	}

	// adjust to new cursor position before the removed character
	_qt_textEdit->setCursorPosition( para, index-1 );
    }
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

    if ( _caption->isShown() )
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

