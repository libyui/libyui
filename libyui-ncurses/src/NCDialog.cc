/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCDialog.cc

   Author:     Michael Andres <ma@suse.de>

just to make the y2makepot script happy:
textdomain "ncurses"

/-*/

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCDialog.h"
#include "NCstring.h"
#include "NCPopupInfo.h"
#include "NCMenuButton.h"
#include <yui/YShortcut.h>
#include "NCtoY2Event.h"
#include "YNCursesUI.h"
#include <yui/YDialogSpy.h>
#include <yui/YDialog.h>
#include <yui/YHttpServer.h>

#include "ncursesw.h"

#include <chrono>

static bool hiddenMenu()
{
    return getenv( "Y2NCDBG" ) != NULL;
}


NCDialog::NCDialog( YDialogType		dialogType,
		    YDialogColorMode	colorMode )
    : YDialog( dialogType, colorMode )
    , pan( 0 )
    , dlgstyle( 0 )
    , inMultiDraw_i( 0 )
    , active( false )
    , wActive( this )
    , ncdopts( DEFAULT )
    , popedpos( -1 )
{
    yuiDebug() << "Constructor NCDialog(YDialogType t, YDialogColorMode c)" << std::endl;
    _init();
}


NCDialog::NCDialog( YDialogType dialogType, const wpos at, const bool boxed )
	: YDialog( dialogType, YDialogNormalColor )
	, pan( 0 )
	, dlgstyle( 0 )
	, inMultiDraw_i( 0 )
	, active( false )
	, wActive( this )
	, ncdopts( boxed ? POPUP : POPUP | NOBOX )
	, popedpos( at )
{
    yuiDebug() << "Constructor NCDialog(YDialogType t, const wpos at, const bool boxed)" << std::endl;
    _init();
}


void NCDialog::_init()
{
    NCurses::RememberDlg( this );
    // don't set text domain to ncurses - other text domains won't work (bnc #476245)

    _init_size();
    wstate = NC::WSdumb;

    if ( colorMode() == YDialogWarnColor )
    {
	mystyleset = NCstyle::WarnStyle;
    }
    else if ( colorMode() == YDialogInfoColor )
    {
	mystyleset = NCstyle::InfoStyle;
    }
    else if ( isPopup() )
    {
	mystyleset = NCstyle::PopupStyle;
    }
    else
    {
	mystyleset = NCstyle::DefaultStyle;
    }

    dlgstyle = &NCurses::style()[mystyleset];

    eventReason = YEvent::UnknownReason;
    yuiDebug() << "+++ " << this << std::endl;
}


void NCDialog::_init_size()
{
    defsze.H = NCurses::lines();
    defsze.W = NCurses::cols();
    hshaddow = vshaddow = false;

    if ( isBoxed() )
    {
	switch ( defsze.H )
	{
	    case 1:
	    case 2:
		defsze.H = 1;
		break;

	    default:
		defsze.H -= 2;
		break;
	}

	switch ( defsze.W )
	{
	    case 1:
	    case 2:
		defsze.W = 1;
		break;

	    default:
		defsze.W -= 2;
		break;
	}
    }
}


NCDialog::~NCDialog()
{
    NCurses::ForgetDlg( this );

    yuiDebug() << "--+START destroy " << this << std::endl;

    if ( pan && !pan->hidden() )
    {
	pan->hide();
	doUpdate();
    }

    grabActive( 0 );

    NCWidget::wDelete();
    delete pan;
    pan = 0;
    yuiDebug() << "---destroyed " << this << std::endl;

}


int NCDialog::preferredWidth()
{
    if ( dialogType() == YMainDialog || ! hasChildren() )
	return	wGetDefsze().W;

    wsze csze( 0, 0 );

    if ( hasChildren() )
    {
	csze = wsze( firstChild()->preferredHeight(),
		     firstChild()->preferredWidth() );
    }

    csze = wsze::min( wGetDefsze(), wsze::max( csze, wsze( 1 ) ) );

    return csze.W;
}


int NCDialog::preferredHeight()
{
    if ( dialogType() == YMainDialog || ! hasChildren() )
    {
	return wGetDefsze().H;
    }

    wsze csze( 0, 0 );

    if ( hasChildren() )
    {
	csze = wsze( firstChild()->preferredHeight(),
		     firstChild()->preferredWidth() );
    }

    csze = wsze::min( wGetDefsze(),
		      wsze::max( csze, wsze( 1 ) ) );

    return csze.H;
}


