#include "QY2Styler.h"
#include <QFile>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QSvgRenderer>

QY2Styler *QY2Styler::_self = 0;

QY2Styler::QY2Styler( QObject *parent )
    : QObject( parent )
{
    _self = this;
}

void QY2Styler::setStyleSheet( const QString &filename )
{
    QFile file( themeDir() + filename );
    if ( file.open( QIODevice::ReadOnly ) )
    {
        QString content = file.readAll();
        processUrls( content );
        qApp->setStyleSheet( content );
    }
}

void QY2Styler::processUrls(QString &text)
{
    QString result;
    QStringList lines = text.split( '\n' );
    QRegExp urlx( ": *url\\((.*)\\)" );
    QRegExp backgroundx( "^ */\\* *Background: *([^ ]*) *([^ ]*) *\\*/$" );
    for ( QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it )
    {
        QString line = *it;
        if ( urlx.indexIn( line ) >= 0 )
            line.replace( urlx, ": url(" + themeDir() + urlx.cap( 1 ) + ")");

        if ( backgroundx.exactMatch( line ) )
            _backgroundFn[backgroundx.cap( 1 )] = themeDir() + backgroundx.cap( 2 );

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
}

bool QY2Styler::eventFilter( QObject * obj, QEvent * ev )
{
    QString name = obj->objectName();

    if ( ev->type() != QEvent::Resize )
        return QObject::eventFilter( obj, ev );

    if ( !_backgroundFn.contains( name ) )
        return QObject::eventFilter( obj, ev );

    qDebug( "eventFilter %s %s %d", qPrintable( name ), obj->metaObject()->className(), ev->type() );

    QWidget *wid = qobject_cast<QWidget*>( obj );
    if ( !_backgroundPx.contains( name ) )
    {
        QString back = _backgroundFn[ name ];
        _backgroundPx[ name ] = QImage( back );
        qDebug( "loading %s for %s", qPrintable( back ), qPrintable( name ) );
    }

    wid->setAutoFillBackground( true );

    QPixmap result( wid->size() );
    if ( wid->contentsRect() != wid->rect() )
        result.fill( QColor( 0, 128, 0, 0 ) );

    QPainter pain( &result );
    if ( !_backgroundFn[ name ].endsWith( ".svg" ) )
    {
        QImage scaled = _backgroundPx[name].scaled( wid->contentsRect().width(), wid->contentsRect().height() );
        pain.drawImage( wid->contentsRect().topLeft(), scaled, QRectF(QPointF(0,0), scaled.size()), Qt::OrderedAlphaDither);
    } else {
#if 0
        QSvgRenderer rend( _backgroundFn[ name ] );
        rend.render( &pain, wid->contentsRect() );
#endif
    }
    QPalette p = wid->palette();
    p.setBrush(QPalette::Window, result );
    wid->setPalette( p );

    return QObject::eventFilter( obj, ev );
}

#include "QY2Styler.moc"
