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

  File:	      YQProgressBar.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qprogressbar.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YUIQt.h"
#include "YQProgressBar.h"


#define SPACING			4	// between subwidgets
#define MARGIN			4	// around the widget


YQProgressBar::YQProgressBar( QWidget *parent, YWidgetOpt & opt,
			     const YCPString & label,
			     const YCPInteger & maxProgress, const YCPInteger & progress)
    : QVBox(parent)
    , YProgressBar(opt, label, maxProgress, progress)
{
    setWidgetRep( this );

    setSpacing( SPACING );
    setMargin( MARGIN );

    qt_label = new QLabel(fromUTF8(label->value()), this);
    qt_label->setTextFormat(QLabel::PlainText);
    qt_label->setFont(YUIQt::ui()->currentFont());
    
    if ( label->value() == "" )
	qt_label->hide();

    qt_progressbar = new QProgressBar(this);
    qt_progressbar->setFont(YUIQt::ui()->currentFont());
    qt_progressbar->setTotalSteps(maxProgress->value());
    qt_label->setBuddy(qt_progressbar);

    setProgress(progress);
}


void YQProgressBar::setEnabling(bool enabled)
{
    qt_label->setEnabled(enabled);
    qt_progressbar->setEnabled(enabled);
}


long YQProgressBar::nicesize(YUIDimension dim)
{
    if (dim == YD_HORIZ)
    {
	long minSize = 200;
	long hintWidth = qt_label->sizeHint().width() + margin();

	if ( ! qt_label->isVisible() )
	    hintWidth = 0;

	return max( minSize, hintWidth );
    }
    else
    {
	return sizeHint().height();
    }
}


void YQProgressBar::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}

void YQProgressBar::setLabel(const YCPString & text)
{
    qt_label->setText(fromUTF8(text->value()));
}


void YQProgressBar::setProgress(const YCPInteger & progress)
{
    if ( progress->value() < qt_progressbar->progress() )
    {
	/*
	 * Qt bug workaround: Decreased progress bar values are not
	 * honored correctly. So we need to reset the progress bar prior
	 * to setting the new value.
	 */

	qt_progressbar->reset();
    }

    qt_progressbar->setProgress(progress->value());
}


bool YQProgressBar::setKeyboardFocus()
{
    qt_progressbar->setFocus();

    return true;
}


#include "YQProgressBar.moc.cc"
