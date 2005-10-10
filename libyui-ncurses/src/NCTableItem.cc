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

   File:       NCTableItem.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCTableItem.h"
#include "stringutil.h"

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableCol
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableCol::NCTableCol
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTableCol::NCTableCol( const NCstring & l, const STYLE & st )
    : label( l )
    , style( st )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableCol::~NCTableCol
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTableCol::~NCTableCol()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableCol::setBkgd
//	METHOD TYPE : chtype
//
//	DESCRIPTION :
//
chtype NCTableCol::setBkgd( NCursesWindow & w,
			  NCTableStyle & tableStyle,
			  NCTableLine::STATE linestate,
			  STYLE colstyle ) const
{
  chtype bkgdstyle = tableStyle.getBG( linestate, colstyle );
  if ( bkgdstyle != NCTableStyle::currentBG )
    w.bkgdset( bkgdstyle );
  else
    bkgdstyle = w.getbkgd();

  return bkgdstyle;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableCol::DrawAt
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableCol::DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const
{
  chtype bg  = setBkgd( w, tableStyle, linestate, style );
  chtype hbg = tableStyle.hotBG( linestate, colidx );
  if ( hbg == NCTableStyle::currentBG )
    hbg = bg;
  label.drawAt( w, bg, hbg, at, tableStyle.ColAdjust( colidx ) );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCTableCol & OBJ )
{
  return STREAM << OBJ.label;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableLine
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::NCTableLine
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTableLine::NCTableLine( unsigned cols, const unsigned s )
    : Items( cols, (NCTableCol*)0 )
    , state ( s )
    , vstate( S_HIDDEN )
{
}

NCTableLine::NCTableLine( vector<NCTableCol*> & nItems, const unsigned s )
    : Items( nItems )
    , state ( s )
    , vstate( S_HIDDEN )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::~NCTableLine
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTableLine::~NCTableLine()
{
  ClearLine();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::assertCol
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::assertCol( unsigned idx )
{
  if ( idx >= Cols() )
    SetCols( idx + 1 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::SetCols
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::SetCols( unsigned idx )
{
  if ( idx == Cols() )
    return;

  if ( idx < Cols() ) {
    for( unsigned i = idx; i < Cols(); ++i ) {
      delete Items[i];
    }
  }

  Items.resize( idx, 0 );
}

void NCTableLine::stripHotkeys()
{
  for( unsigned i = 0; i < Cols(); ++i ) {
    if ( Items[i] )
	Items[i]->stripHotkey();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::SetCols
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::SetCols( vector<NCTableCol*> & nItems )
{
  SetCols( 0 );
  Items = nItems;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::AddCol
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::AddCol( unsigned idx, NCTableCol * item )
{
  assertCol( idx );
  delete Items[idx];
  Items[idx] = item;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::DelCol
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::DelCol( unsigned idx )
{
  if ( idx < Cols() ) {
    delete Items[idx];
    Items[idx] = 0;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::GetCol
//	METHOD TYPE : NCTableCol *
//
//	DESCRIPTION :
//
NCTableCol * NCTableLine::GetCol( unsigned idx )
{
  if ( idx < Cols() )
    return Items[idx];
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::UpdateFormat
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::UpdateFormat( NCTableStyle & tableStyle )
{
  tableStyle.AssertMinCols( Cols() );
  for ( unsigned c = 0; c < Cols(); ++c ) {
    if ( !Items[c] )
      continue;
    tableStyle.MinColWidht( c, Items[c]->Size().W );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::DrawAt
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::DrawAt( NCursesWindow & w, const wrect at,
			  NCTableStyle & tableStyle,
			  bool active ) const
{
  vstate = S_HIDDEN;
  if ( isVisible() ) {
    if ( isDisabeled() )
      vstate = S_DISABELED;
    else
      vstate = active ? S_ACTIVE : S_NORMAL;
  }

  w.bkgdset( tableStyle.getBG( vstate ) );
  for ( int l = 0; l < at.Sze.H; ++l ) {
    w.move( at.Pos.L + l, at.Pos.C );
    w.clrtoeol();
  }

  DrawItems( w, at, tableStyle, active );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableLine::DrawItems
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableLine::DrawItems( NCursesWindow & w, const wrect at,
			     NCTableStyle & tableStyle,
			     bool active ) const
{
  if ( ! (at.Sze > wsze( 0 )) )
    return;

  wrect    lRect( at );
  unsigned destWidht;

  for ( unsigned c = 0; c < Cols(); ++c ) {

    if ( c && tableStyle.ColSepwidht() ) {
      // draw centered
      destWidht = tableStyle.ColSepwidht()/2;
      if ( destWidht < (unsigned)lRect.Sze.W ) {
	w.bkgdset( tableStyle.getBG( vstate, NCTableCol::SEPARATOR ) );
	w.vline( lRect.Pos.L, lRect.Pos.C + destWidht,
		 lRect.Sze.H, tableStyle.ColSepchar() );
	// skip over
	destWidht = tableStyle.ColSepwidht();
	if ( (unsigned)lRect.Sze.W <= destWidht )
	  break;
	lRect.Pos.C += destWidht;
	lRect.Sze.W -= destWidht;
      }
    }

    destWidht = tableStyle.ColWidht( c );
    wrect cRect( lRect );
    // adjust remaining linespace
    lRect.Pos.C += destWidht;
    lRect.Sze.W -= destWidht;
    // adjust destinated width
    if ( lRect.Sze.W < 0 )
      cRect.Sze.W = destWidht + lRect.Sze.W;
    else
      cRect.Sze.W = destWidht;
    // draw item
    if ( Items[c] ) {
      Items[c]->DrawAt( w, cRect, tableStyle, vstate, c );
    }
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
  */
ostream & operator<<( ostream & STREAM, const NCTableLine & OBJ )
{
  STREAM << "Line: cols " << OBJ.Cols() << endl;
  for ( unsigned idx = 0; idx < OBJ.Cols(); ++idx ) {
    STREAM << "  " << idx << " ";
    const NCTableCol * ci = OBJ.GetCol( idx );
    if ( ci )
      STREAM << *ci;
    else
      STREAM << "NO_ITEM";
    STREAM << endl;
  }
  return STREAM;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableHead
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableHead::DrawAt
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTableHead::DrawAt( NCursesWindow & w, const wrect at,
			  NCTableStyle & tableStyle,
			  bool active ) const
{
  vstate = S_HEADLINE;

  w.bkgdset( tableStyle.getBG( vstate ) );
  for ( int l = 0; l < at.Sze.H; ++l ) {
    w.move( at.Pos.L + l, at.Pos.C );
    w.clrtoeol();
  }

  DrawItems( w, at, tableStyle, active );
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableStyle
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableStyle::NCTableStyle
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTableStyle::NCTableStyle( const NCWidget & p )
    : headline( 0 )
    , colWidht( 0 )
    , colAdjust( 0 )
    , parw( p )
    , colSepwidht( 1 )
    , colSepchar ( ACS_VLINE )
    , hotCol( (unsigned)-1 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableStyle::SetStyleFrom
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTableStyle::SetStyleFrom( const vector<NCstring> & head )
{
  unsigned ncols = head.size();

  headline.ClearLine();
  headline.SetCols( ncols );

  colWidht.clear();
  colAdjust.clear();
  AssertMinCols( ncols );

  bool hascontent = false;
  for ( unsigned i = 0; i < head.size(); ++i ) {
    const wstring & entry( head[i].str() );
    bool strip = false;

    if( entry.length() ) {
      switch ( entry[0] ) {
      case 'R':
	strip = true;
	colAdjust[i] = NC::RIGHT;
	break;
      case 'C':
	strip = true;
	colAdjust[i] = NC::CENTER;
	break;
      case 'L':
	strip = true;
	colAdjust[i] = NC::LEFT;
	break;
      default:
	NCWAR << "No style char [LRC] at beginning of '" << entry << "'" << endl;
	break;
      }
    }

    NCstring coltxt;
    coltxt = strip ? entry.substr( 1 ) : entry;
    headline.AddCol( i, new NCTableCol( coltxt ) );
    if ( !hascontent && coltxt.str().length() )
      hascontent = true;
  }
  return hascontent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableStyle::highlightBG
//	METHOD TYPE : chtype
//
//	DESCRIPTION :
//
chtype NCTableStyle::highlightBG( const NCTableLine::STATE lstate,
				  const NCTableCol::STYLE  cstyle,
				  const NCTableCol::STYLE  dstyle ) const
{
  return getBG( lstate, cstyle );
  // unused:
  if ( lstate == NCTableLine::S_ACTIVE
       &&
       parw.GetState() == NC::WSactive )
    return getBG( lstate, cstyle );

  return getBG( lstate, dstyle );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTableStyle::getBG
//	METHOD TYPE : chtype
//
//	DESCRIPTION :
//
chtype NCTableStyle::getBG( const NCTableLine::STATE lstate,
			    const NCTableCol::STYLE  cstyle ) const
{
  switch( lstate ) {

  case NCTableLine::S_NORMAL:
    switch( cstyle ) {
    case NCTableCol::PLAIN:	return listStyle().item.plain;
    case NCTableCol::DATA:	return listStyle().item.data;
    case NCTableCol::ACTIVEDATA:return listStyle().item.plain;
    case NCTableCol::HINT:	return listStyle().item.hint;
    case NCTableCol::SEPARATOR:	return listStyle().item.plain;
    case NCTableCol::NONE:	return currentBG;
    }
    break;

  case NCTableLine::S_ACTIVE:
    switch( cstyle ) {
    case NCTableCol::PLAIN:	return listStyle().selected.plain;
    case NCTableCol::DATA:	return listStyle().selected.data;
    case NCTableCol::ACTIVEDATA:return listStyle().selected.data;
    case NCTableCol::HINT:	return listStyle().selected.hint;
    case NCTableCol::SEPARATOR:	return listStyle().selected.plain;
    case NCTableCol::NONE:	return currentBG;
    }
    break;

  case NCTableLine::S_DISABELED:
    switch( cstyle ) {
    case NCTableCol::PLAIN:     return parw.wStyle().disabledList.item.plain;
    case NCTableCol::DATA:      return parw.wStyle().disabledList.item.data;
    case NCTableCol::ACTIVEDATA:return parw.wStyle().disabledList.item.plain;
    case NCTableCol::HINT:	return parw.wStyle().disabledList.item.hint;
    case NCTableCol::SEPARATOR:	return listStyle().item.plain;
    case NCTableCol::NONE:	return currentBG;
    }
    break;

  case NCTableLine::S_HEADLINE:
    return listStyle().title;
    break;

  case NCTableLine::S_HIDDEN:
    return currentBG;
    break;

  }
  return currentBG;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCTableStyle & OBJ )
{
  STREAM << form( "cols %d, sep %d (%lx)\n",
		  OBJ.Cols(), OBJ.ColSepwidht(), OBJ.ColSepchar() );
  for( unsigned i = 0; i < OBJ.Cols(); ++i ) {
    STREAM << form( "%2d %d(%3d) ", i, OBJ.ColAdjust( i ), OBJ.ColWidht( i ) );
    if ( OBJ.Headline().GetCol( i ) )
      STREAM << OBJ.Headline().GetCol( i )->Label();
    STREAM << endl;
  }
  return STREAM;
}

