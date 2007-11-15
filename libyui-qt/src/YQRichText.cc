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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "qregexp.h"
#include "YEvent.h"
#include "utf8.h"
#include "YQUI.h"
#include "YQDialog.h"
#include "YQRichText.h"


YQRichText::YQRichText( YWidget * parent, const string & text, bool plainTextMode )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YRichText( parent, text, plainTextMode )
{
    setWidgetRep( this );

    setMargin( YQWidgetMargin );

    _textBrowser = new YQTextBrowser( this );
    YUI_CHECK_NEW( _textBrowser );
    
    _textBrowser->setMimeSourceFactory( 0 );
    _textBrowser->installEventFilter( this );

    if ( plainTextMode )
    {
	_textBrowser->setTextFormat( Qt::PlainText );
	_textBrowser->setWordWrap( QTextEdit::NoWrap );
    }
    else
    {
	_textBrowser->setTextFormat( Qt::RichText );
    }

    setValue( text );


    // Set the text foreground color to black, regardless of its current
    // settings - it might be changed if this widget resides in a
    // warnColor dialog - which we cannot find right now out since our
    // parent is not set yet :-(

    QPalette pal( _textBrowser->palette() );
    QColorGroup normalColors( pal.normal() );
    normalColors.setColor( QColorGroup::Text, black );
    pal.setNormal( normalColors );
    _textBrowser->setPalette( pal );

    // Set the text background to a light grey

    _textBrowser->setPaper( QColor( 234, 234, 234 ) );


    // Propagate clicks on hyperlinks

    connect( _textBrowser, SIGNAL( linkClicked( const QString & ) ),
	     this,	   SLOT  ( linkClicked( const QString & ) ) );
}


YQRichText::~YQRichText()
{
    // NOP
}


void YQRichText::setValue( const string & newText )
{
    if ( _textBrowser->horizontalScrollBar() )
	_textBrowser->horizontalScrollBar()->setValue(0);

    if ( ! autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue(0);

    QString text = fromUTF8( newText );

    if ( ! plainTextMode() )
	text.replace( "&product;", YQUI::ui()->productName() );

    YRichText::setValue( newText );
    _textBrowser->setText( text );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maxValue() );
}


void YQRichText::setPlainTextMode( bool newPlainTextMode )
{
    YRichText::setPlainTextMode( newPlainTextMode );

    if ( plainTextMode() )
    {
	_textBrowser->setTextFormat( Qt::PlainText );
	_textBrowser->setWordWrap( QTextEdit::NoWrap );
    }
    else
    {
	_textBrowser->setTextFormat( Qt::RichText );
    }
}


void YQRichText::setAutoScrollDown( bool newAutoScrollDown )
{
    YRichText::setAutoScrollDown( newAutoScrollDown );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maxValue() );
}


void YQRichText::linkClicked( const QString & url )
{
    // y2debug( "Selected hyperlink \"%s\"", (const char *) url );
    YQUI::ui()->sendEvent( new YMenuEvent( YCPString( (const char *) url ) ) );
}


bool YQRichText::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * event = ( QKeyEvent * ) ev;

	if ( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
	     ( event->state() == 0 || event->state() == Qt::Keypad ) &&
	     ! haveHyperLinks() )
	{
	    YQDialog * dia = (YQDialog *) findDialog();

	    if ( dia )
	    {
		( void ) dia->activateDefaultButton();
		return true;
	    }
	}
    }

    return QWidget::eventFilter( obj, ev );
}


bool YQRichText::haveHyperLinks()
{
    if ( plainTextMode() )
	return false;
    
    return ( _textBrowser->text().contains( QRegExp( "<a\\s+href\\s*=", false ) ) > 0 );
}


int YQRichText::preferredWidth()
{
    return shrinkable() ? 10 : 100;
}


int YQRichText::preferredHeight()
{
    return shrinkable() ? 10 : 100;
}


void YQRichText::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQRichText::setEnabled( bool enabled )
{
    _textBrowser->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQRichText::setKeyboardFocus()
{
    _textBrowser->setFocus();

    return true;
}



#include "YQRichText.moc"
