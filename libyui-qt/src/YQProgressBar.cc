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


YQProgressBar::YQProgressBar( QWidget * parent, YWidgetOpt & opt,
			     const YCPString & label,
			     const YCPInteger & maxProgress, const YCPInteger & progress)
    : QVBox(parent)
    , YProgressBar(opt, label, maxProgress, progress)
{
    setWidgetRep( this );

    setSpacing( SPACING );
    setMargin( MARGIN );

    _qt_label = new QLabel(fromUTF8(label->value() ), this);
    _qt_label->setTextFormat(QLabel::PlainText);
    _qt_label->setFont(YUIQt::ui()->currentFont() );
    
    if ( label->value() == "" )
	_qt_label->hide();

    _qt_progressbar = new QProgressBar(this);
    _qt_progressbar->setFont(YUIQt::ui()->currentFont() );
    _qt_progressbar->setTotalSteps(maxProgress->value() );
    _qt_label->setBuddy(_qt_progressbar);

    setProgress(progress);
}


void YQProgressBar::setEnabling( bool enabled)
{
    _qt_label->setEnabled(enabled);
    _qt_progressbar->setEnabled(enabled);
}


long YQProgressBar::nicesize( YUIDimension dim)
{
    if (dim == YD_HORIZ)
    {
	long minSize = 200;
	long hintWidth = _qt_label->sizeHint().width() + margin();

	if ( ! _qt_label->isVisible() )
	    hintWidth = 0;

	return max( minSize, hintWidth );
    }
    else
    {
	return sizeHint().height();
    }
}


void YQProgressBar::setSize( long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}

void YQProgressBar::setLabel( const YCPString & text)
{
    _qt_label->setText(fromUTF8(text->value() ) );
}


void YQProgressBar::setProgress( const YCPInteger & progress)
{
    if ( progress->value() < _qt_progressbar->progress() )
    {
	/*
	 * Qt bug workaround: Decreased progress bar values are not
	 * honored correctly. So we need to reset the progress bar prior
	 * to setting the new value.
	 */

	_qt_progressbar->reset();
    }

    _qt_progressbar->setProgress(progress->value() );
}


bool YQProgressBar::setKeyboardFocus()
{
    _qt_progressbar->setFocus();

    return true;
}


#include "YQProgressBar.moc.cc"
