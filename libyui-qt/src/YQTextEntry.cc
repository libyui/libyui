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

  File:	      YQTextEntry.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlineedit.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "QY2CharValidator.h"
#include "YQTextEntry.h"


YQTextEntry::YQTextEntry( QWidget * 		parent,
			  const YWidgetOpt & 	opt,
			  const YCPString & 	label,
			  const YCPString & 	text )
    : QVBox( parent )
    , YTextEntry( opt, label )
    , _validator(0)
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( label->value() == "" )
	_qt_label->hide();

    _qt_lineedit = new QLineEdit( this );
    _qt_lineedit->setFont( YQUI::ui()->currentFont() );
    _qt_lineedit->setText( fromUTF8(text->value() ) );

    _qt_label->setBuddy( _qt_lineedit );

    if ( opt.passwordMode.value() )
	_qt_lineedit->setEchoMode( QLineEdit::Password );

    _shrinkable = opt.isShrinkable.value();

    connect( _qt_lineedit, SIGNAL( textChanged( const QString & ) ),
	     this,         SLOT  ( changed    ( const QString & ) ) );
}


void YQTextEntry::setEnabling( bool enabled )
{
    _qt_lineedit->setEnabled( enabled );
}


long YQTextEntry::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	long minSize	= _shrinkable ? 15 : 200;
	long hintWidth	= sizeHint().width();

	if ( _qt_label->text().isEmpty() )
	    hintWidth = 0;

	return max( minSize, hintWidth );
    }
    else
    {
	return sizeHint().height();
    }
}


void YQTextEntry::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}

void YQTextEntry::setText( const YCPString & ytext )
{
    QString text = fromUTF8( ytext->value() );
    
    if ( isValidText( text ) )
    {
	_qt_lineedit->blockSignals( true );
	_qt_lineedit->setText( text );
	_qt_lineedit->blockSignals( false );
    }
    else
    {
	y2error( "%s \"%s\": Rejecting invalid value \"%s\"",
		 widgetClass(), debugLabel().c_str(), ytext->value().c_str() );
    }
}


YCPString YQTextEntry::getText()
{
    return YCPString( toUTF8(_qt_lineedit->text() ) );
}


void YQTextEntry::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
    YTextEntry::setLabel( label );
}


bool YQTextEntry::isValidText( const QString & txt ) const
{
    if ( ! _validator )
	return true;

    int pos = 0;
    QString text( txt );	// need a non-const QString &
    
    return _validator->validate( text, pos ) == QValidator::Acceptable;
}


void YQTextEntry::setValidChars( const YCPString & newValidChars )
{
    if ( _validator )
    {
	_validator->setValidChars( fromUTF8( newValidChars->value() ) );
    }
    else
    {
	_validator = new QY2CharValidator( fromUTF8( newValidChars->value() ), this );
	_qt_lineedit->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    if ( ! isValidText( _qt_lineedit->text() ) )
    {
	y2error( "Old value \"%s\" of %s \"%s\" invalid according to ValidChars \"%s\" - deleting",
		 (const char *) _qt_lineedit->text(),
		 widgetClass(), debugLabel().c_str(),
		 newValidChars->value().c_str() );
	_qt_lineedit->setText( "" );
    }

    YTextEntry::setValidChars( newValidChars );
}

void YQTextEntry::setInputMaxLength( const YCPInteger & numberOfChars)
{
	_qt_lineedit->setMaxLength(numberOfChars->asInteger()->value());
}

bool YQTextEntry::setKeyboardFocus()
{
    _qt_lineedit->setFocus();
    _qt_lineedit->selectAll();

    return true;
}


void YQTextEntry::changed( const QString & )
{
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}




#include "YQTextEntry.moc"

