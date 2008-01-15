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

using namespace std;

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
        {
            QStringList name = backgroundx.cap( 1 ).split( '#' );
            qDebug() << name;
            _backgrounds[ name[0] ].filename = themeDir() + backgroundx.cap( 2 );
            _backgrounds[ name[0] ].full = false;
            if ( name.size() > 1 )
                _backgrounds[ name[0] ].full = ( name[1] == "full" );
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
}

bool QY2Styler::eventFilter( QObject * obj, QEvent * ev )
{
    QString name = obj->objectName();

    if ( ev->type() != QEvent::Resize && ev->type() != QEvent::Show )
        return QObject::eventFilter( obj, ev );

    if ( !_backgrounds.contains( name ) )
       return QObject::eventFilter( obj, ev );

    QWidget *wid = qobject_cast<QWidget*>( obj );

    if (! wid->isVisible() )
        return QObject::eventFilter( obj, ev );

    //qDebug() << "eventFilter " << qPrintable( name ) << " " << obj->metaObject()->className() << " " << wid->isVisible();

    if ( _backgrounds[name].pix.isNull() )
    {
        QString back = _backgrounds[ name ].filename;
        _backgrounds[ name ].pix = QImage( back );
        //qDebug() << "loading " << qPrintable( back ) << " for " << qPrintable( name );
    }

    wid->setAutoFillBackground( true );

    QPixmap result( wid->size() );
    QRect fillRect = wid->contentsRect();
    if ( _backgrounds[name].full )
        fillRect = wid->rect();

    if ( fillRect  != wid->rect() )
        result.fill( QColor( 0, 128, 0, 0 ) );

    QPainter pain( &result );
    if ( !_backgrounds[ name ].filename.endsWith( ".svg" ) )
    {
        qDebug() << "scale " << qPrintable( name ) << " " << fillRect.width() << " " << fillRect.height();
        QImage scaled = _backgrounds[name].pix.scaled( fillRect.width(), fillRect.height() );
        pain.drawImage( fillRect.topLeft(), scaled, QRectF(QPointF(0,0), scaled.size()), Qt::OrderedAlphaDither);
    } else {
#if 0
        QSvgRenderer rend( _backgroundFn[ name ] );
        rend.render( &pain, fillRect );
#endif
    }
    QPalette p = wid->palette();
    p.setBrush(QPalette::Window, result );
    wid->setPalette( p );

    return QObject::eventFilter( obj, ev );
}

#include "QY2Styler.moc"
