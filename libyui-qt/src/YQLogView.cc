/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  File:	      YQLogView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlabel.h>
#include <qstyle.h>
#include <QVBoxLayout>
#include <QScrollBar>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YQLogView.h"
#include "YQWidgetCaption.h"


YQLogView::YQLogView( YWidget * 	parent,
		      const string &	label,
		      int 		visibleLines,
		      int 		maxLines )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YLogView( parent, label, visibleLines, maxLines )
{
    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_text = new QTextEdit( this );
    YUI_CHECK_NEW( _qt_text );
    layout->addWidget( _qt_text );

    _qt_text->setReadOnly( true );
    _qt_text->setAcceptRichText( false );
    _qt_text->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    _caption->setBuddy( _qt_text );
}


YQLogView::~YQLogView()
{
    // NOP
}


void
YQLogView::displayLogText( const string & text )
{
    QScrollBar *sb = _qt_text->verticalScrollBar();

    int sbVal = sb->value();
    int sbMaxVal = sb->maximum();
    _qt_text->setPlainText( fromUTF8( text ) );

    if ( sbVal != sbMaxVal )
	sb->setValue( sbVal );
    else
	sb->setValue( sb->maximum() );
}


void
YQLogView::setLabel( const string & label )
{
    _caption->setText( label );
    YLogView::setLabel( label );
}



void
YQLogView::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_text->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQLogView::preferredWidth()
{
    return max( 50, sizeHint().width() );
}


int
YQLogView::preferredHeight()
{
    int hintHeight 	 = visibleLines() * _qt_text->fontMetrics().lineSpacing();
    hintHeight		+= _qt_text->style()->pixelMetric( QStyle::PM_ScrollBarExtent );
    hintHeight		+= _qt_text->frameWidth() * 2;

    if ( !_caption->isHidden() )
	hintHeight	+=  _caption->sizeHint().height();

    return max( 80, hintHeight );
}


void
YQLogView::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQLogView::setKeyboardFocus()
{
    _qt_text->setFocus();

    return true;
}



#include "YQLogView.moc"

