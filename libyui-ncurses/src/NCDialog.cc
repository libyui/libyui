/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCDialog.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCDialog.h"
#include "NCstring.h"
#include "NCPopupInfo.h"
#include "NCMenuButton.h"
#include "YShortcut.h"
#include "PkgNames.h"
#include "NCi18n.h"

#include "ncursesw.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCDialog_"
#endif

/*
  Textdomain "packages"
*/


/******************************************************************
**
**
**	FUNCTION NAME : hiddenMenu
**	FUNCTION TYPE : static bool
**
**	DESCRIPTION :
*/
static bool hiddenMenu()
{
  return getenv( "Y2NCDBG" ) != NULL;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::NCDialog
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCDialog::NCDialog( YWidgetOpt & opt )
    : YDialog    ( opt )
    , pan        ( 0 )
    , dlgstyle   ( 0 )
    , inMultiDraw_i( 0 )
    , active     ( false )
    , wActive    ( this )
    , ncdopts    ( DEFAULT )
    , popedpos   ( -1 )
{
  _init( opt );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::NCDialog
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCDialog::NCDialog( YWidgetOpt & opt, const wpos at, const bool boxed )
    : YDialog    ( opt )
    , pan        ( 0 )
    , dlgstyle   ( 0 )
    , inMultiDraw_i( 0 )
    , active     ( false )
    , wActive    ( this )
    , ncdopts    ( boxed ? POPUP : POPUP|NOBOX )
    , popedpos   ( at )
{
    _init( opt );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::_init
//	METHOD TYPE : void
//
//	DESCRIPTION : Constructor helper
//
void NCDialog::_init( YWidgetOpt & opt )
{
  NCurses::RememberDlg( this );

  setTextdomain( "packages" );
  
  _init_size();

  wstate = NC::WSdumb;
  if ( opt.hasWarnColor.value() ) {
    mystyleset = NCstyle::WarnStyle;
  } else if ( opt.hasInfoColor.value() ) {
    mystyleset = NCstyle::InfoStyle;
  } else if ( isPopup() ) {
    mystyleset = NCstyle::PopupStyle;
  } else {
    mystyleset = NCstyle::DefaultStyle;
  }
  dlgstyle = &NCurses::style()[mystyleset];

  helpPopup = 0;

  WIDDBG << "+++ " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::_init_size
//	METHOD TYPE : void
//
void NCDialog::_init_size()
{
  defsze.H = NCurses::lines();
  defsze.W = NCurses::cols();
  hshaddow = vshaddow = false;

  if ( isBoxed() ) {
    switch ( defsze.H ) {
    case 1:
    case 2:
      defsze.H = 1;
      break;
    default:
      defsze.H -= 2;
      break;
    }
    switch ( defsze.W ) {
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::~NCDialog
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCDialog::~NCDialog()
{
  NCurses::ForgetDlg( this );

  WIDDBG << "--+START destroy " << this << endl;
  if ( pan && !pan->hidden() ) {
    pan->hide();
    doUpdate();
  }
  grabActive( 0 );
  NCWidget::wDelete();
  delete pan;
  pan = 0;
  WIDDBG << "---destroyed " << this << endl;

  if ( helpPopup )
  {
      delete helpPopup;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCDialog::nicesize( YUIDimension dim )
{
  if ( hasDefaultSize() || !numChildren() ) {
    return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
  }

  wsze csze( child( 0 )->nicesize( YD_VERT ),
	     child( 0 )->nicesize( YD_HORIZ ) );
  csze = wsze::min( wGetDefsze(),
		    wsze::max( csze, wsze( 1 ) ) );

  return dim == YD_HORIZ ? csze.W : csze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YDialog::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::initDialog
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::initDialog()
{
  if ( !pan ) {
    setInitialSize();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::showDialog
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::showDialog()
{
  IODBG << "sd+ " << this << endl;
  if ( pan && pan->hidden() ) {
    getVisible();
    doUpdate();
    DumpOn( NCDBG, " " );
  }
  activate( true );
  IODBG << "sd- " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::closeDialog
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::closeDialog()
{
  IODBG << "cd+ " << this << endl;
  activate( false );
  if ( pan && !pan->hidden() ) {
    pan->hide();
    doUpdate();
    NCDBG << this << endl;
  }
  IODBG << "cd+ " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::activate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::activate( const bool newactive )
{
  if ( active != newactive ) {
    active = newactive;
    if ( pan ) {
      pan->show(); // not getVisible() because wRedraw() follows.
      wRedraw();
      if ( active ) {
	Activate();
      } else {
	Deactivate();
      }
      doUpdate();
      IODBG << this << endl;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wMoveTo
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wMoveTo( const wpos & newpos )
{
  NCINT << DLOC << this << newpos << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wCreate( const wrect & newrect )
{
  if ( win )
    throw NCError( "wCreate: already have win" );

  wrect panrect( newrect );
  inparent = newrect;

  if ( isBoxed() ) {
    switch ( NCurses::lines() - panrect.Sze.H ) {
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
    switch ( NCurses::cols() - panrect.Sze.W ) {
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

  if ( popedpos.L >= 0 ) {
    if ( popedpos.L + panrect.Sze.H <= NCurses::lines() )
      panrect.Pos.L = popedpos.L;
    else
      panrect.Pos.L = NCurses::lines() - panrect.Sze.H;
  } else {
    panrect.Pos.L = (NCurses::lines() - panrect.Sze.H) / 2;
  }

  if ( popedpos.C >= 0 ) {
    if ( popedpos.C + panrect.Sze.W <= NCurses::cols() )
      panrect.Pos.C = popedpos.C;
    else
      panrect.Pos.C = NCurses::cols() - panrect.Sze.W;
  } else {
    panrect.Pos.C = (NCurses::cols() - panrect.Sze.W) / 2;
  }

  if ( panrect.Pos.L + panrect.Sze.H < NCurses::lines() ) {
    ++panrect.Sze.H;
    hshaddow = true;
  }
  if ( panrect.Pos.C + panrect.Sze.W < NCurses::cols() ) {
    ++panrect.Sze.W;
    vshaddow = true;
  }

  if ( pan && panrect != wrect( wpos( pan->begy(), pan->begx() ),
				wsze( pan->maxy() + 1, pan->maxx() + 1 ) ) ) {
    pan->hide();
    doUpdate();
    delete pan;
    pan = 0;
  }

  if ( !pan ) {
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

  WIDDBG << DLOC << panrect << '(' << inparent << ')'
    << '[' << popedpos << ']' << endl;
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wRedraw()
{
  if ( pan ) {
    if ( isBoxed() ) {
      pan->bkgdset( wStyle().getDlgBorder( active ).text );
     
      if (    pan->height() != NCurses::lines()
	   || pan->width() != NCurses::cols() ) {
	  pan->box();	// not fullscreen
      }
      else {	
	  pan->hline( 0, 0, pan->width(), ' ' );
	  pan->hline( pan->height()-1, 0, pan->width(), ' ' );
	  pan->vline( 0, 0, pan->height(), ' ' );
	  pan->vline( 0, pan->width()-1, pan->height(), ' ' );
      }
      
      if ( hshaddow ) {
	pan->copywin( *pan,
		      pan->maxy(), 0,
		      pan->maxy()-1, 0,
		      pan->maxy()-1, pan->maxx(), false );
      }
      if ( vshaddow ) {
	pan->copywin( *pan,
		      0, pan->maxx(),
		      0, pan->maxx()-1,
		      pan->maxy(), pan->maxx()-1, false );
      }
    }
    pan->bkgdset( A_NORMAL );
    if ( hshaddow ) {
      pan->hline( pan->maxy(), 0, pan->width(), ' ' );
      pan->transparent( pan->maxy(), 0 );
    }
    if ( vshaddow ) {
      pan->vline( 0, pan->maxx(), pan->height(), ' ' );
      pan->transparent( 0, pan->maxx() );
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wRecoded()
{
  if ( pan ) {
    if ( &NCurses::style()[mystyleset] != dlgstyle ) {
      dlgstyle = &NCurses::style()[mystyleset];
    }
    pan->bkgdset( wStyle(). getDumb().text );
    pan->clear();
    wRedraw();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::startMultipleChanges
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::startMultipleChanges()
{
  ++inMultiDraw_i;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::doneMultipleChanges
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::doneMultipleChanges()
{
  if ( inMultiDraw_i > 1 ) {
    --inMultiDraw_i;
  } else {
    inMultiDraw_i = 0;
    Update();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wUpdate( bool forced_br )
{
  if ( !pan )
    return;
  if ( !forced_br
       && (pan->hidden() || inMultiDraw_i) )
    return;
  NCWidget::wUpdate( forced_br );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::grabActive
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::grabActive( NCWidget * nactive )
{
  if ( wActive && wActive != static_cast<NCWidget *>( this ) )
    wActive->grabRelease( this );
  if ( nactive && nactive != static_cast<NCWidget *>( this ) )
    nactive->grabSet( this );
  const_cast<NCWidget *&>( wActive ) = nactive;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::grabNotify
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::grabNotify( NCWidget * mgrab )
{
  if ( wActive && wActive == mgrab ) {
    WIDDBG << DLOC << mgrab << " active " << endl;
    ActivateNext();
    if ( wActive && wActive == mgrab )
      grabActive( this );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wantFocus
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCDialog::wantFocus( NCWidget & ngrab )
{
  return Activate( ngrab );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::wDelete()
{
  if ( pan ) {
    WIDDBG << DLOC << "+++ " << this << endl;
    NCWidget::wDelete();
    WIDDBG << DLOC << "--- " << this << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::GetNormal
//	METHOD TYPE : NCWidget *
//
//	DESCRIPTION :
//
NCWidget & NCDialog::GetNormal( NCWidget & startwith, SeekDir Direction )
{
  NCWidget * c = (startwith.*Direction)( true )->Value();

  while ( c != &startwith && c->GetState() != NC::WSnormal ) {
    if ( c->GetState() == NC::WSactive ) {
      NCINT << "multiple active widgets in dialog? "
	<< startwith << " <-> " << c << endl;
      c->SetState( NC::WSnormal ); // what else can we do?
      break;
    }
    c = (c->*Direction)( true )->Value();
  }

  return *c;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::GetNextNormal
//	METHOD TYPE : NCWidget &
//
//	DESCRIPTION :
//
NCWidget & NCDialog::GetNextNormal( NCWidget & startwith )
{
  return GetNormal( startwith, &tnode<NCWidget *>::Next );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::GetPrevNormal
//	METHOD TYPE : NCWidget *
//
//	DESCRIPTION :
//
NCWidget & NCDialog::GetPrevNormal( NCWidget & startwith )
{
  return GetNormal( startwith, &tnode<NCWidget *>::Prev );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::Activate
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCDialog::Activate( NCWidget & nactive )
{
  if ( nactive.GetState() == NC::WSactive )
    return true;

  if ( nactive.GetState() == NC::WSnormal ) {
    if ( wActive->GetState() == NC::WSactive )
      wActive->SetState( NC::WSnormal );
    if ( active ) {
      nactive.SetState( NC::WSactive );
    }
    grabActive( &nactive );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::Activate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::Activate( SeekDir Direction )
{
  if ( !wActive )
    grabActive( this );

  if ( Direction == 0 ) {
    if ( Activate( *wActive ) )
      return;   // (re)activated widget
    // can't (re)activate widget, so look for next one
    Direction = &tnode<NCWidget *>::Next;
  }
  Activate( GetNormal( *wActive, Direction ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::Activate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::Activate()
{
  Activate( 0 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::Deactivate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::Deactivate()
{
  if ( wActive->GetState() == NC::WSactive ) {
    wActive->SetState( NC::WSnormal );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::ActivateNext
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::ActivateNext()
{
  Activate( &tnode<NCWidget *>::Next );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::ActivatePrev
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::ActivatePrev()
{
  Activate( &tnode<NCWidget *>::Prev );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::ActivateByKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCDialog::ActivateByKey( int key )
{
  NCWidget * buddy = 0;

  for ( tnode<NCWidget*> * c = this->Next(); c; c = c->Next() ) {
    switch ( c->Value()->GetState() ) {
    case NC::WSnormal:
    case NC::WSactive:
      if ( c->Value()->HasHotkey( key )
	   || c->Value()->HasFunctionHotkey( key ) )
      {
	Activate( *c->Value() );
	return true;
      }
      if ( buddy ) {
	if ( c->IsDescendantOf( buddy ) ) {
	  IDBG << "BUDDY ACTIVATION FOR " << c->Value() << endl;
	  Activate( *c->Value() );
	  return true;
	}
	EDBG << "DROP BUDDY on " << c->Value() << endl;
	buddy = 0;
      }
      break;
    case NC::WSdumb:
      if ( c->Value()->HasHotkey( key )
	   || c->Value()->HasFunctionHotkey( key ) )
      {
	SDBG << "DUMB HOT KEY " << key << " in " << c->Value() << endl;
	buddy = c->Value();
      }
    default:
      break;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getinput
//	METHOD TYPE : wint_t
//
//	DESCRIPTION :
//
wint_t NCDialog::getinput()
{
    wint_t got = WEOF;

    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	wint_t gotwch = WEOF;
	int ret = ::get_wch( &gotwch );	// get a wide character

	if ( ret != ERR )	// get_wch() returns OK or KEY_CODE_YES on success
	{
	    got = gotwch;
	}
	else
	{
	    got = WEOF;
	}
    }
    else
    {
	wstring to;
	int gotch = ::getch();	// get the character in terminal encoding

	if ( gotch != -1 )
	{
	    if ( (KEY_MIN > gotch || KEY_MAX < gotch)
		  &&
		 isprint( gotch ) )
	    {
		string str;
		str += static_cast<char>(gotch);
		// recode printable chars
		NCstring::RecodeToWchar( str, NCstring::terminalEncoding(), &to ); 
		got = to[0];

		NCDBG << "Recode: " << str << " (encoding: " << NCstring::terminalEncoding() << ") "
		      << "to wint_t: " << got << endl; 
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getch
//	METHOD TYPE : wint_t
//
//	DESCRIPTION :
//
wint_t NCDialog::getch( int timeout_millisec )
{
  wint_t got = WEOF;

  if ( timeout_millisec < 0 ) {
    // wait for input
    ::nodelay( ::stdscr, false );

    got = getinput();
    
  } else if ( timeout_millisec ) {
    // max halfdelay is 25 seconds (250 tenths of seconds)
    do {
      if ( timeout_millisec > 25000 ) {
	::halfdelay( 250 );
	timeout_millisec -= 25000;
      } else {
	if ( timeout_millisec < 100 ) {
	  // min halfdelay is 1/10 second (100 milliseconds)
	  ::halfdelay( 1 );
	} else
	  ::halfdelay( timeout_millisec/100 );
	timeout_millisec = 0;
      }
      got = getinput();
    } while( got == WEOF && timeout_millisec > 0 );
    ::cbreak(); // stop halfdelay
  } else {
    // no wait
    ::nodelay( ::stdscr, true );
    got = getinput();
  }

  if ( got == KEY_RESIZE ) {
    NCurses::ResizeEvent();
    int i = 100;
    // after resize sometimes WEOF is returned -> skip this in no timeout mode 
    do {
	got =  NCDialog::getch( timeout_millisec );
    } while ( timeout_millisec < 0 && got == WEOF && --i );
  }

  return got;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::idleInput
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCDialog::idleInput()
{
  if ( !pan ) {
    NCINT << DLOC << " called for uninitialized " << this << endl;
    ::flushinp();
    return;
  }

  if ( !active ) {
    ::flushinp();
    doUpdate();
  } else {
    IODBG << "idle+ " << this << endl;
    processInput( 0 );
    IODBG << "idle- " << this << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::pollInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::pollInput()
{
  IODBG << "poll+ " << this << endl;
  if ( !pan ) {
    NCINT << DLOC << " called for uninitialized " << this << endl;
    return NCursesEvent::cancel;
  }

  if ( pendingEvent ) {
    if ( active ) {
      activate( false );
      IODBG << this << " deactivate" << endl;
    }
  } else {
    if ( !active ) {
      activate( true );
      IODBG << this << " activate" << endl;
    }
  }

  NCursesEvent returnEvent = pendingEvent;
  pendingEvent = NCursesEvent::none;

  IODBG << "poll- " << this << '(' << returnEvent << ')' << endl;
  return returnEvent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::userInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::userInput( int timeout_millisec )
{
  IODBG << "user+ " << this << endl;
  ::flushinp();

  if ( !pan ) {
    NCINT << DLOC << " called for uninitialized " << this << endl;
    return NCursesEvent::cancel;
  }

  processInput( timeout_millisec );

  NCursesEvent returnEvent = pendingEvent;
  pendingEvent = NCursesEvent::none;

  IODBG << "user- " << this << '(' << returnEvent << ')' << endl;
  return returnEvent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::processInput
//	METHOD TYPE : void
//
//	DESCRIPTION : timeout -1 -> wait for input
//                    timeout  0 -> immediate return
//                    else max wait timeout milliseconds
//
void NCDialog::processInput( int timeout_millisec )
{
  IODBG << "process+ " << this << " active " << wActive
    << " timeout_millisec " << timeout_millisec << endl;

  if ( pendingEvent ) {
    IODBG << this << "(return pending event)" << endl;
    doUpdate();
    ::flushinp();
    return;
  }

  // if no active item return on any input
  if ( wActive->GetState() != NC::WSactive ) {
    IODBG << "noactive item => reactivate!" << endl;
    Activate();
  }

  if ( wActive->GetState() != NC::WSactive ) {
    IODBG << "still noactive item!" << endl;

    if ( timeout_millisec == -1 ) {
      pendingEvent = NCursesEvent::cancel;
      NCINT << DLOC << this << "(set ET_CANCEL since noactive item on pollInput)" << endl;
      getch( -1 );
    }
    else
      ::flushinp();

    return;
  }

  // get and process user input
  wint_t ch = 0;
  wint_t hch = 0;

  IODBG << "enter loop..." << endl;
  noUpdates = true;
  while ( !pendingEvent.isReturnEvent() && ch != WEOF ) {

    switch ( (ch = getch( timeout_millisec )) ) {

      // case KEY_RESIZE: is directly handled in NCDialog::getch.

    case WEOF:
      if ( timeout_millisec == -1 )
	pendingEvent = NCursesEvent::cancel;
      else if ( timeout_millisec > 0 )
        pendingEvent = NCursesEvent::timeout;
      break;
      
    case CTRL('D'):
      hch = getch( -1 );
      ::flushinp();
      switch ( hch ) {
      case 'D':
	NCINT << "CTRL('D')-'D' DUMP+++++++++++++++++++++" << endl;
	NCurses::ScreenShot();
	NCINT << this << endl;
	DumpOn( NCINT, " " );
	NCINT << "CTRL('D')-'D' DUMP---------------------" << endl;
	break;
      case 'S':
	if ( hiddenMenu() ) {
	  NCINT << "CTRL('D')-'S' STYLEDEF+++++++++++++++++++++" << endl;
	  const_cast<NCstyle&>(NCurses::style()).changeSyle();
	  NCurses::Redraw();
	  NCINT << "CTRL('D')-'S' STYLEDEF---------------------" << endl;
	}
	break;
      }
      break;

    case KEY_TAB:
    case CTRL('F'):
      ActivateNext();
      break;

    case KEY_BTAB:
    case CTRL('B'):
      ActivatePrev();
      break;

    case CTRL('L'):
      NCurses::Refresh();
      break;

    case CTRL('A'):
      pendingEvent = getInputEvent( KEY_SLEFT );
      break;

    case CTRL('E'):
      pendingEvent = getInputEvent( KEY_SRIGHT );
      break;
	
    case KEY_ESC:
    case CTRL('X'):
      hch = getch( 0 );
      ::flushinp();
      switch ( hch ) {
      case WEOF: // no 2nd char, handle ch
	if ( helpPopup )
	{
	    helpPopup->popdown();
	}
	else
	{
	    pendingEvent = getInputEvent( ch );
	}
	break;
      case KEY_ESC:
      case CTRL('X'):
	pendingEvent = getInputEvent( hch );
	break;
      default:
	pendingEvent = getHotkeyEvent( hch );
	break;
      }
      break;

     case KEY_F(1):
	 if ( !helpPopup )
	 {
	     string helpText = "";
	     string helpIntro = "";
	     bool hasF1 = describeFunctionKeys( helpText );
	     if ( hasF1 )
	     {
		 // part of help for textmode navigation (shown if there is further help available) 
		 helpIntro = _( "<p>Press <b>F1</b> again to get further help or <b>ESC</b> to close this dialog.</p>" );
	     }
	     else
	     {
                 // part of help for text mode navigation
		 helpIntro =  _( "<p>Press <b>F1</b> or <b>ESC</b> to close this dialog.</p>" );
	     }
		 
	     helpPopup = new NCPopupInfo( wpos(1,1),
					   // headline of the text mode help
					  _( "Text Mode Navigation" ),
					  YCPString( helpIntro +
						     _( "<p>Function key bindings:</p>" ) +
						     helpText
						     ),
					  "" );
	 }
	 if ( helpPopup )
	 {
	     if ( !helpPopup->isVisible() )
	     {
		 helpPopup->popup();
	     }
	     else
	     {
		 helpPopup->popdown();
		 delete helpPopup;
		 helpPopup = 0;
		 pendingEvent = getHotkeyEvent( ch );
	     }
	 }
	 break;

    default:
	// only handle keys if the help popup is not existing or not visible
	if ( !helpPopup
	      || (helpPopup && !helpPopup->isVisible()) )
	{
	    if ( ch >= KEY_F(2) && ch <= KEY_F(24) )
	    {
		pendingEvent = getHotkeyEvent( ch );
	    }
	    else
	    {
		pendingEvent = getInputEvent( ch );
	    }
	}
      break;
    }
    doUpdate();
  }
  noUpdates = false;

#if 0 // handled in get...Event
  switch ( pendingEvent.type ) {
  case NCursesEvent::handled:
  case NCursesEvent::none:
  case NCursesEvent::cancel:
  case NCursesEvent::timeout:
    pendingEvent.widget = 0;
    break;
  case NCursesEvent::button:
  case NCursesEvent::menu:
    pendingEvent.widget = wActive;
    break;
  }
#endif

  IODBG << "process- " << this << " active " << wActive << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getInputEvent
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::getInputEvent( wint_t ch )
{
  NCursesEvent ret = wHandleInput( ch );
  ret.widget = wActive;
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::wHandleInput( wint_t ch )
{
  return wActive->wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getHotkeyEvent
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::getHotkeyEvent( wint_t key )
{
  NCWidget *const oActive = wActive;
  NCursesEvent ret = wHandleHotkey( key );
  ret.widget = wActive;
#if 0 // depends on kind of widget
  if ( wActive != oActive ) {
    Activate( *oActive );
  }
#endif
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::wHandleHotkey( wint_t key )
{
  if ( key >= 0 && ActivateByKey( key ) )
    return wActive->wHandleHotkey( key );

  return NCursesEvent::none;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCDialog * OBJ )
{
  if ( OBJ )
    return STREAM << *OBJ;

  return STREAM << "(NoNCDialog)";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::describeFunctionKeys
//	METHOD TYPE : string
//
//	DESCRIPTION : get all PushButtons and MenuButtons with `opt(`key_Fn)
//		      and create a text like F1: Help, F2: Info and so on
//
bool NCDialog::describeFunctionKeys( string & helpText )
{
    string text = "";
    char key[20];
    YCPString label( "" );
    bool hasF1 = false;
    std::map<int, string> fkeys;

    for ( tnode<NCWidget*> * c = this->Next(); c; c = c->Next() )
    {
	int fkey =  c->Value()->GetFunctionHotkey( );
	if ( fkey != 0 )
	{
	    YWidget * w = dynamic_cast<YWidget *> (c->Value() );

	    if ( w )
	    {
		// Retrieve the widget's "shortcut property" that describes
		// whatever it is - regardless of widget type (PushButton, ...)
		YCPSymbol propertyName( w->shortcutProperty() );
		YCPValue  propertyValue = w->queryWidget( propertyName );

		// Get rid of unwanted '&' shortcut markers
		string desc = YShortcut::cleanShortcutString(  propertyValue->asString()->value() );
		int no =  fkey - KEY_F(1) + 1;
		if ( no == 1 )
		{
		    hasF1 = true;
		}
		fkeys[ no ] = desc;
	    }
	    else
	    {
		NCERR << "Dynamic cast to YWidget * failed for" << c->Value() << endl;
	    }
	}
    }
    // create the text with sorted F-keys
    std::map<int, string>::iterator it;

    for ( it = fkeys.begin(); it != fkeys.end(); ++it )
    {
	sprintf( key, "F%-2d: ", (*it).first );
	text += key + (*it).second + "<br>";
    }

    helpText = text;

    return hasF1;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCDialog & OBJ )
{
  STREAM << (const NCWidget &)OBJ << ' ' << OBJ.pan
    << (OBJ.active ? "{A " : "{i " ) << OBJ.pendingEvent;

  if ( OBJ.pendingEvent )
    STREAM << OBJ.pendingEvent.widget;

  return STREAM << '}';
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getInvisible
//	METHOD TYPE : bool
//
bool NCDialog::getInvisible()
{
  if ( !pan || pan->hidden() )
    return false; // no change in visibility

  // just do it.
  // caller is responsible for screen update.
  pan->hide();
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::getVisible
//	METHOD TYPE : bool
//
bool NCDialog::getVisible()
{
  if ( !pan || !pan->hidden() )
    return false; // no change in visibility

  // just do it.
  // caller is responsible for screen update.
  pan->show();
  if ( hshaddow ) {
    pan->transparent( pan->maxy(), 0 );
  }
  if ( vshaddow ) {
    pan->transparent( 0, pan->maxx() );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::resizeEvent
//	METHOD TYPE : void
//
void NCDialog::resizeEvent()
{
  _init_size();
  if ( pan ) {
    setInitialSize();
  }
}

