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
#include "YQUI.h"
#include "YQProgressBar.h"

#define MAX_PROGRESS 10000


YQProgressBar::YQProgressBar( QWidget * 		parent,
			      const YWidgetOpt & 	opt,
			      const YCPString & 	label,
			      const YCPInteger & 	maxProgress,
			      const YCPInteger &	progress )
    : QVBox( parent )
    , YProgressBar( opt, label, maxProgress, progress )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8(label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );
    
    if ( label->value() == "" )
	_qt_label->hide();

    _qt_progressbar = new QProgressBar( this );
    _qt_progressbar->setFont( YQUI::ui()->currentFont() );
    _qt_progressbar->setTotalSteps( MAX_PROGRESS );
    _qt_label->setBuddy( _qt_progressbar );

    setProgress( progress );
}


void YQProgressBar::setEnabling( bool enabled )
{
    _qt_label->setEnabled( enabled );
    _qt_progressbar->setEnabled( enabled );
}


long YQProgressBar::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	long minSize = 200;
	long hintWidth = _qt_label->sizeHint().width() + margin();

	if ( _qt_label->text().isEmpty() )
	    hintWidth = 0;

	return max( minSize, hintWidth );
    }
    else
    {
	return sizeHint().height();
    }
}


void YQProgressBar::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}

void YQProgressBar::setLabel( const YCPString & text )
{
    _qt_label->setText( fromUTF8(text->value() ) );
}


void YQProgressBar::setProgress( const YCPInteger & newProgress )
{
    int progress = 0;

    if ( maxProgress->value() != 0 )
    {
	progress = (int) ( MAX_PROGRESS *
			   ( (float) newProgress->value() ) /
			   ( (float) maxProgress->value() ) );
	
    }

    _qt_progressbar->setProgress( progress );
    YProgressBar::setProgress( newProgress );
}


bool YQProgressBar::setKeyboardFocus()
{
    _qt_progressbar->setFocus();

    return true;
}


#include "YQProgressBar.moc"
