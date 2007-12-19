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


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
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

    connect( this, SIGNAL( toggled( bool ) ),
             SLOT( stateChanged( bool ) ) );
    setValue( checked );
}


void YQCheckBoxFrame::setLabel( const string & newLabel )
{
    YCheckBoxFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


bool YQCheckBoxFrame::value()
{
    return _checkBox ? _checkBox->isChecked() : QGroupBox::isChecked();
}


void YQCheckBoxFrame::setValue( bool newValue )
{
    if ( _checkBox )
	_checkBox->setChecked( newValue );
    else
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
    handleChildrenEnablement( newState );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}

bool YQCheckBoxFrame::event(QEvent *e)
{
    /* now on to something very fishy. The purpose of this widget
     * is for whatever reason to provide a checkbox with a groupbox
     * without the children having any connection to it.
     *
     * So we use this trick to undo everything the base class did
     */
    QHash<QWidget*, bool> widgetState;

    QObjectList childList = children();
    for (int i = 0; i < childList.size(); ++i)
    {
        QObject *o = childList.at(i);
        if (o->isWidgetType())
        {
            QWidget *w = static_cast<QWidget *>(o);
            widgetState[w] = w->isEnabled();
        }
    }

    bool ret = QGroupBox::event( e );

    childList = children();
    for (int i = 0; i < childList.size(); ++i)
    {
        QObject *o = childList.at(i);
        if (o->isWidgetType())
        {
            QWidget *w = static_cast<QWidget *>(o);
            if ( widgetState.contains( w ) )
                w->setEnabled( widgetState[w] );
        }
    }

    return ret;
}

void YQCheckBoxFrame::childEvent( QChildEvent * )
{
    // Reimplemented to prevent the parent class disabling child widgets
    // according to its default policy.

    // y2debug( "ChildEvent" );
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
