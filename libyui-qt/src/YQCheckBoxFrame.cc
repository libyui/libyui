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
    preventQGroupBoxAutoEnablement();
			   
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
	QFrame::setEnabled( true );
	handleChildrenEnablement( value() );
    }
    else
    {
	QFrame::setEnabled( true );
	YWidget::setChildrenEnabled( false );
    }
    
    YWidget::setEnabled( enabled );
}


void YQCheckBoxFrame::preventQGroupBoxAutoEnablement()
{
    /*
     * This is a nasty hack. But it is necessary because QGroupBox handles its
     * internal check box too nearsightedly: It forces all children to be
     * enabled or disabled depending on the status of the check box. The
     * behaviour cannot be inverted or suppressed.
     *
     * In some cases, however, it makes sense to let the application decide to
     * handle that differently. Since the YaST2 UI is a toolkit, we leave this
     * decision up to the application rather than forcing any specific behaviour.
     */

    // Find the check box in the child hierarchy (as a direct child)

    _checkBox = dynamic_cast<QCheckBox *>( QObject::child( 0,		// objName
							   "QCheckBox",	// inheritsClass
							   false ) );	// recursive

    if ( ! _checkBox )
    {
	y2warning( "Can't find QCheckBox child" );

	connect( this, SIGNAL( toggled     ( bool ) ),
		 this, SLOT  ( stateChanged( bool ) ) );

	return;
    }

    // Disconnect all signals to this object.
    //
    // In particular, disconnect the connection from the check box's
    // 'toggled()' signal to this object's parent class's (private)
    // setChildrenEnabled() method.

    disconnect( _checkBox,	// sender
		0,		// signal
		this,		// receiver
		0 );		// slot (private method in parent class)

    // Connect the check box directly to this class.

    connect( _checkBox, SIGNAL( toggled     ( bool ) ),
	     this,	SLOT  ( stateChanged( bool ) ) );
}


void YQCheckBoxFrame::stateChanged( bool newState )
{
    y2debug( "new state: %d", newState );
    handleChildrenEnablement( newState );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
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
	int newChildWidth  = max ( 0, newWidth  - 2 * frameWidth() - 1 );
	int newChildHeight = max ( 0, newHeight - frameWidth() - fontMetrics().height() - TOP_MARGIN - 1 );

	firstChild()->setSize( newChildWidth, newChildHeight );
	
	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( frameWidth(), fontMetrics().height() + TOP_MARGIN );
    }
}


int YQCheckBoxFrame::preferredWidth()
{
    int preferredWidth;
    int childPreferredWidth = hasChildren() ? firstChild()->preferredWidth() : 0;

    preferredWidth = max( childPreferredWidth,
			  (10 + fontMetrics().width( title() ) ) );
    preferredWidth += 2*frameWidth() + 1;

    return preferredWidth;
}


int YQCheckBoxFrame::preferredHeight()
{
    int preferredHeight = hasChildren() ? firstChild()->preferredHeight() : 0;
    preferredHeight += frameWidth() + fontMetrics().height() + TOP_MARGIN + 1;
    
    return preferredHeight;
}


bool YQCheckBoxFrame::setKeyboardFocus()
{
    QGroupBox::setFocus();

    return true;
}




#include "YQCheckBoxFrame.moc"
