/****************************************************************************
    Implementation of QXEmbed class

    Copyright (C) 1999-2002 Trolltech AS

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*****************************************************************************/

#include <qapplication.h>
#include <qptrlist.h>
#include <qptrdict.h>
#include <qguardedptr.h>
#include <qwhatsthis.h>
#include <qfocusdata.h>
#ifdef Q_WS_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
//#include <kdebug.h>
//#include <kxeventutil.h>
//#include <kqeventutil.h>
#include <config.h>

#if 0
#define HAVE_UNISTD_H
#endif
#define HAVE_USLEEP

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#ifdef HAVE_USLEEP
#define USLEEP(x) usleep(x)
#else
#define USLEEP(x) sleep(0)
#endif // HAVE_USLEEP
#else
#define USLEEP(x) sleep(0)
#endif // HAVE_UNISTD_H

#include "qxembed.h"

#ifndef XK_ISO_Left_Tab
#define XK_ISO_Left_Tab                                 0xFE20
#endif
const int XFocusOut = FocusOut;
const int XFocusIn = FocusIn;
const int XKeyPress = KeyPress;
const int XKeyRelease = KeyRelease;
#undef KeyRelease
#undef KeyPress
#undef FocusOut
#undef FocusIn

// defined in qapplication_x11.cpp
extern Atom qt_wm_protocols;
extern Atom qt_wm_delete_window;
extern Atom qt_wm_take_focus;
extern Time qt_x_time;
extern Atom qt_wm_state;

static Atom xembed = 0;
static Atom context_help = 0;

// xembed messages
#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_WINDOW_ACTIVATE          1
#define XEMBED_WINDOW_DEACTIVATE        2
#define XEMBED_REQUEST_FOCUS            3
#define XEMBED_FOCUS_IN                 4
#define XEMBED_FOCUS_OUT                5
#define XEMBED_FOCUS_NEXT               6
#define XEMBED_FOCUS_PREV               7

// xembed details

// XEMBED_FOCUS_IN:
#define XEMBED_FOCUS_CURRENT    0
#define XEMBED_FOCUS_FIRST              1
#define XEMBED_FOCUS_LAST               2



class QXEmbedData
{
public:
    QXEmbedData(){ 
        autoDelete = TRUE;
        xplain = FALSE;
        xgrab = FALSE;
        lastPos = QPoint(0,0);
    }
    ~QXEmbedData(){};

    
    bool autoDelete;
    bool xplain;
    bool xgrab;
    QWidget* focusProxy;
    QPoint lastPos;
};

namespace
{
class QXEmbedAppFilter : public QObject
{
public:
    QXEmbedAppFilter(){ qApp->installEventFilter( this ); }
    ~QXEmbedAppFilter(){};
    bool eventFilter( QObject *, QEvent * );
};
} // namespace

static QXEmbedAppFilter* filter = 0;
static QPtrDict<QGuardedPtr<QWidget> > *focusMap = 0;

static XKeyEvent last_key_event;
static bool tabForward = TRUE;


class QPublicWidget : public QWidget
{
public:
    QTLWExtra* topData() { return QWidget::topData(); };
    QFocusData *focusData(){ return QWidget::focusData(); };
};

typedef int (*QX11EventFilter) (XEvent*);
extern QX11EventFilter qt_set_x11_event_filter (QX11EventFilter filter);
static QX11EventFilter oldFilter = 0;


static void send_xembed_message( WId window, long message, long detail = 0,
                                 long data1 = 0, long data2 = 0)
{
    if (!window) return;
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.window = window;
    ev.xclient.message_type = xembed;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = qt_x_time;
    ev.xclient.data.l[1] = message;
    ev.xclient.data.l[2] = detail;
    ev.xclient.data.l[3] = data1;
    ev.xclient.data.l[4] = data2;
    XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, &ev);
}

static void sendClientMessage(Window window, Atom a, long x)
{
  if (!window) return;
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xclient.type = ClientMessage;
  ev.xclient.window = window;
  ev.xclient.message_type = a;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = x;
  ev.xclient.data.l[1] = qt_x_time;
  XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, &ev);
}

