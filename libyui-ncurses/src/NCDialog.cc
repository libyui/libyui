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
#include "NCPopupInfo.h"
#include "NCMenuButton.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCDialog_"
#endif

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
//	DESCRIPTION :
//
void NCDialog::_init( YWidgetOpt & opt )
{
  defsze.H = NCurses::lines();
  defsze.W = NCurses::cols();
  hshaddow = vshaddow = false;

  if ( isBoxed() )
    defsze -= 2;
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
//	METHOD NAME : NCDialog::~NCDialog
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCDialog::~NCDialog()
{
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
  if ( hasDefaultSize() || !numChildren() )
    return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;

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
    pan->show();
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
      pan->show();
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
  if ( isBoxed() )
    panrect.Sze += 2;

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

  inparent = newrect;
  if ( isBoxed() )
    inparent.Pos += 1;

  if ( panrect.Pos.L + panrect.Sze.H < NCurses::lines() ) {
    SDBG << "panrect.Sze.H " << panrect.Sze.H << " -> " << panrect.Sze.H+1 << endl;
    ++panrect.Sze.H;
    hshaddow = true;
  }
  if ( panrect.Pos.C + panrect.Sze.W < NCurses::cols() ) {
    SDBG << "panrect.Sze.W " << panrect.Sze.W << " -> " << panrect.Sze.W+1 << endl;
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
      pan->box();
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
    }
    if ( vshaddow ) {
      pan->vline( 0, pan->maxx(), pan->height(), ' ' );
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
//	METHOD NAME : NCDialog::getch
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCDialog::getch( int timeout )
{
  int got = -1;

  if ( timeout < 0 ) {
    // wait for input
    ::nodelay( ::stdscr, false );
    got = ::getch();
  } else if ( timeout ) {
    // max halfdelay is 25 seconds (250 tenths of seconds)
    do {
      if ( timeout > 25 ) {
	::halfdelay( 250 );
	timeout -= 25;
      } else {
	::halfdelay( timeout*10 );
	timeout = 0;
      }
      got = ::getch();
    } while( got == -1 && timeout );
    ::cbreak(); // stop halfdelay
  } else {
    // no wait
    ::nodelay( ::stdscr, true );
    got = ::getch();
  }

  // no wait
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
NCursesEvent NCDialog::userInput( int timeout )
{
  IODBG << "user+ " << this << endl;
  ::flushinp();

  if ( !pan ) {
    NCINT << DLOC << " called for uninitialized " << this << endl;
    return NCursesEvent::cancel;
  }

  processInput( timeout );

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
//                    else max wait timout seconds
//
void NCDialog::processInput( int timeout )
{
  IODBG << "process+ " << this << " active " << wActive
    << " timeout " << timeout << endl;

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

    if ( timeout == -1 ) {
      pendingEvent = NCursesEvent::cancel;
      NCINT << DLOC << this << "(set ET_CANCEL since noactive item on pollInput)" << endl;
      getch( -1 );
    }
    else
      ::flushinp();

    return;
  }

  // get and process user input
  int ch = 0;
  int hch = 0;

  IODBG << "enter loop..." << endl;
  noUpdates = true;
  while ( !pendingEvent.isReturnEvent() && ch != -1 ) {

    switch ( (ch = getch( timeout )) ) {

    case -1:
      if ( timeout == -1 )
	pendingEvent = NCursesEvent::cancel;
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

    case KEY_RESIZE:
      NCDBG << "KEY_RESIZE " << NCurses::lines() << 'x' << NCurses::cols() << endl;
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

    case KEY_ESC:
    case CTRL('X'):
      hch = getch( 0 );
      ::flushinp();
      switch ( hch ) {
      case -1: // no 2nd char, handle ch
	if ( helpPopup )
	{
	    helpPopup->popdown();
	}
	else
	{
	  pendingEvent = wHandleInput( ch );
	}
	break;
      case KEY_ESC:
      case CTRL('X'):
	pendingEvent = wHandleInput( hch );
	break;
      default:
	pendingEvent = wHandleHotkey( hch );
	break;
      }
      break;

     case KEY_F(1):
	 if ( !helpPopup )
	 {
	     string helpText = describeFunctionKeys();
	     helpPopup = new NCPopupInfo( wpos(1,1), YCPString( "Text mode navigation" ),
					  YCPString( "Press F1 again to get help<br>" + helpText ),
					  false );
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
		 pendingEvent = wHandleHotkey( ch ); 
	     }
	 }
	 break;
	    
    default:
	if ( ch >= KEY_F(2) && ch <= KEY_F(24) )
	{
	    pendingEvent = wHandleHotkey( ch );
	}
	else
	{
	    pendingEvent = wHandleInput( ch );
	}
      break;
    }
    doUpdate();
  }
  noUpdates = false;

  switch ( pendingEvent.type ) {
  case NCursesEvent::handled:
  case NCursesEvent::none:
  case NCursesEvent::cancel:
    pendingEvent.widget = 0;
    break;
  case NCursesEvent::button:
  case NCursesEvent::menu:
    pendingEvent.widget = wActive;
    break;
  }

  IODBG << "process- " << this << " active " << wActive << endl;
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
string NCDialog::describeFunctionKeys()
{
    string text = "";
    char no[5];
    YCPString label( "" );
    NCPushButton *button = 0;
    NCMenuButton *menuButton = 0;
    
    for ( tnode<NCWidget*> * c = this->Next(); c; c = c->Next() )
    {
	int fkey =  c->Value()->GetFunctionHotkey( );
	if ( fkey != 0 )
	{
	    sprintf( no, "%d", fkey-264 );
	    text = text + "F" +  no + ": ";
	    if ( (button = dynamic_cast<NCPushButton *>(c->Value())) )
	    { 
		label = button->getLabel();
		text = text + label->value() + "<br>";
	    }
	    else if ( (menuButton = dynamic_cast<NCMenuButton *>(c->Value())) )
	    {
		label = menuButton->getLabel();
		text = text + label->value() + "<br>";
	    }
	}
    }

    return text;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::wHandleHotkey( int key )
{
  if ( key >= 0 && ActivateByKey( key ) )
    return wActive->wHandleHotkey( key );

  return NCursesEvent::none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCDialog::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCDialog::wHandleInput( int ch )
{
  return wActive->wHandleInput( ch );
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

