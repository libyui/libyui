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

  textdomain "qt-packages"

/-*/


#include <qlineedit.h>
#include <qlabel.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "QY2CharValidator.h"
#include "YQTextEntry.h"
#include "YQi18n.h"


YQTextEntry::YQTextEntry( QWidget * 		parent,
			  const YWidgetOpt & 	opt,
			  const YCPString & 	label,
			  const YCPString & 	text )
    : QVBox( parent )
    , YTextEntry( opt, label )
    , _validator(0)
    , _displayingCapsLockWarning( false )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( label->value() == "" )
	_qt_label->hide();

    _qt_lineEdit = new YQRawLineEdit( this );
    _qt_lineEdit->setFont( YQUI::ui()->currentFont() );
    _qt_lineEdit->setText( fromUTF8(text->value() ) );

    _qt_label->setBuddy( _qt_lineEdit );

    _shrinkable = opt.isShrinkable.value();

    connect( _qt_lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this,         SLOT  ( changed    ( const QString & ) ) );

    if ( opt.passwordMode.value() )
    {
	_qt_lineEdit->setEchoMode( QLineEdit::Password );

	connect( _qt_lineEdit,	SIGNAL( capsLockActivated() ),
		 this,		SLOT  ( displayCapsLockWarning() ) );

	connect( _qt_lineEdit,	SIGNAL( capsLockDeactivated() ),
		 this,		SLOT  ( clearCapsLockWarning() ) );
    }
}


void YQTextEntry::setEnabling( bool enabled )
{
    _qt_lineEdit->setEnabled( enabled );
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
	_qt_lineEdit->blockSignals( true );
	_qt_lineEdit->setText( text );
	_qt_lineEdit->blockSignals( false );
    }
    else
    {
	y2error( "%s \"%s\": Rejecting invalid value \"%s\"",
		 widgetClass(), debugLabel().c_str(), ytext->value().c_str() );
    }
}


YCPString YQTextEntry::getText()
{
    return YCPString( toUTF8(_qt_lineEdit->text() ) );
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
	_qt_lineEdit->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    if ( ! isValidText( _qt_lineEdit->text() ) )
    {
	y2error( "Old value \"%s\" of %s \"%s\" invalid according to ValidChars \"%s\" - deleting",
		 (const char *) _qt_lineEdit->text(),
		 widgetClass(), debugLabel().c_str(),
		 newValidChars->value().c_str() );
	_qt_lineEdit->setText( "" );
    }

    YTextEntry::setValidChars( newValidChars );
}

void YQTextEntry::setInputMaxLength( const YCPInteger & numberOfChars)
{
	_qt_lineEdit->setMaxLength(numberOfChars->asInteger()->value());
}

bool YQTextEntry::setKeyboardFocus()
{
    _qt_lineEdit->setFocus();
    _qt_lineEdit->selectAll();

    return true;
}


void YQTextEntry::changed( const QString & )
{
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQTextEntry::displayCapsLockWarning()
{
    y2milestone( "warning" );
    if ( _displayingCapsLockWarning )
	return;

    if ( _qt_lineEdit->echoMode() == QLineEdit::Normal )
	return;

    // Translators: This is a very short warning that the CapsLock key
    // is active while trying to type in a password field. This warning
    // replaces the normal label (caption) of that password field while
    // CapsLock is active, so please keep it short. Please don't translate it
    // at all if the term "CapsLock" can reasonably expected to be understood
    // by the target audience.
    //
    // In particular, please don't translate this to death in German.
    // Simply leave it.

    _qt_label->setText( _( "CapsLock!" ) );
    _qt_label->setFont( YQUI::ui()->boldFont() );
    _displayingCapsLockWarning = true;
}


void YQTextEntry::clearCapsLockWarning()
{
    y2milestone( "warning off " );
    if ( ! _displayingCapsLockWarning )
	return;

    if ( _qt_lineEdit->echoMode() == QLineEdit::Normal )
	return;

    _qt_label->setText( fromUTF8( getLabel()->value() ) );
    _qt_label->setFont( YQUI::ui()->currentFont() );
    _displayingCapsLockWarning = false;
}


bool YQRawLineEdit::x11Event( XEvent * event )
{
    // Qt (3.x) does not have support for the CapsLock key.
    // All other modifiers (Shift, Control, Meta) are propagated via
    // Qt's events, but for some reason, CapsLock is not.
    //
    // So let's examine the raw X11 event here to check for the
    // CapsLock status. All events are really handled on the parent class
    // (QWidget) level, though. We only peek into the modifier states.

    if ( event )
    {
	bool oldCapsLockActive = _capsLockActive;

	switch ( event->type )
	{
	    case KeyPress:
		_capsLockActive = (bool) ( event->xkey.state & LockMask );
		break;

	    case KeyRelease:

		_capsLockActive = (bool) ( event->xkey.state & LockMask );

		if ( _capsLockActive && oldCapsLockActive )
		{
		    KeySym key = XLookupKeysym( &(event->xkey), 0 );

		    if ( key == XK_Caps_Lock ||
			 key == XK_Shift_Lock  )
		    {
			y2milestone( "CapsLock released" );
			_capsLockActive = false;
		    }
		}

		y2debug( "Key event; caps lock: %s", _capsLockActive ? "on" : "off" );
		break;

	    case ButtonPress:
	    case ButtonRelease:
		_capsLockActive = (bool) ( event->xbutton.state & LockMask );
		break;

	    case EnterNotify:
		_capsLockActive = (bool) ( event->xcrossing.state & LockMask );
		break;

	    case LeaveNotify:
		_capsLockActive = false;
		break;

	    default:
		break;
	}

	if ( oldCapsLockActive != _capsLockActive )
	{
	    y2milestone( "Emitting warning" );

	    if ( _capsLockActive )
		emit capsLockActivated();
	    else
		emit capsLockDeactivated();
	}
    }

    return false; // handle this event at the Qt level
}


#include "YQTextEntry.moc"
