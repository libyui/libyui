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

   File:       NCWidget.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <climits>

#include "Y2Log.h"
#include "NCWidget.h"
#include "YWidget.h"
#include "YContainerWidget.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCWidget_"
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::NCWidget
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCWidget::NCWidget( NCWidget * myparent )
    : tnode<NCWidget*>( this )
    , grabedBy( 0 )
    , win( 0 )
    , defsze( 11, 45 )
    , framedim( 0, 0 )
    , inparent( -1, -1 )
    , noUpdates( false )
    , wstate( NC::WSnormal )
    , hotlabel( 0 )
    , hotfkey( 0 )
{
  if ( myparent ) {
    ReparentTo( *myparent );
  }
  WIDDBG <<  "CCC " << this << " parent " << myparent << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::~NCWidget
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCWidget::~NCWidget()
{
  WIDDBG << "DD+ " << this << endl;
  wDelete();
  while ( Fchild() )
    Fchild()->Disconnect();
  Disconnect();
  WIDDBG << "DD- " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::PreDisconnect
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::PreDisconnect()
{
  grabRelease( 0 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::PostDisconnect
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::PostDisconnect()
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::PreReparent
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::PreReparent()
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::PostReparent
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::PostReparent()
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::grabFocus
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCWidget::grabFocus()
{
  return Top().Value()->wantFocus( *this );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION : Actualy perform sreen update.
//
void NCWidget::wUpdate( bool forced_br )
{
  if ( !win )
    return;
  if ( noUpdates && !forced_br )
    return;
  NCurses::Update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::Update
//	METHOD TYPE : void
//
//	DESCRIPTION : Redirect Update request to topmost widget
//
void NCWidget::Update()
{
  if ( noUpdates )
    return;
  if ( Parent() ) {
    Parent()->Value()->Update();
  } else {
    wUpdate();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::ParentWin
//	METHOD TYPE : NCursesWindow *
//
//	DESCRIPTION :
//
NCursesWindow * NCWidget::ParentWin()
{
  if ( !Parent() )
    return 0;

  return Parent()->Value()->win;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wMoveChildTo
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wMoveChildTo( NCWidget & child, const wpos & newpos )
{
  WIDDBG << "mc+ " << DLOC << child << " -> " << newpos << " in " << this << endl;
  try {
    child.wMoveTo( newpos );
    Redraw( true );
  }
  catch ( NCursesError & err ) {
    NCINT << DLOC << child << " -> " << newpos << " in " << this << endl;
    NCINT << err << endl;
    ::endwin();
    abort();
  }
  WIDDBG << "mc- " << DLOC << child << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wRelocate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wRelocate( const wrect & newrect )
{
  WIDDBG << "rl+ " << this << " -> " << newrect << endl;
  try {
    if ( win ) {
      wDelete();
    }
    wCreate( newrect );
    if ( win )
      SetState( wstate, true );
    else
      SetState( NC::WSdumb, true );
  }
  catch ( NCursesError & err ) {
    NCINT << *this << endl;
    NCINT << err << endl;
    ::endwin();
    abort();
  }
  WIDDBG << "rl- " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wMoveTo
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wMoveTo( const wpos & newpos )
{
  if ( !win ) {
    WIDDBG << "No win to move: " << this << " -> " << newpos << endl;
    return;
  }

  if ( !Parent() )
    throw NCError( "wMoveTo: got no parent" );

  if ( inparent.Pos != newpos ) {
    WIDDBG << "mv+ " << this << " -> " << newpos << " par " << Parent()->Value() << endl;
    NCWidget & p( *Parent()->Value() );
    p.win->mvsubwin( win,
		     newpos.L + Parent()->Value()->framedim.Pos.L,
		     newpos.C + Parent()->Value()->framedim.Pos.C );
    inparent.Pos = newpos;
    WIDDBG << "mv- " << this << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wCreate( const wrect & newrect )
{
  if ( win )
    throw NCError( "wCreate: already have win" );

  inparent = newrect;

  if ( inparent.Sze == wsze(0,0) ) {
    WIDDBG << "Skip nodim widget: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  NCursesWindow * parw = ParentWin();

  if ( Parent() && !parw ) {
    NCINT << "Can't create widget in nodim parent: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    inparent.Sze = wsze(0,0);
    return;
  }

  WIDDBG << "cw+ " << this << ' ' << inparent << " par " << Parent()->Value() << endl;

  if ( parw ) {
    win = new NCursesWindow( *parw,
			     inparent.Sze.H, inparent.Sze.W,
			     inparent.Pos.L + Parent()->Value()->framedim.Pos.L,
			     inparent.Pos.C + Parent()->Value()->framedim.Pos.C,
			     'r' );
  }
  else
    win = new NCursesWindow( inparent.Sze.H, inparent.Sze.W,
			     inparent.Pos.L, inparent.Pos.C );

  WIDDBG << "cw- " << this << ' ' << inparent << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wDelete()
{
  if ( win ) {
    WIDDBG << "wd+ " << this << endl;
    for ( tnode<NCWidget *> * ch = Fchild(); ch; ch = ch->Nsibling() ) {
      ch->Value()->wDelete();
    }
    win->clear();
    delete win;
    win = 0;
    inparent = wrect( -1, -1 );
    WIDDBG << "wd- " << this << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::ScreenPos
//	METHOD TYPE : wpos
//
//	DESCRIPTION :
//
wpos NCWidget::ScreenPos() const
{
  if ( !win )
    return -1;
  if ( Parent() ) {
    return Parent()->Value()->ScreenPos() + inparent.Pos;
  }
  return wsze( win->begy(), win->begx() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::SetState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::SetState( const NC::WState newstate, const bool force )
{
  if ( newstate != wstate || force ) {
    WIDDBG << DLOC << wstate << " -> " << newstate << endl;
    wstate = newstate;
    if ( win ) {
      win->bkgd( wStyle().getWidget( wstate ).plain );
    }
    Redraw();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::Enable
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCWidget::Enable( const bool do_bv )
{
  WIDDBG << DLOC << this << ' ' << do_bv << ' ' << wstate << endl;

  if ( wstate == NC::WSdumb )
    return false;

  if ( do_bv && wstate == NC::WSdisabeled ) {
    SetState( NC::WSnormal );
  }
  else if ( !do_bv && wstate != NC::WSdisabeled ) {
    if ( wstate == NC::WSactive )
      grabRelease( 0 );
    SetState( NC::WSdisabeled );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::Redraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::Redraw( const bool sub )
{
  if ( !win ) {
    return;
  }
  bool savNoUpdates = noUpdates;
  noUpdates = true;
  if ( sub ) {
    win->clear();
    wRedraw();
    for ( tnode<NCWidget *> * ch = Fchild(); ch; ch = ch->Nsibling() ) {
      ch->Value()->Redraw( sub );
    }
  } else {
    wRedraw();
  }
  noUpdates = savNoUpdates;
  Update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wRedraw()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::Recoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::Recoded()
{
  if ( !win ) {
    return;
  }
  bool savNoUpdates = noUpdates;
  noUpdates = true;
  wRecoded();
  for ( tnode<NCWidget *> * ch = Fchild(); ch; ch = ch->Nsibling() ) {
    ch->Value()->Recoded();
  }
  noUpdates = savNoUpdates;
  Update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::wRecoded()
{
  wRedraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::HasHotkey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCWidget::HasHotkey( int key ) const
{
  if ( key < 0 || UCHAR_MAX < key )
    return false;
  if ( !(hotlabel && hotlabel->hasHotkey()) )
    return false;

  return( tolower( key ) == tolower( hotlabel->hotkey() ) );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCWidget::HasFunctionHotkey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCWidget::HasFunctionHotkey( int key ) const
{
    if ( key < 0 || (hotfkey == 0) )
	return false;

    return( key == hotfkey ) ;
}

int NCWidget::GetFunctionHotkey() const
{
    return hotfkey;
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCWidget::setFunctionHotkey
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::setFunctionHotkey( YWidgetOpt & opt )
{
    if ( opt.key_Fxx.value() > 0 )
    {
	hotfkey = KEY_F( (int) opt.key_Fxx.value() );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCWidget::wHandleHotkey( int /*key*/ )
{
  return wHandleInput( KEY_HOTKEY );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCWidget::wHandleInput( int /*key*/ )
{
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
ostream & operator<<( ostream & STREAM, const NCWidget * OBJ )
{
  if ( OBJ )
    return STREAM << *OBJ;

  return STREAM << "(NoNCWidget)";
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCWidget & OBJ )
{
  return STREAM << OBJ.location() << (void*)&OBJ
    << '(' << OBJ.win
    << ' ' << OBJ.inparent
    << ' ' << OBJ.wstate
    << ')';
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWidget::DumpOn
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWidget::DumpOn( ostream & str, string prfx ) const
{
  str
    //<< prfx << "|" << endl
    << prfx << "+-" << this << endl;
  prfx += ( Nsibling() ? "| " : "  " );

  for ( const tnode<NCWidget *> * ch = Fchild(); ch; ch = ch->Nsibling() ) {
    ch->Value()->DumpOn( str, prfx );
  }
}

