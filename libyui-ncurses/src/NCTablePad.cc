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

   File:       NCTablePad.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCTablePad.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCTablePad_"
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::NCTablePad
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTablePad::NCTablePad( int lines, int cols, const NCWidget & p )
    : NCPad( lines, cols, p )
    , Headpad    ( 1, 1 )
    , dirtyHead  ( false )
    , dirtyFormat( false )
    , ItemStyle  ( p )
    , Headline   ( 0 )
    , Items      ( 0 )
    , citem      ( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::~NCTablePad
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTablePad::~NCTablePad()
{
  ClearTable();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::assertLine
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::assertLine( unsigned idx )
{
  if ( idx >= Lines() )
    SetLines( idx + 1 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::SetLines
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::SetLines( unsigned idx )
{
  if ( idx == Lines() )
    return;

  unsigned olines = Lines();

  if ( idx < Lines() ) {
    for( unsigned i = idx; i < Lines(); ++i ) {
      delete Items[i];
    }
  }
  Items.resize( idx, 0 );
  for( unsigned i = olines; i < Lines(); ++i ) {
    if ( !Items[i] )
      Items[i] = new NCTableLine( 0 );
  }

  DirtyFormat();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::SetLines
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::SetLines( vector<NCTableLine*> & nItems )
{
  SetLines( 0 );
  Items = nItems;
  for( unsigned i = 0; i < Lines(); ++i ) {
    if ( !Items[i] )
      Items[i] = new NCTableLine( 0 );
  }

  DirtyFormat();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::AddLine
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::AddLine( unsigned idx, NCTableLine * item )
{
  assertLine( idx );
  delete Items[idx];
  Items[idx] = item ? item : new NCTableLine( 0 );

  DirtyFormat();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::DelLine
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::DelLine( unsigned idx )
{
  if ( idx < Lines() ) {
    Items[idx]->ClearLine();
    DirtyFormat();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::GetLine
//	METHOD TYPE : const NCTableLine *
//
//	DESCRIPTION :
//
const NCTableLine * NCTablePad::GetLine( unsigned idx ) const
{
  if ( idx < Lines() )
    return Items[idx];
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::ModifyLine
//	METHOD TYPE : NCTableLine *
//
//	DESCRIPTION :
//
NCTableLine * NCTablePad::ModifyLine( unsigned idx )
{
  if ( idx < Lines() ) {
    DirtyFormat();
    return Items[idx];
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::SetHeadline
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTablePad::SetHeadline( const vector<NCstring> & head )
{
  bool hascontent = ItemStyle.SetStyleFrom( head );
  DirtyFormat();
  update();
  return hascontent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::wRecoded()
{
  DirtyFormat();
  update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::CurPos
//	METHOD TYPE : wpos
//
//	DESCRIPTION :
//
wpos NCTablePad::CurPos() const
{
  citem.C = srect.Pos.C;
  return citem;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::UpdateFormat
//	METHOD TYPE : wsze
//
//	DESCRIPTION :
//
wsze NCTablePad::UpdateFormat()
{
  IDBG << endl;
  dirty = true;
  dirtyFormat = false;
  ItemStyle.ResetToMinCols();
  for( unsigned l = 0; l < Lines(); ++l ) {
    Items[l]->UpdateFormat( ItemStyle );
  }

  resize( wsze( Lines(), ItemStyle.TableWidth() ) );
  return wsze( Lines(), ItemStyle.TableWidth() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::DoRedraw
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCTablePad::DoRedraw()
{
  if ( !Destwin() ) {
    dirty = true;
    return OK;
  }

  EDBG << "dirtyFormat " << dirtyFormat << endl;
  if ( dirtyFormat )
    UpdateFormat();

  bkgdset( ItemStyle.getBG() );
  clear();
  wsze lSze( 1, width() );
  for ( unsigned l = 0; l < Lines(); ++l ) {
    Items[l]->DrawAt( *this, wrect( wpos( l, 0 ), lSze ),
		      ItemStyle, ((unsigned)citem.L == l) );
  }

  if ( Headpad.width() != width() )
    Headpad.resize( 1, width() );
  Headpad.clear();
  ItemStyle.Headline().DrawAt( Headpad, wrect( wpos( 0, 0 ), lSze ),
			       ItemStyle, false );
  SendHead();

  dirty = false;
  return update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::setpos
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCTablePad::setpos( const wpos & newpos )
{
  if ( !Lines() ) {
    if ( dirty || dirtyFormat )
      return DoRedraw();
    return OK;
  }
  SDBG << newpos << " : l " << Lines() << " : cl " << citem.L
    << " : d " << dirty << " : df " << dirtyFormat << endl;
  if ( dirtyFormat )
    UpdateFormat();

  // save old values
  int oitem = citem.L;
  int opos  = srect.Pos.C;

  // calc new values
  citem.L = newpos.L < 0 ? 0 : newpos.L;
  if ( (unsigned)citem.L >= Lines() )
    citem.L = Lines() - 1;

  srect.Pos = wpos( citem.L - (drect.Sze.H-1)/2, newpos.C ).between( 0, maxspos );

  if ( dirty ) {
    return DoRedraw();
  }

  // adjust only
  if ( citem.L != oitem ) {
    Items[oitem]->DrawAt( *this, wrect( wpos( oitem, 0 ), wsze( 1, width() ) ),
			  ItemStyle, false );
  }

  Items[citem.L]->DrawAt( *this, wrect( wpos( citem.L, 0 ), wsze( 1, width() ) ),
			  ItemStyle, true );

  if ( srect.Pos.C != opos )
    SendHead();

  return update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::updateScrollHint
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTablePad::updateScrollHint()
{
  NCPad::updateScrollHint();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::setItemByKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTablePad::setItemByKey( int key )
{
  if ( HotCol() >= Cols() )
    return false;
  if ( key < 0 || UCHAR_MAX < key )
    return false;

  unsigned hcol = HotCol();
  unsigned hkey = tolower( key );

  for ( unsigned l = 0; l < Lines(); ++l ) {
    if ( Items[l]->GetCol( hcol )->hasHotkey()
	 &&
	 (unsigned)tolower( Items[l]->GetCol( hcol )->hotkey() ) == hkey ) {
      ScrlLine( l );
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTablePad::handleInput
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTablePad::handleInput( int key )
{
  return NCPad::handleInput( key );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCTablePad & OBJ )
{
  STREAM << "TablePad: lines " << OBJ.Lines() << endl;
  for ( unsigned idx = 0; idx < OBJ.Lines(); ++idx ) {
    STREAM << idx << " " << *OBJ.GetLine( idx );
  }
  return STREAM;
}