void NCDialog::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    yuiDebug() << "setSize() called: width: " << newwidth << "	 height: " << newheight << std::endl;
    YDialog::setSize( newwidth, newheight );
}


void NCDialog::initDialog()
{
    if ( !pan )
    {
	yuiDebug() << "setInitialSize() called!" << std::endl;
	setInitialSize();
    }
}


void NCDialog::openInternal()
{
    showDialog();
}


void NCDialog::showDialog()
{
    yuiDebug() << "sd+ " << this << std::endl;

    if ( pan && pan->hidden() )
    {
	YPushButton *defaultB = YDialog::defaultButton();

	if ( defaultB )
	{
	    defaultB->setKeyboardFocus();
	}

	getVisible();

	doUpdate();
	DumpOn( yuiDebug(), " " );

    }
    else if ( !pan )
    {
	yuiMilestone() << "no pan" << std::endl;
    }

    activate( true );

    yuiDebug() << "sd- " << this << std::endl;
}


void NCDialog::closeDialog()
{
    yuiDebug() << "cd+ " << this << std::endl;
    activate( false );

    if ( pan && !pan->hidden() )
    {
	pan->hide();
	doUpdate();
	yuiDebug() << this << std::endl;
    }

    yuiDebug() << "cd+ " << this << std::endl;
}


void NCDialog::activate( const bool newactive )
{
    if ( active != newactive || ( pan && pan->hidden() ) )
    {
	active = newactive;

	if ( pan )
	{
	    pan->show(); // not getVisible() because wRedraw() follows.
	    wRedraw();

	    if ( active )
		Activate();
	    else
		Deactivate();

	    NCurses::SetStatusLine( describeFunctionKeys() );
	    doUpdate();
	    yuiDebug() << this << std::endl;
	}
    }
}


/**
 * Implementation of YDialog::activate().
 *
 * This is called e.g. for the next-lower dialog in the dialog stack when the
 * topmost dialog is destroyed: That next-lower dialog is now the active
 * dialog.
 **/
void NCDialog::activate()
{
    activate( true ); // Forward to NCurses-specific activate()
}


void NCDialog::wMoveTo( const wpos & newpos )
{
    yuiDebug() << DLOC << this << newpos << std::endl;
}


void NCDialog::wCreate( const wrect & newrect )
{
    if ( win )
	throw NCError( "wCreate: already have win" );

    wrect panrect( newrect );

    inparent = newrect;

    if ( isBoxed() )
    {
	switch ( NCurses::lines() - panrect.Sze.H )
	{
	    case 0:
		break;

	    case 1:
		panrect.Sze.H += 1;
		inparent.Pos.L += 1;
		break;

	    default:
		panrect.Sze.H += 2;
		inparent.Pos.L += 1;
		break;
	}

	switch ( NCurses::cols() - panrect.Sze.W )
	{
	    case 0:
		break;

	    case 1:
		panrect.Sze.W += 1;
		inparent.Pos.C += 1;
		break;

	    default:
		panrect.Sze.W += 2;
		inparent.Pos.C += 1;
		break;
	}
    }

    if ( popedpos.L >= 0 )
    {
	if ( popedpos.L + panrect.Sze.H <= NCurses::lines() )
	    panrect.Pos.L = popedpos.L;
	else
	    panrect.Pos.L = NCurses::lines() - panrect.Sze.H;
    }
    else
    {
	panrect.Pos.L = ( NCurses::lines() - panrect.Sze.H ) / 2;
    }

    if ( popedpos.C >= 0 )
    {
	if ( popedpos.C + panrect.Sze.W <= NCurses::cols() )
	    panrect.Pos.C = popedpos.C;
	else
	    panrect.Pos.C = NCurses::cols() - panrect.Sze.W;
    }
    else
    {
	panrect.Pos.C = ( NCurses::cols() - panrect.Sze.W ) / 2;
    }

    if ( panrect.Pos.L + panrect.Sze.H < NCurses::lines() )
    {
	++panrect.Sze.H;
	hshaddow = true;
    }

    if ( panrect.Pos.C + panrect.Sze.W < NCurses::cols() )
    {
	++panrect.Sze.W;
	vshaddow = true;
    }

    if ( pan && panrect != wrect( wpos( pan->begy(), pan->begx() ),
				  wsze( pan->maxy() + 1, pan->maxx() + 1 ) ) )
    {
	pan->hide();
	doUpdate();
	delete pan;
	pan = 0;
    }

    if ( !pan )
    {
	pan = new NCursesUserPanel<NCDialog>( panrect.Sze.H, panrect.Sze.W,
					      panrect.Pos.L, panrect.Pos.C,
					      this );
	pan->hide();
	doUpdate();
    }

    win = new NCursesWindow( *pan,

			     inparent.Sze.H, inparent.Sze.W,
			     inparent.Pos.L, inparent.Pos.C,
			     'r' );
    win->nodelay( true );

    yuiDebug() << DLOC << panrect << '(' << inparent << ')'
    << '[' << popedpos << ']' << std::endl;
}


