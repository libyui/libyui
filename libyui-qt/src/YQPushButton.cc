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

  File:	      YQPushButton.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#include <qevent.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQPushButton.h"
#include "YQDialog.h"


#define BORDER 3
#define BORDERSIZE QSize(BORDER, BORDER)
#define FOCUS_CHANGES_DEFAULT_BUTTON 0

YQPushButton::YQPushButton( 			    QWidget *	parent,
			    YQDialog *	dialog,
			    YWidgetOpt & opt,
			    YCPString 	label )
    : QWidget( parent )
    , YPushButton( opt, label )
    , _dialog( dialog )
{
    setWidgetRep( this );
    _qPushButton = new QPushButton( fromUTF8(label->value()), this);
    _qPushButton->setFont( YUIQt::ui()->currentFont() );
    _qPushButton->setMinimumSize( 2, 2 );
    _qPushButton->setAutoDefault( true );
    _qPushButton->installEventFilter( this );
    _qPushButton->move(BORDER, BORDER );
    setMinimumSize( _qPushButton->minimumSize() + 2 * BORDERSIZE );
    
    connect( _qPushButton, SIGNAL( clicked() ),
	     this,          SLOT  ( hit()     ) );

    _isDefault = opt.isDefaultButton.value();

    CHECK_PTR( _dialog );

    if ( _isDefault )
	_dialog->setDefaultButton( this );
}


YQPushButton::~YQPushButton()
{
    if ( _dialog->focusButton() == this )
	_dialog->losingFocus( this );
		
    if ( _dialog->defaultButton() == this )
	_dialog->setDefaultButton( 0 );
}


void YQPushButton::setEnabling( bool enabled )
{
    _qPushButton->setEnabled(enabled);
    YWidget::setEnabling( enabled );
}


void YQPushButton::setIcon( const YCPString & y_icon_name )
{
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


long YQPushButton::nicesize(YUIDimension dim)
{
    return 2 * BORDER + (dim == YD_HORIZ
			 ? _qPushButton->sizeHint().width()
			 : _qPushButton->sizeHint().height());
}


void YQPushButton::setSize( long newWidth, long newHeight )
{
    _qPushButton->resize(newWidth - 2 * BORDER, newHeight - 2 * BORDER);
    resize(newWidth, newHeight);
}


void YQPushButton::setLabel( const YCPString & label )
{
    _qPushButton->setText(fromUTF8(label->value()));
    YPushButton::setLabel( label );
}


void YQPushButton::showAsDefault( bool show )
{
    _qPushButton->setDefault( show );
    update();
}


bool YQPushButton::isShownAsDefault() const
{
    return _qPushButton->isDefault();
}


QString
YQPushButton::text() const
{
    return _qPushButton->text();
}


void YQPushButton::activate()
{
    _qPushButton->animateClick();
}


void YQPushButton::hit()
{
    YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


bool YQPushButton::eventFilter( QObject * obj, QEvent * event )
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
    
    return QWidget::eventFilter( obj, event );
}


bool YQPushButton::setKeyboardFocus()
{
    _dialog->gettingFocus( this );
    _qPushButton->setFocus();

    return true;
}


#include "YQPushButton.moc.cc"
