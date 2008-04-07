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

/-*/
#include <climits>


#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "tnode.h"
#include "NCWidget.h"
#include "YWidget.h"

//#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCWidget_"
//#endif



NCWidget::NCWidget( YWidget * parent )
    : tnode<NCWidget*>( this )
    , magic( YWIDGET_MAGIC )
    , grabedBy( 0 )
    , win( 0 )
    , defsze( 11, 45 )
    , framedim( 0, 0 )
    , inparent( -1, -1 )
    , noUpdates( false )
    , skipNoDimWin( true )
    , wstate( NC::WSnormal )
    , hotlabel( 0 )
{
  NCWidget * myparent = dynamic_cast<NCWidget *> (parent);
    
  if ( myparent ) {
    ReparentTo( *myparent );
  }
  yuiDebug() <<  "CCC " << this << " parent " << myparent << endl;
}

NCWidget::NCWidget( NCWidget * myparent )
    : tnode<NCWidget*>( this )
    , magic( YWIDGET_MAGIC )
    , grabedBy( 0 )
    , win( 0 )
    , defsze( 11, 45 )
    , framedim( 0, 0 )
    , inparent( -1, -1 )
    , noUpdates( false )
    , skipNoDimWin( true )
    , wstate( NC::WSnormal )
    , hotlabel( 0 )
{
  if ( myparent ) {
    ReparentTo( *myparent );
  }
  yuiDebug() <<  "CCC " << this << " parent " << myparent << endl;
}



NCWidget::~NCWidget()
{
  yuiDebug() << "DD+ " << this << endl;
  wDelete();
  while ( Fchild() )
    Fchild()->Disconnect();
  Disconnect();

  invalidate();

  yuiDebug() << "DD- " << this << endl;
}




void NCWidget::PreDisconnect()
{
  grabRelease( 0 );
}



void NCWidget::PostDisconnect()
{}



void NCWidget::PreReparent()
{}



void NCWidget::PostReparent()
{}



bool NCWidget::grabFocus()
{
  return Top().Value()->wantFocus( *this );
}



// Actualy perform sreen update.
void NCWidget::wUpdate( bool forced_br )
{
  if ( !win )
    return;
  if ( noUpdates && !forced_br )
    return;
  NCurses::Update();
}



// Redirect Update request to topmost widget
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



NCursesWindow * NCWidget::ParentWin()
{
  if ( !Parent() )
  {
    return 0;
  }

  return Parent()->Value()->win;
}



void NCWidget::wMoveChildTo( NCWidget & child, const wpos & newpos )
{
  yuiDebug() << "mc+ " << DLOC << child << " -> " << newpos << " in " << this << endl;
  try {
    child.wMoveTo( newpos );
    Redraw( true );
  }
  catch ( NCursesError & err ) {
    yuiError() << DLOC << child << " -> " << newpos << " in " << this << endl;
    yuiError() << err << endl;
    ::endwin();
    abort();
  }
  yuiDebug() << "mc- " << DLOC << child << endl;
}



void NCWidget::wRelocate( const wrect & newrect )
{
  yuiDebug() << "rl+ " << this << " -> " << newrect << endl;
  try {
    if ( win ) {
      wDelete();
    }
    wCreate( newrect );
//    if ( win )
      SetState( wstate, true );
//    else
//      SetState( NC::WSdumb, true );
  }
  catch ( NCursesError & err ) {
    yuiError() << *this << endl;
    yuiError() << err << endl;
    ::endwin();
    abort();
  }
  yuiDebug() << "rl- " << this << endl;
}



void NCWidget::wMoveTo( const wpos & newpos )
{
  if ( !win ) {
    yuiDebug() << "No win to move: " << this << " -> " << newpos << endl;
    return;
  }

  if ( !Parent() )
    throw NCError( "wMoveTo: got no parent" );

  if ( skipNoDimWin && inparent.Sze.H == 0) {
    yuiDebug() << "Skip widget with zero height: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  if ( skipNoDimWin && inparent.Sze.W == 0) {
    yuiDebug() << "Skip widget with zero width: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  if ( inparent.Pos != newpos ) {
    yuiDebug() << "mv+ " << this << " -> " << newpos << " par " << Parent()->Value() << endl;
    NCWidget & p( *Parent()->Value() );
    p.win->mvsubwin( win,
		     newpos.L + Parent()->Value()->framedim.Pos.L,
		     newpos.C + Parent()->Value()->framedim.Pos.C );
    inparent.Pos = newpos;
    yuiDebug() << "mv- " << this << endl;
  }
}