void NCDialog::wRedraw()
{
    if ( pan )
    {
	if ( isBoxed() )
	{
	    pan->bkgdset( wStyle().getDlgBorder( active ).text );

	    if ( pan->height() != NCurses::lines()
		 || pan->width() != NCurses::cols() )
	    {
		pan->box();	// not fullscreen
	    }
	    else
	    {
		pan->hline( 0, 0, pan->width(), ' ' );
		pan->hline( pan->height() - 1, 0, pan->width(), ' ' );
		pan->vline( 0, 0, pan->height(), ' ' );
		pan->vline( 0, pan->width() - 1, pan->height(), ' ' );
	    }

	    if ( hshaddow )
	    {
		pan->copywin( *pan,
			      pan->maxy(), 0,
			      pan->maxy() - 1, 0,
			      pan->maxy() - 1, pan->maxx(), false );
	    }

	    if ( vshaddow )
	    {
		pan->copywin( *pan,
			      0, pan->maxx(),
			      0, pan->maxx() - 1,
			      pan->maxy(), pan->maxx() - 1, false );
	    }
	}

	pan->bkgdset( A_NORMAL );

	if ( hshaddow )
	{
	    pan->hline( pan->maxy(), 0, pan->width(), ' ' );
	    pan->transparent( pan->maxy(), 0 );
	}

	if ( vshaddow )
	{
	    pan->vline( 0, pan->maxx(), pan->height(), ' ' );
	    pan->transparent( 0, pan->maxx() );
	}
    }
}


void NCDialog::wRecoded()
{
    if ( pan )
    {
	if ( &NCurses::style()[mystyleset] != dlgstyle )
	{
	    dlgstyle = &NCurses::style()[mystyleset];
	}

	pan->bkgdset( wStyle(). getDumb().text );

	pan->clear();
	wRedraw();
    }
}


void NCDialog::startMultipleChanges()
{
    ++inMultiDraw_i;
}


void NCDialog::doneMultipleChanges()
{
    if ( inMultiDraw_i > 1 )
    {
	--inMultiDraw_i;
    }
    else
    {
	inMultiDraw_i = 0;
	NCurses::SetStatusLine( describeFunctionKeys() );
	Update();
    }
}

void NCDialog::setStatusLine()
{
    NCurses::SetStatusLine( describeFunctionKeys() );
    doUpdate();
}

void NCDialog::wUpdate( bool forced_br )
{
    if ( !pan )
	return;

    if ( !forced_br
	 && ( pan->hidden() || inMultiDraw_i ) )
	return;

    NCWidget::wUpdate( forced_br );
}


void NCDialog::grabActive( NCWidget * nactive )
{
    if ( wActive && wActive != static_cast<NCWidget *>( this ) )
	wActive->grabRelease( this );

    if ( nactive && nactive != static_cast<NCWidget *>( this ) )
	nactive->grabSet( this );

    const_cast<NCWidget *&>( wActive ) = nactive;
}


void NCDialog::grabNotify( NCWidget * mgrab )
{
    if ( wActive && wActive == mgrab )
    {
	yuiDebug() << DLOC << mgrab << " active " << std::endl;
	ActivateNext();

	if ( wActive && wActive == mgrab )
	    grabActive( this );
    }
}


bool NCDialog::wantFocus( NCWidget & ngrab )
{
    return Activate( ngrab );
}


void NCDialog::wDelete()
{
    if ( pan )
    {
	yuiDebug() << DLOC << "+++ " << this << std::endl;
	NCWidget::wDelete();
	yuiDebug() << DLOC << "--- " << this << std::endl;
    }
}


