
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |*************************************************************************** */

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		QY2Styler.cc

  Author:	Stefan Kulow <coolo@suse.de>

/-*/


#define YUILogComponent "qt-styler"
#include <YUILog.h>
#include <YUIException.h>

#include "QY2Styler.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QSvgRenderer>
#include <iostream>
#include <QPixmapCache>

#define LOGGING_CAUSES_QT4_THREADING_PROBLEMS	1

std::ostream & operator<<( std::ostream & stream, const QString     & str     );
std::ostream & operator<<( std::ostream & stream, const QStringList & strList );
std::ostream & operator<<( std::ostream & stream, const QWidget     * widget  );

using namespace std;


QY2Styler::QY2Styler( QObject * parent )
    : QObject( parent )
{
    QPixmapCache::setCacheLimit( 5 * 1024 );
    yuiDebug() << "Styler created" << endl;
}


QY2Styler *
QY2Styler::styler()
{
    static QY2Styler * styler = 0;

    if ( ! styler )
    {
	yuiDebug() << "Creating QY2Styler singleton" << endl;
	
	styler = new QY2Styler( qApp );
	YUI_CHECK_NEW( styler );
	
	QString style = getenv("Y2STYLE");
    
	if ( ! style.isEmpty() )
	    styler->loadStyleSheet( style );
	else
	    styler->loadStyleSheet( "style.qss" );
    }

    return styler;
}


void QY2Styler::loadStyleSheet( const QString & filename )
{
    QFile file( themeDir() + filename );
    
    if ( file.open( QIODevice::ReadOnly ) )
    {
	yuiMilestone() << "Using style sheet \"" << file.fileName() << "\"" << endl;
	QString text = file.readAll();
	setStyleSheet( text );
    }
    else
    {
        yuiMilestone() << "Couldn't open style sheet \"" << file.fileName() << "\"" << endl;
    }

}

void QY2Styler::setStyleSheet( const QString & text )
{
    _style = text;
    processUrls( _style );
        
    QWidget *child;
    QList< QWidget* > childlist;

    foreach( childlist, _children )
        foreach( child, childlist )
            child->setStyleSheet( _style );
}


void QY2Styler::processUrls( QString & text )
{
    QString result;
    QStringList lines = text.split( '\n' );
    QRegExp urlRegex( ": *url\\((.*)\\)" );
    QRegExp backgroundRegex( "^ */\\* *Background: *([^ ]*) *([^ ]*) *\\*/$" );
    QRegExp richTextRegex( "^ */\\* *Richtext: *([^ ]*) *\\*/$" );

    _backgrounds.clear();

    for ( QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it )
    {
        QString line = *it;

	// Replace file name inside url() with full path (from themeDir() )
	
        if ( urlRegex.indexIn( line ) >= 0 )
	{
	    QString fileName = urlRegex.cap( 1 );
	    QString fullPath = themeDir() + fileName;
	    yuiDebug() << "Expanding " << fileName << "\tto " << fullPath << endl;
            line.replace( urlRegex, ": url(" + fullPath + ")");
	}

        if ( backgroundRegex.exactMatch( line ) )
        {
            QStringList name = backgroundRegex.cap( 1 ).split( '#' );
	    QString fullPath =  themeDir() + backgroundRegex.cap( 2 );
	    yuiDebug() << "Expanding background " << name[0] << "\tto " << fullPath << endl;
	    
            _backgrounds[ name[0] ].filename = fullPath;
            _backgrounds[ name[0] ].full = false;
	    
            if ( name.size() > 1 )
                _backgrounds[ name[0] ].full = ( name[1] == "full" );
        }

        if ( richTextRegex.exactMatch( line ) )
        {
            QString filename = richTextRegex.cap( 1 );
            QFile file( themeDir() + "/" + filename );
	    
            if ( file.open(  QIODevice::ReadOnly ) )
            {
		yuiDebug() << "Reading " << file.fileName();
                _textStyle = file.readAll();
            }
	    else
	    {
		yuiError() << "Can't read " << file.fileName();
	    }
        }

        result += line;
    }
    
    text = result;
}


QString
QY2Styler::themeDir() const
{
    return THEMEDIR "/current/wizard/";
}


void QY2Styler::registerWidget( QWidget * widget )
{
    widget->installEventFilter( this );
    widget->setAutoFillBackground( true );
    widget->setStyleSheet( _style );
}


void QY2Styler::unregisterWidget( QWidget  *widget )
{
    _children.remove( widget );
}


void QY2Styler::registerChildWidget( QWidget * parent, QWidget * widget )
{
    // Don't use yuiDebug() here - deadlock (reason unknown so far) in thread handling!
    
    qDebug() << "Registering " << widget << " for parent " << parent << endl;
    widget->installEventFilter( this );
    _children[parent].push_back( widget );
}


