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

  File:		YQTimezoneSelector.cc

  Author:	Stephan Kulow <coolo@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <math.h>

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTimezoneSelector.h"
#include "YQWidgetCaption.h"
#include "YEvent.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QToolTip>

static float
convert_pos (const QString &pos, int digits)
{
    if (pos.length() < 4 || digits > 9) return 0.0;

    QString whole = pos.left( digits + 1 );
    QString fraction = pos.mid( digits + 1 );

    float t1 = whole.toFloat();
    float t2 = fraction.toFloat();

    if (t1 >= 0.0)
        return t1 + t2/pow (10.0, fraction.length() );
    else
        return t1 - t2/pow (10.0, fraction.length());
}

bool YQTimezoneSelector::Location::operator<(const Location& l1 ) const
{
    return l1.latitude < latitude;
}

YQTimezoneSelector::YQTimezoneSelector( YWidget * parent, const string & pixmap,  const map<string,string> & timezones )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTimezoneSelector( parent, pixmap, timezones )
{
    setWidgetRep( this );
    setMouseTracking(true);
    _pix.load( QString::fromStdString( pixmap ) );

    setStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT,  true );

    char buf[4096];
    FILE *tzfile = fopen ("/usr/share/zoneinfo/zone.tab", "r");
    while (fgets (buf, sizeof(buf), tzfile))
    {
        if (*buf == '#') continue;

        QString sbuf = buf;
        QStringList arr = sbuf.trimmed().split( '\t' );

        int split_index = 1;
        while ( split_index < arr[1].length() && arr[1][split_index] != '-' && arr[1][split_index] != '+' )
            split_index++;

        Location loc;
        loc.country = arr[0];
        loc.zone = arr[2];
        map<string, string>::const_iterator tooltip = timezones.find( loc.zone.toStdString() );
        if (tooltip  == timezones.end() )
            continue;

        loc.tip = QString::fromStdString( tooltip->second );
        if ( arr.size() > 3 )
            loc.comment = arr[3];
        loc.latitude  = convert_pos ( arr[1].left( split_index ), 2);
        loc.longitude = convert_pos ( arr[1].mid( split_index ), 3);

        loc.pix_pos = pixPosition( loc );

        locations.push_back( loc );
    }

    fclose (tzfile);

    qSort( locations.begin(), locations.end() );

    QPainter p( &_pix );
    for ( QList<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it )
        p.drawRect( ( *it ).pix_pos.x() - 1, ( *it ).pix_pos.y() - 1, 3, 3 );

    blink = new QTimer( this );
    blink->setInterval( 200 );
    connect( blink, SIGNAL( timeout() ), SLOT( slotBlink() ) );

    highlight = false;
}

YQTimezoneSelector::~YQTimezoneSelector()
{
    // NOP
}


int YQTimezoneSelector::preferredWidth()
{
    return _pix.width() / 2;
}


int YQTimezoneSelector::preferredHeight()
{
    return _pix.height() / 2;
}


void YQTimezoneSelector::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}

QPoint YQTimezoneSelector::pixPosition( const Location &pos ) const
{
    return QPoint(  _pix.width() / 2 + _pix.width() / 2 * pos.longitude / 180,
                    _pix.height() / 2 - _pix.height() / 2 * pos.latitude / 90 );
}

void YQTimezoneSelector::mousePressEvent ( QMouseEvent * event )
{
    if ( event->button() == Qt::RightButton )
    {
        _zoom = QPoint();
        _best = Location();
    }
    else if ( event->button() == Qt::LeftButton )
    {
        if ( _zoom.isNull() )
        {
            QPoint click = event->pos();
            /* keep the zoom point in unscaled math */
            _zoom.rx() = double( click.x() ) * _pix.width() / width();
            _zoom.ry() = double( click.y() ) * _pix.height() / height();
        } else {
            _best = findBest( event->pos() );

            if ( notify() )
                YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );

            blink->start();
        }
    } else
        return;

    update();
}

void YQTimezoneSelector::paintEvent( QPaintEvent *event )
{
    QFrame::paintEvent( event );
    QPainter p( this );
    if ( _zoom.isNull() )
    {
        QImage t = _pix.toImage().scaled( width(),  height() );
        p.drawPixmap( 0, 0, QPixmap::fromImage( t ) );

        setCursor( QCursor( QPixmap( "/home/col/prod/ubiquity/pixmaps/zoom-in.png" ) ) );
    } else {
        int left = qMin( qMax( _zoom.x() - width() / 2, 0 ), _pix.width() - width() );
        int top  = qMin( qMax( _zoom.y() - height() / 2, 0 ), _pix.height() - height() );
        p.drawPixmap( QPoint( 0, 0 ), _pix, QRect( QPoint( left, top ), size() ) );

        if ( highlight )
        {
            QPoint pos = pixPosition( _best );
            p.setPen( Qt::red );
            p.drawRect( pos.x() - left - 1, pos.y() - top - 1, 3, 3 );
            qDebug() << QRect( pos.x() - left - 1, pos.y() - top - 1, 3, 3 ) << event->region().rects();
        }
        setCursor( Qt::CrossCursor );
    }
}

YQTimezoneSelector::Location YQTimezoneSelector::findBest( const QPoint &pos ) const
{
    double min_dist = 2000;
    Location best;
    for ( QList<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it )
    {
        double dist = QPoint( pixToWindow( ( *it ).pix_pos ) - pos ).manhattanLength ();
        if ( dist < min_dist )
        {
            min_dist = dist;
            best = *it;
        }
    }
    return best;
}

bool YQTimezoneSelector::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        Location best = findBest( helpEvent->pos() );
        QToolTip::showText(helpEvent->globalPos(), best.tip );
    }
    return QWidget::event(event);
}


std::string YQTimezoneSelector::currentZone() const
{
    return _best.zone.toStdString();
}

QPoint YQTimezoneSelector::pixToWindow( const QPoint &pos ) const
{
    int left = qMin( qMax( _zoom.x() - width() / 2, 0 ), _pix.width() - width() );
    int top  = qMin( qMax( _zoom.y() - height() / 2, 0 ), _pix.height() - height() );

    return QPoint( pos.x() - left, pos.y() - top );
}

void YQTimezoneSelector::setCurrentZone( const std::string &_zone )
{
    QString zone = QString::fromStdString( _zone );

    if ( _best.zone == zone )
        return;

    _best = Location();

    for ( QList<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it )
    {
        if ( ( *it ).zone == zone )
            _best = *it;
    }

    _zoom = _best.pix_pos;

    blink->start();
    update();
}

void YQTimezoneSelector::slotBlink()
{
    if ( _best.zone.isNull() )
    {
        blink->stop();
        return;
    }

    highlight = !highlight;

    QPoint current = pixToWindow( _best.pix_pos );
    update( QRect( current - QPoint( 2, 2 ), QSize( 5, 5 ) ) );
}

#include "YQTimezoneSelector.moc"