NCWidget & NCDialog::GetNormal( NCWidget & startwith, SeekDir Direction )
{
    NCWidget * c = ( startwith.*Direction )( true )->Value();

    while ( c != &startwith && ( c->GetState() != NC::WSnormal || !c->winExist() ) )
    {
	if ( c->GetState() == NC::WSactive )
	{
	    yuiWarning() << "multiple active widgets in dialog? "
	    << startwith << " <-> " << c << std::endl;
	    c->SetState( NC::WSnormal ); // what else can we do?
	    break;
	}

	c = ( c->*Direction )( true )->Value();
    }

    return *c;
}


NCWidget & NCDialog::GetNextNormal( NCWidget & startwith )
{
    return GetNormal( startwith, &tnode<NCWidget *>::Next );
}


NCWidget & NCDialog::GetPrevNormal( NCWidget & startwith )
{
    return GetNormal( startwith, &tnode<NCWidget *>::Prev );
}


bool NCDialog::Activate( NCWidget & nactive )
{
    if ( nactive.GetState() == NC::WSactive )
	return true;

    if ( nactive.GetState() == NC::WSnormal )
    {
	if ( wActive->GetState() == NC::WSactive )
	    wActive->SetState( NC::WSnormal );

	if ( active )
	{
	    nactive.SetState( NC::WSactive );
	}

	grabActive( &nactive );

	return true;
    }

    return false;
}


void NCDialog::Activate( SeekDir Direction )
{
    if ( !wActive )
	grabActive( this );

    if ( Direction == 0 )
    {
	if ( Activate( *wActive ) )
	    return;   // (re)activated widget

	// can't (re)activate widget, so look for next one
	Direction = &tnode<NCWidget *>::Next;
    }

    Activate( GetNormal( *wActive, Direction ) );
}


void NCDialog::Activate()
{
    Activate( 0 );
}


void NCDialog::Deactivate()
{
    if ( wActive->GetState() == NC::WSactive )
    {
	wActive->SetState( NC::WSnormal );
    }
}


void NCDialog::ActivateNext()
{
    Activate( &tnode<NCWidget *>::Next );
}


void NCDialog::ActivatePrev()
{
    Activate( &tnode<NCWidget *>::Prev );
}


bool NCDialog::ActivateByKey( int key )
{
    NCWidget * buddy = 0;

    for ( tnode<NCWidget*> * c = this->Next(); c; c = c->Next() )
    {
	switch ( c->Value()->GetState() )
	{
	    case NC::WSnormal:
	    case NC::WSactive:

		if ( c->Value()->HasHotkey( key )
		     || c->Value()->HasFunctionHotkey( key ) )
		{
		    Activate( *c->Value() );
		    return true;
		}

		if ( buddy )
		{
		    if ( c->IsDescendantOf( buddy ) )
		    {
			yuiDebug() << "BUDDY ACTIVATION FOR " << c->Value() << std::endl;
			Activate( *c->Value() );
			return true;
		    }

		    yuiDebug() << "DROP BUDDY on " << c->Value() << std::endl;

		    buddy = 0;
		}

		break;

	    case NC::WSdumb:

		if ( c->Value()->HasHotkey( key )
		     || c->Value()->HasFunctionHotkey( key ) )
		{
		    yuiDebug() << "DUMB HOT KEY " << key << " in " << c->Value() << std::endl;
		    buddy = c->Value();
		}

	    default:

		break;
	}
    }

    return false;
}


wint_t NCDialog::getinput()
{
    wint_t got = WEOF;

    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	wint_t gotwch = WEOF;
	int ret = ::get_wch( &gotwch ); // get a wide character

	if ( ret != ERR )	// get_wch() returns OK or KEY_CODE_YES on success
	{
	    got = gotwch;
	    // UTF-8 keys (above KEY_MIN) may deliver same keycode as curses KEY_...
	    // -> mark this keys

	    if ( ret == OK
		 && got > KEY_MIN )
	    {
		got += 0xFFFF;
	    }
	}
	else
	{
	    got = WEOF;
	}
    }
    else
    {
	std::wstring to;
	int gotch = ::getch();	// get the character in terminal encoding

	if ( gotch != -1 )
	{
	    if (( KEY_MIN > gotch || KEY_MAX < gotch )
		&&
		isprint( gotch ) )
	    {
		std::string str;
		str += static_cast<char>( gotch );
		// recode printable chars
		NCstring::RecodeToWchar( str, NCstring::terminalEncoding(), &to );
		got = to[0];

		if ( gotch != ( int )got )
		{
		    got += 0xFFFF;			// mark this key
		}

		yuiDebug() << "Recode: " << str << " (encoding: " << NCstring::terminalEncoding() << ") "

		<< "to wint_t: " << got << std::endl;
	    }
	    else
	    {
		got = gotch;
	    }
	}
	else
	{
	    got = WEOF;
	}
    }

    return got;
}


