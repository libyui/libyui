#define YUILogComponent "qt-wizard"
#include "YUILog.h"

#include "QY2Styler.h"
#include <QFile>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>
#include <iostream>
#include <QPixmapCache>

using namespace std;

QY2Styler *QY2Styler::_self = 0;

QY2Styler::QY2Styler( QObject *parent )
    : QObject( parent )
{
    QPixmapCache::setCacheLimit( 5 * 1024 );
    _self = this;
}

void QY2Styler::setStyleSheet( const QString &filename )
{
    QFile file( themeDir() + filename );
    if ( file.open( QIODevice::ReadOnly ) )
    {
        _style = file.readAll();
        processUrls( _style );
        yuiMilestone() << "set stylesheet " << qPrintable(filename) << endl;
    }
    else
        yuiMilestone() << "could not open " << qPrintable(filename) << endl;
}

void QY2Styler::processUrls(QString &text)
{
    QString result;
    QStringList lines = text.split( '\n' );
    QRegExp urlx( ": *url\\((.*)\\)" );
    QRegExp backgroundx( "^ */\\* *Background: *([^ ]*) *([^ ]*) *\\*/$" );
    QRegExp richtextx( "^ */\\* *Richtext: *([^ ]*) *\\*/$" );
    for ( QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it )
    {
        QString line = *it;
        if ( urlx.indexIn( line ) >= 0 )
            line.replace( urlx, ": url(" + themeDir() + urlx.cap( 1 ) + ")");

        if ( backgroundx.exactMatch( line ) )
        {
            QStringList name = backgroundx.cap( 1 ).split( '#' );
            qDebug() << name;
            _backgrounds[ name[0] ].filename = themeDir() + backgroundx.cap( 2 );
            _backgrounds[ name[0] ].full = false;
            if ( name.size() > 1 )
                _backgrounds[ name[0] ].full = ( name[1] == "full" );
        }

        if ( richtextx.exactMatch( line ) )
        {
            QString filename = richtextx.cap( 1 );
            QFile file( themeDir() + "/" + filename );
            if ( file.open(  QIODevice::ReadOnly ) )
            {
                _textStyle = file.readAll();
            }
        }

        result += line;
    }
    text = result;
}

QString QY2Styler::themeDir() const
{
    return THEMEDIR "/current/wizard/";
}

void QY2Styler::registerWidget( QWidget *widget )
{
    widget->installEventFilter( this );
    widget->setAutoFillBackground( true );
    widget->setStyleSheet( _style );
}

void QY2Styler::unregisterWidget( QWidget *widget )
{
    _children.remove( widget );
}

void QY2Styler::registerChildWidget( QWidget *parent, QWidget *widget )
{
    qDebug() << "registerChildWidget " << parent << " " << widget;
    widget->installEventFilter( this );
    _children[parent].push_back( widget );
}

QImage QY2Styler::getScaled( const QString name, const QSize & size )
{
    qDebug() << "scale " << qPrintable( name ) << " " << size;
    return _backgrounds[name].pix.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

void QY2Styler::renderParent( QWidget *wid )
{
    qDebug() << "renderParent " << wid << endl;
    QString name = wid->objectName();

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
        qDebug() << "foreach " << child << " " << wid;
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
            qDebug() << "found " << qPrintable( key );
        } else {
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

bool QY2Styler::updateRendering( QWidget *wid )
{
    if (!wid)
       return false;

    QString name = wid->objectName();

    if ( !_backgrounds.contains( name ) )
        return false;

    if (! wid->isVisible() || !wid->updatesEnabled() )
        return false;

    if ( _backgrounds[name].pix.isNull() )
    {
        QString back = _backgrounds[ name ].filename;
        _backgrounds[ name ].pix = QImage( back );
        qDebug() << "loading " << qPrintable( back ) << " for " << qPrintable( name );
    }

    // if it's a children itself, we have to do the full blow action
    if ( !_children.contains( wid ) )
    {
        QWidget *parent = wid->parentWidget();
        while ( parent && !_children.contains( parent ) )
            parent = parent->parentWidget();
        if (!parent)
           return false;
        renderParent( parent );
    } else {
        renderParent( wid );
    }
    return true;

#if 0
    QPixmap result( wid->size() );
    QRect fillRect = wid->contentsRect();
    if ( _backgrounds[name].full )
        fillRect = wid->rect();

    if ( fillRect  != wid->rect() )
        result.fill( QColor( 0, 128, 0, 0 ) );

    QPainter pain( &result );
    if ( !_backgrounds[ name ].filename.endsWith( ".svg" ) )
    {
        QString key = QString( "style_%1_%2_%3" ).arg( name ).arg( fillRect.width() ).arg( fillRect.height() );
        QPixmap scaled;
        if ( QPixmapCache::find( key, scaled ) )
        {
            qDebug() << "found " << qPrintable( key );
        } else {
            qDebug() << "scale " << qPrintable( name ) << " " << fillRect.width() << " " << fillRect.height();
            scaled = QPixmap::fromImage( _backgrounds[name].pix.scaled( fillRect.width(), fillRect.height() ) );
            QPixmapCache::insert( key, scaled );
        }
        pain.drawPixmap( fillRect.topLeft(), scaled );
    } else {
#if 0
        QSvgRenderer rend( _backgroundFn[ name ] );
        rend.render( &pain, fillRect );
#endif
    }
    QPalette p = wid->palette();
    p.setBrush(QPalette::Window, result );
    wid->setPalette( p );

    return true;
#endif
}

bool QY2Styler::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize || ev->type() == QEvent::Show )
        updateRendering( qobject_cast<QWidget*>( obj ) );

    return QObject::eventFilter( obj, ev );
}

#include "QY2Styler.moc"