static void sendFocusMessage(Window window, int type, int mode, int detail)
{
  if (!window) return;
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xfocus.type = type;
  ev.xfocus.window = window;
  ev.xfocus.mode = mode;
  ev.xfocus.detail = detail;
  XSendEvent(qt_xdisplay(), window, FALSE, FocusChangeMask, &ev);
}


bool QXEmbedAppFilter::eventFilter( QObject *o, QEvent * e)
{
    static bool obeyFocus = FALSE;
    switch ( e->type() ) {
    case QEvent::MouseButtonPress:
        if ( !((QWidget*)o)->isActiveWindow() )
            obeyFocus = TRUE;
        break;
    case QEvent::FocusIn:
        if ( qApp->focusWidget() == o &&
             ((QPublicWidget*)qApp->focusWidget()->topLevelWidget())->topData()->embedded ) {
            QFocusEvent* fe = (QFocusEvent*) e;

            if ( obeyFocus || fe->reason() == QFocusEvent::Mouse ||
                 fe->reason() == QFocusEvent::Shortcut ) {
                WId window = ((QPublicWidget*)qApp->focusWidget()->topLevelWidget())->topData()->parentWinId;
                focusMap->remove( qApp->focusWidget()->topLevelWidget() );
                send_xembed_message( window, XEMBED_REQUEST_FOCUS );
            } else if ( fe->reason() == QFocusEvent::ActiveWindow ) {
                focusMap->remove( qApp->focusWidget()->topLevelWidget() );
                focusMap->insert( qApp->focusWidget()->topLevelWidget(),
                                  new QGuardedPtr<QWidget>(qApp->focusWidget()->topLevelWidget()->focusWidget() ) );
                qApp->focusWidget()->clearFocus();
            }
            obeyFocus = FALSE;
        }
        break;
    default:
        break;
    }
    return FALSE;
}


