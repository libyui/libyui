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

  File:	      YQGenericButton.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#include <qevent.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQGenericButton.h"
#include "YQDialog.h"


YQGenericButton::YQGenericButton( QWidget *		parent,
				  YQDialog *		dialog,
				  const YWidgetOpt &	opt,
				  YCPString 		label )
    : QWidget( parent )
    , YPushButton( opt, label )
    , _dialog( dialog )
{
    setWidgetRep( 0 );
    
    _isDefault = opt.isDefaultButton.value();
}


void YQGenericButton::setQPushButton( QPushButton * pb )
{
    _qPushButton = pb;
    _qPushButton->setAutoDefault( true );
    _qPushButton->installEventFilter( this );
    _qPushButton->setAutoDefault( true );

    YPushButton::setLabel( toUTF8 ( _qPushButton->text() ) );

    if ( _dialog && _isDefault )
	_dialog->setDefaultButton( this );
}


YQGenericButton::~YQGenericButton()
{
    if ( _dialog->focusButton() == this )
	_dialog->losingFocus( this );
		
    if ( _dialog->defaultButton() == this )
	_dialog->setDefaultButton(0);
}


void YQGenericButton::setEnabling( bool enabled )
{
    if ( _qPushButton )
	_qPushButton->setEnabled( enabled );
    
    YWidget::setEnabling( enabled );
}


bool YQGenericButton::isEnabled() const
{
    return _qPushButton ? _qPushButton->isEnabled() : false;
}


void YQGenericButton::setIcon( const YCPString & y_icon_name )
{
    if ( ! _qPushButton )
    {
	y2error( "NULL button (icon %s)", y_icon_name->value().c_str() );
	return;
    }
    
    QString icon_name = fromUTF8( y_icon_name->value() );

    if ( icon_name.isEmpty() )
    {
	_qPushButton->setIconSet( QIconSet() );
	return;
    }

    icon_name = QString( ICONDIR ) + "/" + icon_name;
    QPixmap icon( icon_name );

    if ( icon.isNull() )
	y2warning( "Can't load icon '%s'", (const char *) icon_name );
    else
	_qPushButton->setIconSet( icon );
}


void YQGenericButton::setLabel( const QString & label )
{
    if ( _qPushButton )
	_qPushButton->setText( label );
    else
	y2error( "NULL button '%s'", (const char *) label );
    
    YPushButton::setLabel( toUTF8( label ) );
}


void YQGenericButton::setLabel( const YCPString & label )
{
    if ( _qPushButton )
	_qPushButton->setText( fromUTF8( label->value() ) );
    else
	y2error( "NULL button '%s'", label->value().c_str() );
    
    YPushButton::setLabel( label );
}


void YQGenericButton::showAsDefault( bool show )
{
    if ( _qPushButton )
    {
	_qPushButton->setDefault( show );
	_qPushButton->update();
    }
}


bool YQGenericButton::isShownAsDefault() const
{
    return _qPushButton ? _qPushButton->isDefault() : false;
}


QString
YQGenericButton::text() const
{
    return _qPushButton ? _qPushButton->text() : "";
}


void YQGenericButton::activate()
{
    if ( _qPushButton )
	_qPushButton->animateClick();
}


bool YQGenericButton::eventFilter( QObject * obj, QEvent * event )
{
    if ( event->type() == QEvent::FocusIn )
    {
	_dialog->gettingFocus( this );
	return false;	// event processed?
    }
    else if ( event->type() == QEvent::FocusOut )
    {
	_dialog->losingFocus( this );
	return false;	// event processed?
    }
    
    return QObject::eventFilter( obj, event );
}


bool YQGenericButton::setKeyboardFocus()
{
    if ( ! _qPushButton )
	return false;
    
    _dialog->gettingFocus( this );
    _qPushButton->setFocus();

    return true;
}


#include "YQGenericButton.moc"
