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
#include "utf8.h"

using std::max;

#include "YQFrame.h"


YQFrame::YQFrame( YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt,
		  const YCPString &newLabel )
    : QGroupBox( parent )
    , YFrame( opt, newLabel )
{
    QGroupBox::setTitle ( fromUTF8 ( newLabel->value() ) );
    setWidgetRep ( (QWidget *) this );
}

void YQFrame::setEnabling(bool enabled)
{
    QFrame::setEnabled(enabled);
}


void
YQFrame::setSize ( long newwidth, long newheight )
{
    resize ( newwidth, newheight );

    long newChildWidth  = max ( 0L, newwidth  - 2 * frameWidth() );
    long newChildHeight = max ( 0L, newheight - frameWidth() - fontMetrics().height() );

    YContainerWidget::child(0)->setSize (newChildWidth, newChildHeight);

}


void
YQFrame::setLabel ( const YCPString & newLabel )
{
    QGroupBox::setTitle ( fromUTF8 ( newLabel->value() ) );
    YFrame::setLabel ( newLabel );
}


long YQFrame::nicesize ( YUIDimension dim )
{
    long niceSize;
    long childNiceSize = YContainerWidget::child(0)->nicesize ( dim );

    if ( dim == YD_HORIZ )
    {
	niceSize = max(childNiceSize, (long)(10 + fontMetrics().width(title()))) + 2 * frameWidth();
    }
    else
    {
	niceSize = childNiceSize + frameWidth() + fontMetrics().height();
    }

    return niceSize;
}


void
YQFrame::childAdded ( YWidget *child )
{
    ( (QWidget *) child->widgetRep() )->move ( frameWidth(), fontMetrics().height() );
}



#include "YQFrame.moc.cc"
