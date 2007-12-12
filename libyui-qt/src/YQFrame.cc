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
//Added by qt3to4:

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
	int newChildWidth  = max ( 0, newWidth  - 2 * 11 - 1 );
	int newChildHeight = max ( 0, newHeight - 11 - fontMetrics().height() - 1 );

	firstChild()->setSize( newChildWidth, newChildHeight );

	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( 11, fontMetrics().height() );
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
    preferredWidth += 2*11 + 1;

   return preferredWidth;

}


int YQFrame::preferredHeight()
{
    int preferredHeight = hasChildren() ? firstChild()->preferredHeight() : 0;
    preferredHeight += 11 + fontMetrics().height() + 1;

    return preferredHeight;
}


#include "YQFrame.moc"
