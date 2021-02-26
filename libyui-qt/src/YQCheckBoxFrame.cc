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

  File:	      YQCheckBoxFrame.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include <qcheckbox.h>
#include <QDebug>
#include <QVBoxLayout>
#include <qevent.h>
#include "YQUI.h"
#include "YEvent.h"
#include "utf8.h"

using std::max;

#include "YQCheckBoxFrame.h"

#define TOP_MARGIN 6


YQCheckBoxFrame::YQCheckBoxFrame( YWidget * 		parent,
				  const string &	label,
				  bool			checked )
    : QGroupBox( (QWidget *) parent->widgetRep() )
    , YCheckBoxFrame( parent, label, checked)
{
    setWidgetRep ( this );
    QGroupBox::setTitle( fromUTF8( label ) );
    QGroupBox::setCheckable( true );
    setValue( checked );

    connect( this, SIGNAL( toggled     ( bool ) ),
             this, SLOT  ( stateChanged( bool ) ) );
}


void YQCheckBoxFrame::setLabel( const string & newLabel )
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
	yuiDebug() << "Status change of " << this << " : now " << boolalpha << newStatus << endl;

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
	// yuiDebug() << "Child widget added" << endl;

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




#include "YQCheckBoxFrame.moc"