static int wait_for_input(int timeout_millisec)
{
    struct timeval tv;
    fd_set fdset_read, fdset_write, fdset_excpt;

    // infinite timout => do blocking select()
    timeval *tv_ptr = (timeout_millisec < 0) ? nullptr : &tv;

    // remember the original value
    int timeout_millisec_orig = timeout_millisec;

    do
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        tv.tv_sec  = 0;
        tv.tv_usec = timeout_millisec * 1000;

        FD_ZERO( &fdset_read );
        FD_ZERO( &fdset_write );
        FD_ZERO( &fdset_excpt );
        FD_SET( 0,	&fdset_read );

        // the higest fd number to watch
        int fd_max = 0;

        // watch HTTP server fd
        if (YUI::server())
        {
            yuiMilestone() << "Adding HTTP server notifiers..." << std::endl;
            YHttpServerSockets sockets = YUI::server()->sockets();

            for(int fd: sockets.read())
            {
                FD_SET( fd, &fdset_read );
                if (fd_max < fd) fd_max = fd;
            }

            for(int fd: sockets.write())
            {
                FD_SET( fd, &fdset_write );
                if (fd_max < fd) fd_max = fd;
            }

            for(int fd: sockets.exception())
            {
                FD_SET( fd, &fdset_excpt );
                if (fd_max < fd) fd_max = fd;
            }
        }

        yuiWarning() << "Calling select()... " << std::endl;
        int retval = select( fd_max + 1, &fdset_read, &fdset_write, &fdset_excpt, tv_ptr );
        yuiWarning() << "select() result: " << retval << std::endl;

        if ( retval < 0 )
        {
            if ( errno != EINTR )
                yuiError() << "error in select() (" << errno << ')' << std::endl;
        }
        else if ( retval != 0 )
        {
            if (YUI::server())
            {
                YHttpServerSockets sockets = YUI::server()->sockets();
                bool server_ready = false;

                for(int fd: sockets.read())
                {
                    if (FD_ISSET( fd, &fdset_read ))
                        server_ready = true;
                }

                for(int fd: sockets.write())
                {
                    if (FD_ISSET( fd, &fdset_write ))
                        server_ready = true;
                }

                for(int fd: sockets.exception())
                {
                    if (FD_ISSET( fd, &fdset_excpt ))
                        server_ready = true;
                }

                yuiWarning() << "Server ready: " << server_ready << std::endl;

                if (server_ready)
                {
                    YUI::server()->process_data();

                    if (timeout_millisec > 0)
                    {
                        std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();

                        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
                        yuiWarning() << "Elapsed time (ms): " << elapsed << std::endl;
                        timeout_millisec -= elapsed;
                        yuiWarning() << "Remaining time out (ms): " << timeout_millisec << std::endl;

                        // spent too much time
                        if (timeout_millisec <= 0)
                            timeout_millisec = 0;
                    }

                    // the request might have changed something in the UI, let's redraw it...
                    NCurses::Redraw();
                }
            }
        }
        // no input within timeout
        else
        {
            yuiWarning() << "Timeout " << timeout_millisec << "ms reached" << std::endl;
            return timeout_millisec_orig;
        }
    }
    while ( !FD_ISSET( 0, &fdset_read ) );

    // if there is an user input we do not need the spent time
    return 0;
}


wint_t NCDialog::getch( int timeout_millisec )
{
    wint_t got = WEOF;

    yuiWarning() << "NCDialog::getch timeout: " << timeout_millisec << std::endl;
    yuiWarning() << "pendingEvent: " << pendingEvent << std::endl;

    if ( timeout_millisec < 0 )
    {
	   // wait for input (block)
	   wait_for_input(timeout_millisec);
	   got = getinput();
    }
    else if ( timeout_millisec )
    {
        do
        {
            // wait for input
            int slept = wait_for_input(timeout_millisec);
            yuiWarning() << "slept: " << slept << std::endl;
            timeout_millisec -= slept;
            yuiWarning() << "new timeout: " << timeout_millisec << std::endl;

            got = getinput();
        }
        while ( got == WEOF && timeout_millisec > 0 );
    }
    else
    {
	    // no wait (non blocking)
	    wait_for_input(0);
	    ::nodelay( ::stdscr, true );
	    got = getinput();
    }

    if ( got == KEY_RESIZE )
    {
	NCurses::ResizeEvent();
	int i = 100;
	// after resize sometimes WEOF is returned -> skip this in no timeout mode

	do
	{
	    got =  NCDialog::getch( timeout_millisec );
	}
	while ( timeout_millisec < 0 && got == WEOF && --i );
    }

    return got;
}


