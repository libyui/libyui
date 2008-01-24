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

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include <QColorGroup>
#include <QScrollBar>
#include <QRegExp>
#include <QKeyEvent>
#include <QVBoxLayout>

#include "YEvent.h"
#include "utf8.h"
#include "YQUI.h"
#include "YQDialog.h"
#include "YQRichText.h"


YQRichText::YQRichText( YWidget * parent, const string & text, bool plainTextMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YRichText( parent, text, plainTextMode )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 0 );
    setLayout( layout );

    setWidgetRep( this );

    layout->setMargin( YQWidgetMargin );

    _textBrowser = new YQTextBrowser( this );
    YUI_CHECK_NEW( _textBrowser );
    layout->addWidget( _textBrowser );

    _textBrowser->installEventFilter( this );
    _textBrowser->setFrameShape( QFrame::NoFrame );

    if ( plainTextMode )
    {
        _textBrowser->setPlainText( fromUTF8( text ) );
        _textBrowser->setWordWrapMode( QTextOption::NoWrap );
    }
    else
    {
        _textBrowser->setHtml( fromUTF8( text ) );
    }
  
    setValue( text );

    // Set the text foreground color to black, regardless of its current
    // settings - it might be changed if this widget resides in a
    // warnColor dialog - which we cannot find right now out since our
    // parent is not set yet :-(

    QPalette pal( _textBrowser->palette() );
    pal.setColor( QPalette::Text, Qt::black );
    pal.setColor( QPalette::Base, QColor(0,0,0,0));
    _textBrowser->setPalette( pal );


    // Propagate clicks on hyperlinks

    connect( _textBrowser, SIGNAL( anchorClicked( const QUrl & ) ),
	     this,	   SLOT  ( linkClicked  ( const QUrl & ) ) );
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
    {
          text.replace( "&product;", YQUI::ui()->productName() );
          _textBrowser->setHtml( text );
    }
    else
    {
          _textBrowser->setPlainText( text );
    }
    YRichText::setValue( newText );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maximum() );
}


void YQRichText::setPlainTextMode( bool newPlainTextMode )
{
    YRichText::setPlainTextMode( newPlainTextMode );

    if ( plainTextMode() )
    {
      _textBrowser->setWordWrapMode( QTextOption::NoWrap );
    }
}


void YQRichText::setAutoScrollDown( bool newAutoScrollDown )
{
    YRichText::setAutoScrollDown( newAutoScrollDown );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maximum() );
}


void YQRichText::linkClicked( const QUrl & url )
{
    // yuiDebug() << "Selected hyperlink \"" << url.toString() << "\" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( url.toString().toUtf8()) );
}


bool YQRichText::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * event = ( QKeyEvent * ) ev;

	if ( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
	     ( event->modifiers() & Qt::NoModifier || event->modifiers() & Qt::KeypadModifier ) &&
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

    return ( _textBrowser->document()->toPlainText().contains( QRegExp( "<a\\s+href\\s*=", Qt::CaseInsensitive ) ) > 0 );
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
