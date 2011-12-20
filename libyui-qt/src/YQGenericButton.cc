
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |*************************************************************************** */

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
#include <qpixmap.h>
#include <qevent.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YEvent.h"
#include "YQGenericButton.h"
#include "YQDialog.h"


YQGenericButton::YQGenericButton( YWidget *		parent,
				  const string &	label )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YPushButton( parent, label )
    , _dialog( 0 )
    , _qPushButton( 0 )
{
    setWidgetRep( 0 );
}


void YQGenericButton::setQPushButton( QPushButton * pb )
{
    _qPushButton = pb;
    _qPushButton->installEventFilter( this );
    _qPushButton->setAutoDefault( true );

    YPushButton::setLabel( toUTF8 ( _qPushButton->text() ) );
}


YQGenericButton::~YQGenericButton()
{
    if ( _dialog ) // If we don't have one any more, don't bother
    {
	if ( _dialog->focusButton() == this )
	    _dialog->losingFocus( this );

	if ( _dialog->defaultButton() == this )
	    _dialog->setDefaultButton(0);
    }
}


void YQGenericButton::forgetDialog()
{
   _dialog = 0;
}


YQDialog *
YQGenericButton::dialog()
{
    if ( ! _dialog )
    {
	YDialog * yDialog = findDialog();

	if ( yDialog )
	    _dialog = dynamic_cast<YQDialog *> (yDialog);

	YUI_CHECK_PTR( _dialog );
    }

    return _dialog;
}


void YQGenericButton::setEnabled( bool enabled )
{
    if ( _qPushButton )
	_qPushButton->setEnabled( enabled );

    YWidget::setEnabled( enabled );
}


bool YQGenericButton::isEnabled() const
{
    return _qPushButton ? _qPushButton->isEnabled() : false;
}


void YQGenericButton::setIcon( const string & iconName )
{
    if ( ! _qPushButton )
    {
	yuiError() << "NULL button (icon " << iconName << ")" << endl;
	return;
    }

    QString qIconName = fromUTF8( iconName );

    if ( qIconName.isEmpty() )
    {
	_qPushButton->setIcon( QIcon() );
	return;
    }

    // Search for the icon - FaTE #306356
    qIconName =  fromUTF8( YQUI::yqApp()->iconLoader()->findIcon( iconName ) );
    QPixmap icon( qIconName );

    if ( icon.isNull() )
	yuiWarning() << "Can't load icon \"" << qIconName << "\"" << endl;
    else
	_qPushButton->setIcon( icon );
}


void YQGenericButton::setLabel( const QString & label )
{
    if ( _qPushButton )
	_qPushButton->setText( label );
    else
	yuiError() << "NULL button \"" << label << "\"" << endl;

    YPushButton::setLabel( toUTF8( label ) );
}


void YQGenericButton::setLabel( const string & label )
{
    if ( _qPushButton )
	_qPushButton->setText( fromUTF8( label ) );
    else
	yuiError() << "NULL button \"" << label << "\"" << endl;

    YPushButton::setLabel( label );
}


void YQGenericButton::showAsDefault( bool show )
{
    if ( _qPushButton )
    {
        _qPushButton->setAutoDefault( !show );
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
    if ( event )
    {
	if ( event->type() == QEvent::FocusIn )
	{
	    dialog()->gettingFocus( this );
	    return false;	// event processed?
	}
	else if ( event->type() == QEvent::FocusOut )
	{
	    dialog()->losingFocus( this );
	    return false;	// event processed?
	}
	else if ( event->type() == QEvent::MouseButtonRelease )
	{
	    QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (event);

	    if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
	    {
		yuiMilestone() << "Right click on button detected" << endl;
		YQUI::yqApp()->maybeLeftHandedUser();
	    }
	}
    }


    return QObject::eventFilter( obj, event );
}


bool YQGenericButton::setKeyboardFocus()
{
    if ( ! _qPushButton )
	return false;

    dialog()->gettingFocus( this );
    _qPushButton->setFocus();

    return true;
}


#include "YQGenericButton.moc"