bool NCDialog::flushTypeahead()
{
    // Don't throw away keys from the input buffer after a ValueChanged or
    // SelectionChanged event but save them e.g. for input in TextEntry,
    // MultiLineEdit or to scroll in lists ( bug #245476 )
    if ( eventReason == YEvent::ValueChanged ||
	 eventReason == YEvent::SelectionChanged )
    {
	yuiDebug() << "DON't flush input buffer - reason: " << eventReason << std::endl;
	return false;
    }
    else
    {
	yuiDebug() << "Flush input buffer" << std::endl;
	return true;
    }
}


void NCDialog::idleInput()
{
    if ( !pan )
    {
	yuiWarning() << DLOC << " called for uninitialized " << this << std::endl;
	::flushinp();
	return;
    }

    yuiDebug() << "idle+ " << this << std::endl;

    if ( !active )
    {
	if ( flushTypeahead() )
	{
	    ::flushinp();
	}

	doUpdate();
    }
    else
    {
	yuiDebug() << "idle+ " << this << std::endl;
	processInput( 0 );
	yuiDebug() << "idle- " << this << std::endl;
    }
}


NCursesEvent NCDialog::pollInput()
{
    yuiDebug() << "poll+ " << this << std::endl;

    if ( !pan )
    {
	yuiWarning() << DLOC << " called for uninitialized " << this << std::endl;
	return NCursesEvent::cancel;
    }

    if ( pendingEvent )
    {
	if ( active )
	{
	    activate( false );
	    yuiDebug() << this << " deactivate" << std::endl;
	}
    }
    else
    {
	if ( !active )
	{
	    activate( true );
	    yuiDebug() << this << " activate" << std::endl;
	}
    }

    NCursesEvent returnEvent = pendingEvent;

    eventReason = returnEvent.reason;
    pendingEvent = NCursesEvent::none;

    yuiDebug() << "poll- " << this << '(' << returnEvent << ')' << std::endl;
    return returnEvent;
}


NCursesEvent NCDialog::userInput( int timeout_millisec )
{
    yuiDebug() << "user+ " << this << std::endl;

    if ( flushTypeahead() )
    {
	::flushinp();
    }

    if ( !pan )
    {
	yuiWarning() << DLOC << " called for uninitialized " << this << std::endl;
	return NCursesEvent::cancel;
    }

    processInput( timeout_millisec );

    NCursesEvent returnEvent = pendingEvent;
    eventReason = returnEvent.reason;
    pendingEvent = NCursesEvent::none;

    yuiDebug() << "user- " << this << '(' << returnEvent << ')' << std::endl;
    return returnEvent;
}


/**
 * Back-end for YDialog::waitForEvent()
 **/
YEvent * NCDialog::waitForEventInternal( int timeout_millisec )
{
    NCtoY2Event cevent;
    activate( true );
    cevent = userInput( timeout_millisec ? timeout_millisec : -1 );
    activate( false );

    YEvent * yevent = cevent.propagate();

    return yevent;
}


/**
 * Back-end for YDialog::pollEvent()
 **/
YEvent * NCDialog::pollEventInternal()
{
    // no activation here, done in pollInput, if..
    NCtoY2Event cevent = pollInput();
    YEvent * yevent = cevent.propagate();

    return yevent;
}


/**
 * Process input
 *
 * timeout -1 -> wait for input
 * timeout  0 -> immediate return
 * else wait for up to timeout milliseconds
 **/
