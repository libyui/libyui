/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YQCheckBoxFrame.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <qcheckbox.h>
#include <QDebug>
#include <QVBoxLayout>
#include <qevent.h>
#include "YQUI.h"
#include <yui/YEvent.h>
#include "utf8.h"

using std::max;

#include "YQCheckBoxFrame.h"

#define TOP_MARGIN 6


YQCheckBoxFrame::YQCheckBoxFrame( YWidget * 		parent,
				  const std::string &	label,
				  bool			checked )
    : QGroupBox( (QWidget *) parent->widgetRep() )
    , YCheckBoxFrame( parent, label, checked)
{
    setWidgetRep ( this );
    QGroupBox::setTitle( fromUTF8( label ) );
    QGroupBox::setCheckable( true );
    setValue( checked );

    connect( this, &pclass(this)::toggled,
             this, &pclass(this)::stateChanged );
}


void YQCheckBoxFrame::setLabel( const std::string & newLabel )
{
    YCheckBoxFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


bool YQCheckBoxFrame::value()
{
    return QGroupBox::isChecked();
}


void YQCheckBoxFrame::setValue( bool newValue )
{
    setChecked( newValue );
    setEnabled( newValue );
}


void YQCheckBoxFrame::setEnabled( bool enabled )
{
    if ( enabled )
    {
	QGroupBox::setEnabled( true );
	handleChildrenEnablement( value() );
    }
    else
    {
	QGroupBox::setEnabled( true );
	YWidget::setChildrenEnabled( false );
    }

    YWidget::setEnabled( enabled );
}


void YQCheckBoxFrame::stateChanged( bool newState )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQCheckBoxFrame::event( QEvent *e )
{
    bool oldChildEnabled = true;

    if ( YCheckBoxFrame::hasChildren() )
	oldChildEnabled = YCheckBoxFrame::firstChild()->isEnabled();

    bool oldStatus = QGroupBox::isChecked();
    bool ret = QGroupBox::event( e );
    bool newStatus = QGroupBox::isChecked();

    if ( oldStatus != newStatus )
    {
	yuiDebug() << "Status change of " << this << " : now " << std::boolalpha << newStatus << std::endl;

	if ( autoEnable() )
	{
	    handleChildrenEnablement( newStatus );
	}
	else
	{
	    if ( YCheckBoxFrame::hasChildren() )
		YCheckBoxFrame::firstChild()->setEnabled( oldChildEnabled );
	}
    }

    return ret;
}


void YQCheckBoxFrame::childEvent( QChildEvent * event )
{
    if ( event->added() )
    {
	// yuiDebug() << "Child widget added" << std::endl;

	// Prevent parent class from disabling child widgets according to its
	// own policy: YCheckBoxFrame is much more flexible than QGroupBox.
    }
    else
	QGroupBox::childEvent( event );
}


void
YQCheckBoxFrame::setSize( int newWidth, int newHeight )
{
    resize ( newWidth, newHeight );

    if ( hasChildren() )
    {
        int left, top, right, bottom;
        getContentsMargins( &left, &top, &right, &bottom );
	int newChildWidth  = newWidth - left - right;
	int newChildHeight = newHeight - bottom - top;

	firstChild()->setSize( newChildWidth, newChildHeight );

	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( left, top );
    }
}


int YQCheckBoxFrame::preferredWidth()
{
    int preferredWidth = hasChildren() ? firstChild()->preferredWidth() : 0;
    int left, top, right, bottom;
    getContentsMargins( &left, &top, &right, &bottom );

    return preferredWidth + left + right;
}


int YQCheckBoxFrame::preferredHeight()
{
    int preferredHeight = hasChildren() ? firstChild()->preferredHeight() : 0;
    int left, top, right, bottom;
    getContentsMargins( &left, &top, &right, &bottom );

    return preferredHeight + top + left;
}


bool YQCheckBoxFrame::setKeyboardFocus()
{
    setFocus();

    return true;
}





