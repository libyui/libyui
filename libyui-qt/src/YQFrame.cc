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
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YUIQt.h"
#include "utf8.h"

using std::max;

#include "YQFrame.h"


YQFrame::YQFrame( QWidget * parent, YWidgetOpt & opt,
		  const YCPString & newLabel )
    : QGroupBox( parent )
    , YFrame( opt, newLabel )
{
    QGroupBox::setTitle( fromUTF8 ( getLabel()->value() ) );
    setFont( YUIQt::ui()->currentFont() );
    setWidgetRep ( this );
}

void YQFrame::setEnabling(bool enabled)
{
    QFrame::setEnabled(enabled);
}


void
YQFrame::setSize( long newWidth, long newHeight )
{
    resize ( newWidth, newHeight );

    long newChildWidth  = max ( 0L, newWidth  - 2 * frameWidth() - 1 );
    long newChildHeight = max ( 0L, newHeight - frameWidth() - fontMetrics().height() - 1 );

    if ( numChildren() > 0 )
	YContainerWidget::child(0)->setSize (newChildWidth, newChildHeight);

}


void
YQFrame::setLabel( const YCPString & newLabel )
{
    YFrame::setLabel( newLabel );
    QGroupBox::setTitle( fromUTF8 ( getLabel()->value() ) );
}


long YQFrame::nicesize( YUIDimension dim )
{
    long niceSize;
    long childNiceSize = numChildren() > 0 ? YContainerWidget::child(0)->nicesize ( dim ) : 0;

    if ( dim == YD_HORIZ )
    {
	niceSize = max(childNiceSize, (long)(10 + fontMetrics().width(title() ) ) ) + 2 * frameWidth() + 1;
    }
    else
    {
	niceSize = childNiceSize + frameWidth() + fontMetrics().height() + 1;
    }

    return niceSize;
}


void
YQFrame::childAdded( YWidget * child )
{
    ( (QWidget *) child->widgetRep() )->move ( frameWidth(), fontMetrics().height() );
}



#include "YQFrame.moc.cc"