void NCDialog::processInput( int timeout_millisec )
{
    yuiDebug() << "process+ " << this << " active " << wActive
    << " timeout_millisec " << timeout_millisec << std::endl;

    if ( pendingEvent )
    {
	yuiDebug() << this << "(return pending event)" << std::endl;
	doUpdate();
	::flushinp();
	return;
    }

    // if no active item return on any input
    if ( wActive->GetState() != NC::WSactive )
    {
	yuiDebug() << "noactive item => reactivate!" << std::endl;
	Activate();
    }

    if ( wActive->GetState() != NC::WSactive )
    {
	yuiDebug() << "still noactive item!" << std::endl;

	if ( timeout_millisec == -1 )
	{
	    pendingEvent = NCursesEvent::cancel;
	    yuiDebug() << DLOC << this << "(std::set ET_CANCEL since noactive item on pollInput)" << std::endl;
	    getch( -1 );
	}
	else
	    ::flushinp();

	// if there is no active widget and we are in timeout, handle properly
	// bug #182982
	if ( timeout_millisec > 0 )
	{
	    usleep( timeout_millisec * 1000 );
	    pendingEvent = NCursesEvent::timeout;
	}

	return;
    }

    // get and process user input
    wint_t ch = 0;

    wint_t hch = 0;

    yuiDebug() << "enter loop..." << std::endl;

    noUpdates = true;

    while ( !pendingEvent.isReturnEvent() && ch != WEOF )
    {
        yuiWarning() << "starting loop..." << std::endl;

	ch = getch( timeout_millisec );

	switch ( ch )
	{
	    // case KEY_RESIZE: is directly handled in NCDialog::getch.

	    case WEOF:

		if ( timeout_millisec == -1 )
		    pendingEvent = NCursesEvent::cancel;
		else if ( timeout_millisec > 0 )
		    pendingEvent = NCursesEvent::timeout;

		break;

	    case KEY_F( 13 ): // = Shift-F1 on e.g. a linux console
		showHotkeyHelp();
		break;

	    case KEY_F( 16 ): // = Shift-F4 on e.g. a linux console
		const_cast<NCstyle&>( NCurses::style() ).nextStyle();

		NCurses::Redraw();

		break;

	    case KEY_F( 18 ): // = Shift-F6 on e.g. a linux console
                {
                    yuiMilestone() << "Asking for widget ID" << std::endl;
                    YWidget * widget = YNCursesUI::ui()->askSendWidgetID();

                    if ( widget )
                    {
                        NCPushButton * button = dynamic_cast<NCPushButton *>( widget );

                        if ( button )
                        {
                            Activate( *button );
                            pendingEvent = getInputEvent( KEY_RETURN );
                        }
                    }
                }
		break;

	    case CTRL( 'D' ):
		hch = getch( -1 );

		::flushinp();

		switch ( hch )
		{
		    case KEY_F( 1 ):
			showHotkeyHelp();
			break;

		    case 'D':
			yuiMilestone() << "CTRL('D')-'D' DUMP+++++++++++++++++++++" << std::endl;
			NCurses::ScreenShot();
			yuiMilestone() << this << std::endl;
			DumpOn( yuiMilestone(), " " );
			yuiMilestone() << "CTRL('D')-'D' DUMP---------------------" << std::endl;
			break;

		    case 'S':

			if ( hiddenMenu() )
			{
			    yuiMilestone() << "CTRL('D')-'S' STYLEDEF+++++++++++++++++++++" << std::endl;
			    const_cast<NCstyle&>( NCurses::style() ).changeSyle();
			    NCurses::Redraw();
			    yuiMilestone() << "CTRL('D')-'S' STYLEDEF---------------------" << std::endl;
			}

			break;

		    case 'Y':
			YDialogSpy::showDialogSpy();
			break;

		}

		break;

	    case KEY_TAB:

	    case CTRL( 'F' ):
		ActivateNext();
		break;

	    case KEY_BTAB:

	    case CTRL( 'B' ):
		ActivatePrev();
		break;

	    case CTRL( 'L' ):
		NCurses::Refresh();
		break;

	    case CTRL( 'A' ):
		pendingEvent = getInputEvent( KEY_SLEFT );
		break;

	    case CTRL( 'E' ):
		pendingEvent = getInputEvent( KEY_SRIGHT );
		break;

	    case KEY_ESC:

	    case CTRL( 'X' ):
		hch = getch( 0 );
		::flushinp();

		switch ( hch )
		{
		    case WEOF: // no 2nd char, handle ch
                        pendingEvent = getInputEvent( ch );
			break;

		    case KEY_ESC:

		    case CTRL( 'X' ):
			pendingEvent = getInputEvent( hch );
			break;

		    default:
			pendingEvent = getHotkeyEvent( hch );
			break;
		}

		break;

	    default:
                if ( ch >= KEY_F( 1 ) && ch <= KEY_F( 24 ) )
                {
                    pendingEvent = getHotkeyEvent( ch );
                }
		else
		{
                    pendingEvent = getInputEvent( ch );
                }

		break;
	}

	doUpdate();
    }

    noUpdates = false;

    yuiDebug() << "process- " << this << " active " << wActive << std::endl;
}


