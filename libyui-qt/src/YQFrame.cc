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

  File:	      YQFrame.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQUI.h"
#include "utf8.h"

using std::max;

#include "YQFrame.h"


YQFrame::YQFrame( YWidget * 		parent,
		  const string &	initialLabel )
    : QGroupBox( (QWidget *) parent->widgetRep() )
    , YFrame( parent, initialLabel )
{
    setWidgetRep ( this );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


YQFrame::~YQFrame()
{
    // NOP
}


void YQFrame::setEnabled( bool enabled )
{
    QGroupBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void
YQFrame::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );

    if ( hasChildren() )
    {
	int newChildWidth  = max ( 0, newWidth  - 2 * frameWidth() - 1 );
	int newChildHeight = max ( 0, newHeight - frameWidth() - fontMetrics().height() - 1 );

	firstChild()->setSize( newChildWidth, newChildHeight );
	
	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( frameWidth(), fontMetrics().height() );
    }
}


void
YQFrame::setLabel( const string & newLabel )
{
    YFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8( label() ) );
}


int YQFrame::preferredWidth()
{
    int preferredWidth;
    int childPreferredWidth = hasChildren() ? firstChild()->preferredWidth() : 0;

    preferredWidth = max( childPreferredWidth,
			  (10 + fontMetrics().width( title() ) ) );
    preferredWidth += 2*frameWidth() + 1;

    return preferredWidth;
}


int YQFrame::preferredHeight()
{
    int preferredHeight = hasChildren() ? firstChild()->preferredHeight() : 0;
    preferredHeight += frameWidth() + fontMetrics().height() + 1;
    
    return preferredHeight;
}


#include "YQFrame.moc"
