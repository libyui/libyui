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
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQUI.h"
#include "YEvent.h"
#include "utf8.h"

using std::max;

#include "YQCheckBoxFrame.h"

#define TOP_MARGIN 6


YQCheckBoxFrame::YQCheckBoxFrame( QWidget * 		parent,
				  const YWidgetOpt & 	opt,
				  const YCPString  &	newLabel,
				  bool			initialValue )
    : QGroupBox( parent )
    , YCheckBoxFrame( opt, newLabel )
{
    QGroupBox::setTitle( fromUTF8( getLabel()->value() ) );
    QGroupBox::setCheckable( true );
    setFont( YQUI::ui()->currentFont() );
    setWidgetRep ( this );

    connect( this, SIGNAL( toggled     ( bool ) ),
	     this, SLOT  ( stateChanged( bool ) ) );
    
    setChecked( initialValue );
}


bool YQCheckBoxFrame::getValue()
{
    return isChecked();
}


void YQCheckBoxFrame::setValue( bool newValue )
{
    setChecked( newValue );
}


void YQCheckBoxFrame::setEnabling( bool enabled )
{
    if ( enabled )
    {
	QFrame::setEnabled( true );
	handleChildrenEnablement( getValue() );
    }
    else
    {
	setChildrenEnabling( false );
	QFrame::setEnabled( true );
    }
}


void
YQCheckBoxFrame::setSize( long newWidth, long newHeight )
{
    resize ( newWidth, newHeight );

    long newChildWidth  = max ( 0L, newWidth  - 2 * frameWidth() - 1 );
    long newChildHeight = max ( 0L, newHeight - frameWidth() - fontMetrics().height() - TOP_MARGIN - 1 );

    if ( numChildren() > 0 )
	YContainerWidget::child(0)->setSize( newChildWidth, newChildHeight );
}


void
YQCheckBoxFrame::setLabel( const YCPString & newLabel )
{
    YCheckBoxFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8( getLabel()->value() ) );
}


long YQCheckBoxFrame::nicesize( YUIDimension dim )
{
    long niceSize;
    long childNiceSize = numChildren() > 0 ? YContainerWidget::child(0)->nicesize( dim ) : 0;

    if ( dim == YD_HORIZ )
    {
	niceSize = max( childNiceSize, (long)(10 + fontMetrics().width(title() ) ) ) + 2 * frameWidth() + 1;
    }
    else
    {
	niceSize = childNiceSize + frameWidth() + fontMetrics().height() + TOP_MARGIN + 1;
    }

    return niceSize;
}


void
YQCheckBoxFrame::childAdded( YWidget * child )
{
    ( (QWidget *) child->widgetRep() )->move( frameWidth(), fontMetrics().height() + TOP_MARGIN );
}


void YQCheckBoxFrame::stateChanged( bool newState )
{
    // y2milestone( "new state: %d", newState );
    setChecked( newState );
    handleChildrenEnablement( newState );
    
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQCheckBoxFrame::setKeyboardFocus()
{
    QGroupBox::setFocus();

    return true;
}




#include "YQCheckBoxFrame.moc"
