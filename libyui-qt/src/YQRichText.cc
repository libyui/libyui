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
#include "Y2QtComponent.h"
#include "YQDialog.h"
#include "YQRichText.h"


YQRichText::YQRichText( QWidget * 		parent,
			YWidgetOpt & 		opt,
			const YCPString &	text )
    : QVBox( parent )
    , YRichText( opt, text )
{
    setWidgetRep( this );

    setMargin( YQWidgetMargin );

    _textBrowser = new YQTextBrowser( this );
    _textBrowser->setMimeSourceFactory( 0 );
    _textBrowser->setFont( Y2QtComponent::ui()->currentFont() );
    _textBrowser->installEventFilter( this );

    if ( opt.plainTextMode.value() )
    {
	_textBrowser->setTextFormat( Qt::PlainText );
	_textBrowser->setWordWrap( QTextEdit::NoWrap );
    }
    else
    {
	_textBrowser->setTextFormat( Qt::RichText );
    }

    setText( text );


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

    // Very small default size if specified

    _shrinkable = opt.isShrinkable.value();


    // Propagate clicks on hyperlinks

    connect( _textBrowser, SIGNAL( linkClicked( const QString & ) ),
	     this,	   SLOT  ( linkClicked( const QString & ) ) );
}


void YQRichText::setEnabling( bool enabled )
{
    _textBrowser->setEnabled( enabled );
}


long YQRichText::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return _shrinkable ? 10 : 100;
    else 			return _shrinkable ? 10 : 100;
}


void YQRichText::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQRichText::setText( const YCPString & ytext )
{
    if ( _textBrowser->horizontalScrollBar() )
	_textBrowser->horizontalScrollBar()->setValue(0);

    if ( ! autoScrollDown && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue(0);

    QString text = fromUTF8( ytext->value() );

    if ( _textBrowser->textFormat() != Qt::PlainText )
	text.replace( "&product;", Y2QtComponent::ui()->productName() );

    YRichText::setText( ytext );
    _textBrowser->setText( text );

    if ( autoScrollDown && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maxValue() );
}


bool YQRichText::setKeyboardFocus()
{
    _textBrowser->setFocus();

    return true;
}


void YQRichText::linkClicked( const QString & url )
{
    // y2debug( "Selected hyperlink \"%s\"", (const char *) url );
    Y2QtComponent::ui()->sendEvent( new YMenuEvent( YCPString( (const char *) url ) ) );
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
	    YQDialog * dia = ( YQDialog * ) yDialog();

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
    if ( _textBrowser->textFormat() == Qt::PlainText )
	return false;
    
    return ( _textBrowser->text().contains( QRegExp( "<a\\s+href\\s*=", false ) ) > 0 );
}



#include "YQRichText.moc"