void NCWidget::wCreate( const wrect & newrect )
{
  if ( win )
    throw NCError( "wCreate: already have win" );

  if ( !Parent() )
    throw NCError( "wCreate: got no parent" );
      
  inparent = newrect;

  if ( skipNoDimWin && inparent.Sze == wsze(0,0) ) {
    yuiDebug() << "Skip nodim widget: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  if ( skipNoDimWin && inparent.Sze.H == 0) {
    yuiDebug() << "Skip widget with zero height: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  if ( skipNoDimWin && inparent.Sze.W == 0) {
    yuiDebug() << "Skip widget with zero width: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    return;
  }

  NCursesWindow * parw = ParentWin();

  if ( Parent() && !parw ) {
    yuiError() << "Can't create widget in nodim parent: " << this << ' ' << inparent << " par " << Parent()->Value() << endl;
    inparent.Sze = wsze(0,0);
    return;
  }

  yuiDebug() << "cw+ " << this << ' ' << inparent << " par " << Parent()->Value() << endl;

  if ( parw ) {
    try {
      win = new NCursesWindow( *parw,
			       inparent.Sze.H, inparent.Sze.W,
			       inparent.Pos.L + Parent()->Value()->framedim.Pos.L,
			       inparent.Pos.C + Parent()->Value()->framedim.Pos.C,
			       'r' );
    }
    catch ( ... ) {
      try {
	win = new NCursesWindow( *parw,
				 inparent.Sze.H, inparent.Sze.W,
				 inparent.Pos.L,
				 inparent.Pos.C,
				 'r' );
      }
      catch ( ... ) {
	inparent.Sze = wsze(1,1);
	inparent.Pos = wpos(0,0);
	win = new NCursesWindow( *parw, 1, 1, 0, 0, 'r' );
      }
    }
  }
  else
  {
      win = new NCursesWindow( inparent.Sze.H, inparent.Sze.W,
			     inparent.Pos.L, inparent.Pos.C );
  }

  yuiDebug() << "cw- " << this << ' ' << inparent << endl;
}



void NCWidget::wDelete()
{
  if ( win ) {
    yuiDebug() << "wd+ " << this << endl;
    for ( tnode<NCWidget *> * ch = Fchild(); ch; ch = ch->Nsibling() ) {
      ch->Value()->wDelete();
    }
    win->clear();
    delete win;
    win = 0;
    inparent = wrect( -1, -1 );
    yuiDebug() << "wd- " << this << endl;
  }
}



wpos NCWidget::ScreenPos() const
{
  if ( !win )
    return -1;
  if ( Parent() ) {
    return Parent()->Value()->ScreenPos() + inparent.Pos;
  }
  return wsze( win->begy(), win->begx() );
}



void NCWidget::SetState( const NC::WState newstate, const bool force )
{
  if ( newstate != wstate || force ) {
    yuiDebug() << DLOC << wstate << " -> " << newstate << endl;
    wstate = newstate;
    if ( win ) {
      win->bkgd( wStyle().getWidget( wstate ).plain );
    }
    Redraw();
  }
}



void NCWidget::setEnabled( bool do_bv )
{
  yuiDebug() << DLOC << this << ' ' << do_bv << ' ' << wstate << endl;

  tnode<NCWidget*> *c = this;

  //If widget has kids ([HV]Boxes, alignments,...), disable all of 
  //them recursively (#256707)
  if (c->HasChildren()) {
    yuiMilestone() <<  this << "setEnabled children recursively" << endl;
    for ( c = this->Next();
	c && c->IsDescendantOf( this );
	c = c->Next() ) {
      if ( c->Value()->GetState() != NC::WSdumb )
        c->Value()->setEnabled( do_bv );
    }
  }

  else {
    if ( wstate == NC::WSdumb )
      return;

    if ( do_bv && wstate == NC::WSdisabeled ) {
      SetState( NC::WSnormal );
    }
    else if ( !do_bv && wstate != NC::WSdisabeled ) {
      if ( wstate == NC::WSactive )
        grabRelease( 0 );
      SetState( NC::WSdisabeled );
    }
  }
}



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



void NCWidget::wRedraw()
{
}



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



void NCWidget::wRecoded()
{
  wRedraw();
}



bool NCWidget::HasHotkey( int key ) const
{
  if ( key < 0 || UCHAR_MAX < key )
    return false;
  if ( !(hotlabel && hotlabel->hasHotkey()) )
    return false;

  return( tolower( key ) == tolower( hotlabel->hotkey() ) );
}



bool NCWidget::HasFunctionHotkey( int key ) const
{
    const YWidget * w = dynamic_cast<const YWidget *>( this );

    if ( w )
    {
	if ( key < 0 || ( ! w->hasFunctionKey() ) )
	    return false;

	return( key == KEY_F( w->functionKey() ) );
    }
    else
    {
	yuiError() << "No YWidget" << endl;
	return false;
    }
}



NCursesEvent NCWidget::wHandleHotkey( wint_t /*key*/ )
{
  return wHandleInput( KEY_HOTKEY );
}



NCursesEvent NCWidget::wHandleInput( wint_t /*key*/ )
{
  return NCursesEvent::none;
}


ostream & operator<<( ostream & STREAM, const NCWidget * OBJ )
{
  if ( OBJ && OBJ->isValid() )
    return STREAM << *OBJ;

  return STREAM << "(NoNCWidget)";
}


ostream & operator<<( ostream & STREAM, const NCWidget & OBJ )
{
    if ( OBJ.isValid() )
	return STREAM << OBJ.location() << (void*)&OBJ
		      << '(' << OBJ.win
		      << ' ' << OBJ.inparent
		      << ' ' << OBJ.wstate
		      << ')';
    return STREAM << "( invalid NCWidget)"; 
}



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

