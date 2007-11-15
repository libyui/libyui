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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qprogressbar.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YQProgressBar.h"
#include "YQWidgetCaption.h"


YQProgressBar::YQProgressBar( YWidget * 	parent,
			      const string &	label,
			      int		maxValue )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YProgressBar( parent, label, maxValue )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    
    _qt_progressbar = new QProgressBar( maxValue, this );
    YUI_CHECK_NEW( _qt_progressbar );
    
    _caption->setBuddy( _qt_progressbar );
}


YQProgressBar::~YQProgressBar()
{
    // NOP
}


void YQProgressBar::setLabel( const string & label )
{
    _caption->setText( label );
    YProgressBar::setLabel( label );
}


void YQProgressBar::setValue( int newValue )
{
    YProgressBar::setValue( newValue );
    _qt_progressbar->setProgress( value() );
}



void YQProgressBar::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_progressbar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQProgressBar::preferredWidth()
{
    int hintWidth = _caption->isShown() ?
	_caption->sizeHint().width() + margin() : 0;

    return max( 200, hintWidth );
}


int YQProgressBar::preferredHeight()
{
    return sizeHint().height();
}


void YQProgressBar::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQProgressBar::setKeyboardFocus()
{
    _qt_progressbar->setFocus();

    return true;
}


#include "YQProgressBar.moc"