static int qxembed_x11_event_filter( XEvent* e)
{
    //kdDebug() << "qxembed_x11_event_filter " << KXEventUtil::getX11EventInfo(e) << endl;

    switch ( e->type ) {
    case XKeyPress: {
        int kc = XKeycodeToKeysym(qt_xdisplay(), e->xkey.keycode, 0);
        if ( kc == XK_Tab || kc == XK_ISO_Left_Tab ) {
            tabForward = (e->xkey.state & ShiftMask) == 0;
            QWidget* w = QWidget::find( e->xkey.window );
            if ( w && w->isActiveWindow() && qApp->focusWidget() &&
                 qApp->focusWidget()->topLevelWidget() == w->topLevelWidget() &&
                 ((QPublicWidget*)w->topLevelWidget())->topData()->embedded ) {
                WId window = ((QPublicWidget*)w->topLevelWidget())->topData()->parentWinId;
                QFocusData *fd = ((QPublicWidget*)w)->focusData();
                while ( fd->next() != w->topLevelWidget() )
                    ;
                QWidget* first = fd->next();
                QWidget* last = fd->prev(); last = fd->prev();
                if ( !tabForward && fd->focusWidget() == first ) {
                    send_xembed_message( window, XEMBED_FOCUS_PREV );
                    return TRUE;
                } else if ( tabForward && fd->focusWidget() == last ) {
                    send_xembed_message( window, XEMBED_FOCUS_NEXT );
                    return TRUE;
                }
            }
        } else
            last_key_event = e->xkey;
    }
    // FALL THROUGH
    case XKeyRelease: {
        last_key_event = e->xkey;
        break;
    }
    case ClientMessage:
        if ( e->xclient.message_type == xembed ) {
            Time msgtime = (Time) e->xclient.data.l[0];
            long message = e->xclient.data.l[1];
            long detail = e->xclient.data.l[2];
            if ( msgtime > qt_x_time )
                qt_x_time = msgtime;
            QWidget* w = QWidget::find( e->xclient.window );
            if ( !w )
                break;
            switch ( message) {
            case XEMBED_EMBEDDED_NOTIFY:
                ((QPublicWidget*)w->topLevelWidget())->topData()->embedded = 1;
                w->topLevelWidget()->show();
                break;
            case XEMBED_WINDOW_ACTIVATE: {
                // fake focus in
                XEvent ev;
                memset(&ev, 0, sizeof(ev));
                ev.xfocus.display = qt_xdisplay();
                ev.xfocus.type = XFocusIn;
                ev.xfocus.window = w->topLevelWidget()->winId();
                ev.xfocus.mode = NotifyNormal;
                ev.xfocus.detail = NotifyAncestor;
                qApp->x11ProcessEvent( &ev );
            }
            break;
            case XEMBED_WINDOW_DEACTIVATE: {
                // fake focus out
                XEvent ev;
                memset(&ev, 0, sizeof(ev));
                ev.xfocus.display = qt_xdisplay();
                ev.xfocus.type = XFocusOut;
                ev.xfocus.window = w->topLevelWidget()->winId();
                ev.xfocus.mode = NotifyNormal;
                ev.xfocus.detail = NotifyAncestor;
                qApp->x11ProcessEvent( &ev );
            }
            break;
            case XEMBED_FOCUS_IN:
                {
                    QWidget* focusCurrent = 0;
                    QGuardedPtr<QWidget>* fw = focusMap->find( w->topLevelWidget() );
                    if ( fw ) {
                        focusCurrent = *fw;
                        focusMap->remove( w->topLevelWidget() );
                    }
                    switch ( detail ) {
                    case XEMBED_FOCUS_CURRENT:
                        if ( focusCurrent )
                            focusCurrent->setFocus();
                        else if ( !w->topLevelWidget()->focusWidget() )
                            w->topLevelWidget()->setFocus();
                        break;
                    case XEMBED_FOCUS_FIRST:
                        {
                            QFocusData *fd = ((QPublicWidget*)w)->focusData();
                            while ( fd->next() != w->topLevelWidget() )
                                ;
                            QWidget* fw = fd->next();
                            QFocusEvent::setReason( QFocusEvent::Tab );
                            if ( w )
                                fw->setFocus();
                            else
                                w->topLevelWidget()->setFocus();
                            QFocusEvent::resetReason();
                        }
                        break;
                    case XEMBED_FOCUS_LAST:
                        {
                            QFocusData *fd = ((QPublicWidget*)w)->focusData();
                            while ( fd->next() != w->topLevelWidget() )
                                ;
                            QWidget* fw = fd->prev();
                            QFocusEvent::setReason( QFocusEvent::Tab );
                            if ( w )
                                fw->setFocus();
                            else
                                w->topLevelWidget()->setFocus();
                            QFocusEvent::resetReason();
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case XEMBED_FOCUS_OUT:
                if ( w->topLevelWidget()->focusWidget() ) {
                    focusMap->insert( w->topLevelWidget(),
                                      new QGuardedPtr<QWidget>(w->topLevelWidget()->focusWidget() ) );
                    w->topLevelWidget()->focusWidget()->clearFocus();
                }
            break;
            default:
                break;
            }
        } else if ( e->xclient.format == 32 && e->xclient.message_type ) {
            if ( e->xclient.message_type == qt_wm_protocols ) {
                QWidget* w = QWidget::find( e->xclient.window );
                if ( !w )
                    break;
                Atom a = e->xclient.data.l[0];
                if ( a == qt_wm_take_focus ) {
                    if ( (ulong) e->xclient.data.l[1] > qt_x_time )
                        qt_x_time = e->xclient.data.l[1];
                    if ( w->isActiveWindow() ) {
                        // send another WindowActivate. The QXEmbed
                        // widget will filter it and pass the focus to
                        // the proxy
                        QEvent e( QEvent::WindowActivate );
                        QApplication::sendEvent( w, &e );
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    if ( oldFilter )
        return oldFilter( e );
    return FALSE;
}

/*!
  Initializes the xembed system.

  This function is called automatically when using
  embedClientIntoWindow() or creating an instance of QXEmbed You may
  have to call it manually for a client when using embedder-side
  embedding, though.
 */
void QXEmbed::initialize()
{
    static bool is_initialized = FALSE;
    if ( is_initialized )
        return;
    xembed = XInternAtom( qt_xdisplay(), "_XEMBED", FALSE );
    oldFilter = qt_set_x11_event_filter( qxembed_x11_event_filter );

    focusMap = new QPtrDict<QGuardedPtr<QWidget> >;
    focusMap->setAutoDelete( TRUE );

    filter = new QXEmbedAppFilter;

    is_initialized = TRUE;
}


/*!
  \class QXEmbed qxembed.h

  \brief The QXEmbed widget is a graphical socket that can embed an
  external X-Window.

  \extension XEmbed

  A QXEmbed widget serves as an embedder that can manage one single
  embedded X-window. These so-called client windows can be arbitrary
  QWidgets.

  There are two different ways of using QXembed, from the embedder's
  or from the client's side.  When using it from the embedder's side,
  you already know the window identifier of the window that should be
  embedded. Simply call embed() with this identifier as parameter.

  Embedding from the client's side requires that the client knows the
  window identifier of the respective embedder widget. Use either
  embedClientIntoWindow() or the high-level wrapper
  processClientCmdline().

  If a window has been embedded successfully, embeddedWinId() returns
  its id.

  Reimplement the change handler windowChanged() to catch embedding or
  the destruction of embedded windows. In the latter case, the
  embedder also emits a signal embeddedWindowDestroyed() for
  convenience.

*/

/*!
  Constructs a xembed widget.

  The \e parent, \e name and \e f arguments are passed to the QFrame
  constructor.
 */
QXEmbed::QXEmbed(QWidget *parent, const char *name, WFlags f)
  : QWidget(parent, name, f)
{
    d = new QXEmbedData;
    d->focusProxy = new QWidget( this, "xembed_focus" );
    d->focusProxy->setGeometry( -1, -1, 1, 1 );
    initialize();
    window = 0;
    setFocusPolicy(StrongFocus);
    setKeyCompression( FALSE );

    //trick to create extraData();
    (void) topData();

    // we are interested in SubstructureNotify
    XSelectInput(qt_xdisplay(), winId(),
                 KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask |
                 KeymapStateMask |
                 ButtonMotionMask |
                 PointerMotionMask | // may need this, too
                 EnterWindowMask | LeaveWindowMask |
                 FocusChangeMask |
                 ExposureMask |
                 StructureNotifyMask |
                 SubstructureRedirectMask |
                 SubstructureNotifyMask
                 );

    topLevelWidget()->installEventFilter( this );
    qApp->installEventFilter( this );

    if (isActiveWindow())
        if ( !((QPublicWidget*) topLevelWidget())->topData()->embedded )
            XSetInputFocus( qt_xdisplay(), d->focusProxy->winId(), 
                            RevertToParent, qt_x_time );

    setAcceptDrops( TRUE );
}

/*!
  Destructor. Cleans up the focus if necessary.
 */
QXEmbed::~QXEmbed()
{
  if ( d && d->xgrab)
    XUngrabButton( qt_xdisplay(), AnyButton, AnyModifier, winId() );
  
  if ( window != 0 ) {
        if ( autoDelete() )
            XUnmapWindow( qt_xdisplay(), window );
        
        XReparentWindow(qt_xdisplay(), window, qt_xrootwin(), 0, 0);
        XSync(qt_xdisplay(), FALSE);

        if ( autoDelete() ) {
            XEvent ev;
            memset(&ev, 0, sizeof(ev));
            ev.xclient.type = ClientMessage;
            ev.xclient.window = window;
            ev.xclient.message_type = qt_wm_protocols;
            ev.xclient.format = 32;
            ev.xclient.data.s[0] = qt_wm_delete_window;
            XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, &ev);
        }
        XFlush( qt_xdisplay() );
     }
  window = 0;

  delete d;
}


/*!
 Sets the protocol used for embedding windows.
 This function must be called before embedding a window.
 Protocol XEMBED provides maximal functionality (focus, tabs, etc)
 but requires explicit cooperation from the embedded window.  
 Protocol XPLAIN provides maximal compatibility with 
 embedded applications that do not support the XEMBED protocol.
 The default is XEMBED.  

 Future work:
 Create a protocol AUTO that selects the best option.  
 This will be possible with the XEMBED v2 specification.
*/
void QXEmbed::setProtocol( Protocol proto )
{
    if (window == 0) {
        d->xplain = FALSE;
        if (proto == XPLAIN)
            d->xplain = TRUE;
    }
}

/*! 
  Returns the protocol used for embedding the current window.
*/

QXEmbed::Protocol QXEmbed::protocol()
{
  if (d->xplain)
    return XPLAIN;
  return XEMBED;
}


/*!\reimp
 */
void QXEmbed::resizeEvent(QResizeEvent*)
{
    if (window != 0)
        XResizeWindow(qt_xdisplay(), window, width(), height());
}

/*!\reimp
 */
void QXEmbed::showEvent(QShowEvent*)
{
    if (window != 0)
        XMapRaised(qt_xdisplay(), window);
}


/*!\reimp
 */
bool QXEmbed::eventFilter( QObject *o, QEvent * e)
{

    switch ( e->type() ) {
    case QEvent::WindowActivate:
        if ( o == topLevelWidget() ) {
            if ( !((QPublicWidget*) topLevelWidget())->topData()->embedded )
                if (! hasFocus() )
                    XSetInputFocus( qt_xdisplay(), d->focusProxy->winId(), 
                                    RevertToParent, qt_x_time );
            if (d->xplain)
                checkGrab();
            else
                send_xembed_message( window, XEMBED_WINDOW_ACTIVATE );
        }
        break;
    case QEvent::WindowDeactivate:
        if ( o == topLevelWidget() ) {
            if (d->xplain)
                checkGrab();
            else
                send_xembed_message( window, XEMBED_WINDOW_DEACTIVATE );
        }
        break;
    case QEvent::Move:
        {
            QWidget* pos = this;
            while( pos != o && pos != topLevelWidget())
                pos = pos->parentWidget();
            if( pos == o ) {
                QPoint globalPos = mapToGlobal(QPoint(0,0));
                if (globalPos != d->lastPos) {
                    d->lastPos = globalPos;
                    sendSyntheticConfigureNotifyEvent();
                }
            }
        }                    
        break;
    default:
        break;
   }
   return FALSE;
}


bool  QXEmbed::event( QEvent * e)
{
    return QWidget::event( e );
}

/*!\reimp
 */
void QXEmbed::keyPressEvent( QKeyEvent *)
{
    if (!window)
        return;
    last_key_event.window = window;
    XSendEvent(qt_xdisplay(), window, FALSE, KeyPressMask, (XEvent*)&last_key_event);

}

/*!\reimp
 */
void QXEmbed::keyReleaseEvent( QKeyEvent *)
{
    if (!window)
        return;
    last_key_event.window = window;
    XSendEvent(qt_xdisplay(), window, FALSE, KeyReleaseMask, (XEvent*)&last_key_event);
}

/*!\reimp
 */
void QXEmbed::focusInEvent( QFocusEvent * e ){
    if (!window)
        return;
    if ( !((QPublicWidget*) topLevelWidget())->topData()->embedded )
        XSetInputFocus( qt_xdisplay(), d->focusProxy->winId(), 
                        RevertToParent, qt_x_time );
    if (d->xplain) {
        checkGrab();
        sendFocusMessage(window, XFocusIn, NotifyNormal, NotifyPointer );
    } else {
        int detail = XEMBED_FOCUS_CURRENT;
        if ( e->reason() == QFocusEvent::Tab )
            detail = tabForward?XEMBED_FOCUS_FIRST:XEMBED_FOCUS_LAST;
        send_xembed_message( window, XEMBED_FOCUS_IN, detail);
    }
}

/*!\reimp
 */
void QXEmbed::focusOutEvent( QFocusEvent * ){
    if (!window)
        return;
    if (d->xplain) {
        checkGrab();
        sendFocusMessage(window, XFocusOut, NotifyNormal, NotifyPointer );
    } else {
        send_xembed_message( window, XEMBED_FOCUS_OUT );
    }
    if ( !((QPublicWidget*) topLevelWidget())->topData()->embedded )
        XSetInputFocus( qt_xdisplay(), d->focusProxy->winId(), 
                        RevertToParent, qt_x_time );
}



static bool wstate_withdrawn( WId winid )
{
    Atom type;
    int format;
    unsigned long length, after;
    unsigned char *data;
    int r = XGetWindowProperty( qt_xdisplay(), winid, qt_wm_state, 0, 2,
                                FALSE, AnyPropertyType, &type, &format,
                                &length, &after, &data );
    bool withdrawn = TRUE;
    if ( r == Success && data && format == 32 ) {
        Q_UINT32 *wstate = (Q_UINT32*)data;
        withdrawn  = (*wstate == WithdrawnState );
        XFree( (char *)data );
    }
    return withdrawn;
}

static int get_parent(WId winid, Window *out_parent)
{
    Window root, *children=0;
    unsigned int nchildren;
    int st = XQueryTree(qt_xdisplay(), winid, &root, out_parent, &children, &nchildren);
    if (st) {
        if (children) {
            XFree(children);
        }
        return st;
    } else {
        // kdDebug() << QString("**** FAILED **** XQueryTree returns status %1").arg(st) << endl;
    }
    return st;
}
#include <unistd.h>
/*!

  Embeds the window with the identifier \a w into this xembed widget.

  This function is useful if the server knows about the client window
  that should be embedded.  Often it is vice versa: the client knows
  about its target embedder. In that case, it is not necessary to call
  embed(). Instead, the client will call the static function
  embedClientIntoWindow().

  \sa embeddedWinId()
 */
void QXEmbed::embed(WId w)
{
    // kdDebug() << "************************** Embed "<< QString("0x%1").arg(w, 0, 16) << " into " << QString("0x%1").arg(winId(), 0, 16) << " window=" << QString("0x%1").arg(window, 0, 16) << " **********" << endl; 
    if (!w)
        return;
    XAddToSaveSet( qt_xdisplay(), w );
    bool has_window =  w == window;
    window = w;
    if ( !has_window ) {
        if ( !wstate_withdrawn(window) ) {
            XWithdrawWindow(qt_xdisplay(), window, qt_xscreen());
            QApplication::flushX();
            while (!wstate_withdrawn(window))
                ;
        }
        Window parent;
        get_parent(w, &parent);
        // kdDebug() << QString(">>> before reparent: parent=0x%1").arg(parent, 0, 16) << endl;
        for (int i = 0; i < 50; i++) {
            Window parent = 0;
            XReparentWindow(qt_xdisplay(), w, winId(), 0, 0);
            if (get_parent(w, &parent) && parent == winId()) {
		// kdDebug() << QString(">>> Loop %1: reparent of 0x%2 into 0x%3 successful").arg(i).arg(w, 0, 16).arg(winId(), 0, 16) << endl;
               break;
            }
            // kdDebug() << QString(">>> Loop %1: reparent of 0x%2 into 0x%3 failed").arg(i).arg(w, 0, 16).arg(winId(), 0, 16) << endl;
            USLEEP(1000);
        }
        QApplication::syncX();
    }

    XResizeWindow(qt_xdisplay(), w, width(), height());
    XMapRaised(qt_xdisplay(), window);
    sendSyntheticConfigureNotifyEvent();
    extraData()->xDndProxy = w;

    if ( parent() ) {
        QEvent * layoutHint = new QEvent( QEvent::LayoutHint );
        QApplication::postEvent( parent(), layoutHint );
    }
    windowChanged( window );
    if (d->xplain) {
        checkGrab();
        if ( hasFocus() )
            sendFocusMessage(window, XFocusIn, NotifyNormal, NotifyPointer );
    } else {
        send_xembed_message( window, XEMBED_EMBEDDED_NOTIFY, 0, (long) winId() );
        send_xembed_message( window, isActiveWindow() ? XEMBED_WINDOW_ACTIVATE : XEMBED_WINDOW_DEACTIVATE );
        if ( hasFocus() )
            send_xembed_message( window, XEMBED_FOCUS_IN );
    }
}


/*!
  Returns the window identifier of the embedded window, or 0 if no
  window is embedded yet.
 */
WId QXEmbed::embeddedWinId() const
{
    return window;
}


/*!\reimp
 */
bool QXEmbed::focusNextPrevChild( bool next )
{
    if ( window )
        return FALSE;
    else
        return QWidget::focusNextPrevChild( next );
}


/*!\reimp
 */
bool QXEmbed::x11Event( XEvent* e)
{
    //kdDebug() << "x11Event " << KXEventUtil::getX11EventInfo(e) << endl;
    switch ( e->type ) {
    case DestroyNotify:
        if ( e->xdestroywindow.window == window ) {
            window = 0;
            windowChanged( window );
            emit embeddedWindowDestroyed();
        }
        break;
    case ReparentNotify:
        if ( e->xreparent.window == d->focusProxy->winId() )
            break; // ignore proxy
        if ( window && e->xreparent.window == window &&
             e->xreparent.parent != winId() ) {
            // we lost the window
            window = 0;
            windowChanged( window );
        } else if ( e->xreparent.parent == winId() ){
            // we got a window
            window = e->xreparent.window;
            embed( window );
        }
        break;
    case ButtonPress:
        if (d->xplain) {
            QFocusEvent::setReason( QFocusEvent::Mouse );
            setFocus();
            QFocusEvent::resetReason();
            XAllowEvents(qt_xdisplay(), ReplayPointer, CurrentTime);
            return TRUE;
        }
        break;
    case ButtonRelease:
        if (d->xplain) 
            XAllowEvents(qt_xdisplay(), SyncPointer, CurrentTime);
        break;
    case MapRequest:
        if ( window && e->xmaprequest.window == window )
            XMapRaised(qt_xdisplay(), window );
        break;
    case ClientMessage:
        if ( e->xclient.format == 32 && e->xclient.message_type == xembed ) {
            long message = e->xclient.data.l[1];
//          long detail = e->xclient.data.l[2];
            switch ( message ) {
            case XEMBED_FOCUS_NEXT:
                QWidget::focusNextPrevChild( TRUE );
                break;
            case XEMBED_FOCUS_PREV:
                QWidget::focusNextPrevChild( FALSE );
                break;
            case XEMBED_REQUEST_FOCUS:
                QFocusEvent::setReason( QFocusEvent::Mouse );
                setFocus();
                QFocusEvent::resetReason();
                break;
            default:
                break;
            }
        }
	break;

    case ConfigureRequest:
        if (e->xconfigurerequest.window == window) 
        {
             sendSyntheticConfigureNotifyEvent();
        }
        break;
    case MotionNotify: 
	// fall through, workaround for Qt 3.0 < 3.0.3
    case EnterNotify:
        if ( QWhatsThis::inWhatsThisMode() )
            enterWhatsThisMode();
        break;
    default:
        break;
    }
    return FALSE;
}

 // temporary, fix in Qt (Matthias, Mon Jul 17 15:20:55 CEST 2000  )
void QXEmbed::enterWhatsThisMode()
{
    QWhatsThis::leaveWhatsThisMode();
    if ( !context_help )
        context_help = XInternAtom( x11Display(), "_NET_WM_CONTEXT_HELP", FALSE );
    sendClientMessage(window , qt_wm_protocols, context_help );
}


/*!
  A change handler that indicates that the embedded window has been
  changed.  The window handle can also be retrieved with
  embeddedWinId().
 */
void QXEmbed::windowChanged( WId )
{
}


/*!
  A utility function for clients that embed theirselves. The widget \a
  client will be embedded in the window that is passed as
  \c -embed command line argument.

  The function returns TRUE on success or FALSE if no such command line
  parameter is specified.

  \sa embedClientIntoWindow()
 */
bool QXEmbed::processClientCmdline( QWidget* client, int& argc, char ** argv )
{
    int myargc = argc;
    WId window = 0;
    int i, j;

    j = 1;
    for ( i=1; i<myargc; i++ ) {
        if ( argv[i] && *argv[i] != '-' ) {
            argv[j++] = argv[i];
            continue;
        }
        QCString arg = argv[i];
        if ( strcmp(arg,"-embed") == 0 && i < myargc-1 ) {
            QCString s = argv[++i];
            window = s.toInt();
        } else
            argv[j++] = argv[i];
    }
    argc = j;

    if ( window != 0 ) {
        embedClientIntoWindow( client, window );
        return TRUE;
    }

    return FALSE;
}


/*!
  A  function for clients that embed themselves. The widget \a
  client will be embedded in the window \a window. The application has
  to ensure that \a window is the handle of the window identifier of
  an QXEmbed widget.

  \sa processClientCmdline()
 */
void QXEmbed::embedClientIntoWindow(QWidget* client, WId window)
{
    initialize();
    XReparentWindow(qt_xdisplay(), client->winId(), window, 0, 0);
    ((QXEmbed*)client)->topData()->embedded = TRUE;
    ((QXEmbed*)client)->topData()->parentWinId = window;
    client->show();
}



/*!
  Specifies that this widget can use additional space, and that it can
  survive on less than sizeHint().
*/

QSizePolicy QXEmbed::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}


/*!
  Returns a size sufficient for the embedded window
*/
QSize QXEmbed::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \fn void QXEmbed::embeddedWindowDestroyed()

  This signal is emitted when the embedded window has been destroyed.

  \sa embeddedWinId()
*/

/*!
  Returns the minimum size specified by the embedded window.
*/

QSize QXEmbed::minimumSizeHint() const
{
    int minw = 0;
    int minh = 0;
    if ( window ) {
        XSizeHints size;
        long msize;
        if (XGetWMNormalHints(qt_xdisplay(), window, &size, &msize)
            && ( size.flags & PMinSize) ) {
            minw = size.min_width;
            minh = size.min_height;
        }
    }

    return QSize( minw, minh );
}

void QXEmbed::setAutoDelete( bool b)
{
    d->autoDelete = b;
}

bool QXEmbed::autoDelete() const
{
    return d->autoDelete;
}

/*!\reimp
 */
bool QXEmbed::customWhatsThis() const
{
    return TRUE;
}


void QXEmbed::checkGrab() 
{
    if (d->xplain && isActiveWindow() && !hasFocus()) {
        if (! d->xgrab)
            XGrabButton(qt_xdisplay(), AnyButton, AnyModifier, winId(),
                        FALSE, ButtonPressMask, GrabModeSync, GrabModeAsync,
                        None, None );
        d->xgrab = TRUE;
    } else {
        if (d->xgrab)
            XUngrabButton( qt_xdisplay(), AnyButton, AnyModifier, winId() );
        d->xgrab = FALSE;
    }
}


void QXEmbed::sendSyntheticConfigureNotifyEvent() 
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    if (window) {
        // kdDebug(6100) << "*************** sendSyntheticConfigureNotify ******************" << endl;
        XConfigureEvent c;
        memset(&c, 0, sizeof(c));
        c.type = ConfigureNotify;
        c.display = qt_xdisplay();
        c.send_event = True;
        c.event = window;
        c.window = winId();
        c.x = globalPos.x();
        c.y = globalPos.y();
        c.width = width();
        c.height = height();
        c.border_width = 0;
        c.above = None;
        c.override_redirect = 0;
        XSendEvent( qt_xdisplay(), c.event, TRUE, StructureNotifyMask, (XEvent*)&c );
        //kdDebug(6100) << "SENT " << KXEventUtil::getX11EventInfo((XEvent*)&c) << endl;
    }
}

void QXEmbed::reparent( QWidget * parent, WFlags f, const QPoint & p, bool showIt )
{
// QWidget::reparent() destroys the old X Window for the widget, and creates a new one,
// thus QXEmbed after reparenting is no longer the parent of the embedded window.
// I think reparenting of QXEmbed can be done only by a mistake, so just complain.
    Q_ASSERT( window == 0 );
    QWidget::reparent( parent, f, p, showIt );
}

// for KDE
#include "qxembed.moc"
#endif