QImage
QY2Styler::getScaled( const QString name, const QSize & size )
{
    QImage image = _backgrounds[name].pix;

    if ( size != image.size() )
        image = image.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    else
        image = image.convertToFormat( QImage::Format_ARGB32 );

    if ( image.isNull() )
	yuiError() << "Can't load pixmap from " <<  name << endl;
#if 1
    else
	yuiMilestone() << "Loaded pixmap from \"" << name
		       << "\"  size: " << image.size().width() << "x" << image.size().height()
		       << endl;
#endif

    return image;
}


void QY2Styler::renderParent( QWidget * wid )
{
    // yuiDebug() << "Rendering " << wid << endl;
    QString name = wid->objectName();
    
    // TODO
    wid->setPalette( QApplication::palette() );

    // if the parent does not have a background, this does not make sense
    if ( _backgrounds[name].pix.isNull() )
        return;

    QRect fillRect = wid->contentsRect();
    if ( _backgrounds[name].full )
        fillRect = wid->rect();

    QImage back;
    
    if ( _backgrounds[name].lastscale != fillRect.size() )
    {
        _backgrounds[name].scaled = getScaled( name, fillRect.size() );
        _backgrounds[name].lastscale = fillRect.size();
    }
    
    back = _backgrounds[name].scaled;

    QPainter pain( &back );
    QWidget *child;
   

    foreach( child, _children[wid] )
    {
        // yuiDebug() << "foreach " << child << " " << wid << endl;
        QString name = child->objectName();

        if (! child->isVisible() || _backgrounds[name].pix.isNull() )
             continue;

        QRect fillRect = child->contentsRect();
        if ( _backgrounds[name].full )
            fillRect = child->rect();

        QString key = QString( "style_%1_%2_%3" ).arg( name ).arg( fillRect.width() ).arg( fillRect.height() );
        QPixmap scaled;
	
        if ( QPixmapCache::find( key, scaled ) )
        {
            // yuiDebug() << "found " << key << endl;
        }
	else
	{
            scaled = QPixmap::fromImage( getScaled( name, fillRect.size() ) );
            QPixmapCache::insert( key, scaled );
        }
        pain.drawPixmap( wid->mapFromGlobal( child->mapToGlobal( fillRect.topLeft() ) ), scaled );
    }
    
    QPixmap result = QPixmap::fromImage( back );

    QPalette p = wid->palette();
    p.setBrush(QPalette::Window, result );
    wid->setPalette( p );
}


bool
QY2Styler::updateRendering( QWidget *wid )
{
    if (!wid)
       return false;

    QString name = wid->objectName();

    if (! wid->isVisible() || !wid->updatesEnabled() )
        return false;

    if ( _backgrounds[name].pix.isNull() )
    {
        QString back = _backgrounds[ name ].filename;

	QImage image( back );
        _backgrounds[ name ].pix = image;

	if ( ! back.isEmpty() )	// Prevent misleading error messages
	{
	    if ( image.isNull() )
	    {
		yuiError() << "Couldn't load background image \"" << back
			   << "\" for \"" << name << "\""
			   << endl;
	    }
	    else
	    {
		yuiDebug() << "Loading background image \"" << back
			   << "\" for " << name << "\""
			   << endl;
	    }
	}
    }

    
    // if it's a child itself, we have to do the full blow action
    
    if ( !_children.contains( wid ) )
    {
        QWidget *parent = wid->parentWidget();
        while ( parent && !_children.contains( parent ) )
            parent = parent->parentWidget();
        if (!parent)
           return false;
        renderParent( parent );
    }
    else
    {
        renderParent( wid );
    }
	
    return true;
}

    
bool
QY2Styler::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize || 
	 ev->type() == QEvent::Show   ||
	 ev->type() == QEvent::LayoutRequest ||
	 ev->type() == QEvent::UpdateRequest )
        updateRendering( qobject_cast<QWidget*>( obj ) );

    return QObject::eventFilter( obj, ev );
}




std::ostream & operator<<( std::ostream & stream, const QString & str )
{
    return stream << qPrintable( str );
}


std::ostream & operator<<( std::ostream & stream, const QStringList & strList )
{
    stream << "[ ";
    
    for ( QStringList::const_iterator it = strList.begin();
	  it != strList.end();
	  ++it )
    {
	stream << qPrintable( *it ) << " ";
    }
    
    stream << " ]";
    
    return stream;
}


std::ostream & operator<<( std::ostream & stream, const QWidget * widget )
{
#if LOGGING_CAUSES_QT4_THREADING_PROBLEMS
 
    // QObject::metaObject()->className() and QObject::objectName() can cause
    // YQUI to hang, probably due to threading problems.
    
    stream << "QWidget at " << hex << (void *) widget << dec;
#else
    if ( widget )
    {
	if ( widget->metaObject() )
	    stream << widget->metaObject()->className();
	else
	    stream << "<QWidget>";

	if ( ! widget->objectName().isEmpty() )
	    stream << " \"" << qPrintable( widget->objectName() ) << "\"";

	stream << " at " << hex << widget << dec;
    }
    else // ! widget
    {
	stream << "<NULL QWidget>";
    }
#endif

    
    return stream;
}


#include "QY2Styler.moc"
