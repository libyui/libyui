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
#include <qprogressbar.h>
#include <qtimer.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQDownloadProgress.h"


YQDownloadProgress::YQDownloadProgress( 					QWidget *	parent,
					YWidgetOpt &	opt,
					const YCPString & label,
					const YCPString & filename,
					int 		expectedSize)
    : QVBox( parent )
    , YDownloadProgress( opt, label, filename, expectedSize )
{
    setWidgetRep( this );
    setMargin( YQWIDGET_BORDER );

    qt_label = new QLabel( fromUTF8(label->value() ), this );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont( YUIQt::ui()->currentFont() );

    qt_progress = new QProgressBar( this );
    qt_progress->setFont( YUIQt::ui()->currentFont() );
    qt_progress->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed) );

    qt_progress->setTotalSteps( expectedSize );
    qt_progress->setProgress( currentFileSize() );

    timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( pollFileSize() ) );

    timer->start( 250,		// millisec
		  false );	// single shot?
}


void YQDownloadProgress::setEnabling(bool enabled)
{
    qt_progress->setEnabled( enabled );
}


long YQDownloadProgress::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ) return sizeHint().width();
    else			return sizeHint().height();
}


void YQDownloadProgress::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}


void YQDownloadProgress::setLabel(const YCPString & label)
{
    qt_label->setText( fromUTF8( label->value() ) );
    YDownloadProgress::setLabel( label );
}


void YQDownloadProgress::setExpectedSize( int expectedSize )
{
    qt_progress->setTotalSteps( expectedSize );
    qt_progress->setProgress( currentFileSize() );
    YDownloadProgress::setExpectedSize( expectedSize );
}


void YQDownloadProgress::pollFileSize()
{
    long size = currentFileSize();

    if ( size > (long) expectedSize() )
	size = (long) expectedSize();
    qt_progress->setProgress( size );
}



#include "YQDownloadProgress.moc.cc"