NCursesEvent NCDialog::getInputEvent( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;

    if ( wActive->isValid() )
    {
	ret = wHandleInput( ch );
	ret.widget = wActive;
    }

    return ret;
}


NCursesEvent NCDialog::wHandleInput( wint_t ch )
{
    return wActive->wHandleInput( ch );
}


NCursesEvent NCDialog::getHotkeyEvent( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    if ( wActive->isValid() )
    {
	ret = wHandleHotkey( key );
	ret.widget = wActive;
    }

    return ret;
}


NCursesEvent NCDialog::wHandleHotkey( wint_t key )
{
    if ( key >= 0 && ActivateByKey( key ) )
	return wActive->wHandleHotkey( key );

    return NCursesEvent::none;
}


std::ostream & operator<<( std::ostream & STREAM, const NCDialog * OBJ )
{
    if ( OBJ )
	return STREAM << *OBJ;

    return STREAM << "(NoNCDialog)";
}



/**
 * Create description for function keys:
 *
 * Get all PushButtons and MenuButtons that have a function key std::set
 * (`opt(`key_Fn) in YCP) and create a std::map:
 * $[ 1: "Help", 2: "Info",... ]
 * NCurses::SetStatusLine will process this.
 **/
std::map<int, NCstring> NCDialog::describeFunctionKeys( )
{
    std::map<int, NCstring> fkeys;

    for ( tnode<NCWidget*> * c = this->Next(); c; c = c->Next() )
    {
	YWidget * w = dynamic_cast<YWidget *>( c->Value() );

	if ( w && w->hasFunctionKey() && w->isEnabled() )
	{
	    // Retrieve the widget's "shortcut property" that describes
	    // whatever it is - regardless of widget type (PushButton, ...)

	    fkeys[ w->functionKey()] = NCstring(w->debugLabel());
	}
    }

    return fkeys;
}


std::ostream & operator<<( std::ostream & STREAM, const NCDialog & OBJ )
{
    STREAM << ( const NCWidget & )OBJ << ' ' << OBJ.pan
    << ( OBJ.active ? "{A " : "{i " ) << OBJ.pendingEvent;

    if ( OBJ.pendingEvent )
	STREAM << OBJ.pendingEvent.widget;

    return STREAM << '}';
}


bool NCDialog::getInvisible()
{
    if ( !pan || pan->hidden() )
	return false; // no change in visibility

    // just do it.
    // caller is responsible for screen update.
    pan->hide();

    return true;
}


bool NCDialog::getVisible()
{
    if ( !pan || !pan->hidden() )
	return false; // no change in visibility

    // just do it.
    // caller is responsible for screen update.
    pan->show();

    if ( hshaddow )
    {
	pan->transparent( pan->maxy(), 0 );
    }

    if ( vshaddow )
    {
	pan->transparent( 0, pan->maxx() );
    }

    return true;
}


void NCDialog::resizeEvent()
{
    _init_size();

    if ( pan )
    {
	setInitialSize();
    }
}

void NCDialog::showHotkeyHelp()
{
    std::string old_textdomain = textdomain( NULL );
    setTextdomain( "ncurses" );

    YDialog::showText(
	_( "<h1>Advanced Hotkeys:</h1>"
           "<p><b>Shift-F1</b> Show a list of advanced hotkeys.</p>"
           "<p><b>Shift-F4</b> Change color schema.</p>"
           "<p><b>Ctrl-\\</b> Quit the application.</p>"
           "<p><b>Ctrl-L</b> Refresh screen.</p>"
           "<p><b>Ctrl-D F1</b> Show a list of advanced hotkeys.</p>"
           "<p><b>Ctrl-D Shift-D</b> Dump dialog to the log file as a screen shot.</p>"
           "<p><b>Ctrl-D Shift-Y</b> Open YDialogSpy to see the widget hierarchy.</p>"
           "<p>Depending on your desktop environment some of these key combinations <br/>might not work.</p>" ),
	true );

    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}
