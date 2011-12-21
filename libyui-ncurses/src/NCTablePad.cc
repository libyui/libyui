/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

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

   File:       NCTablePad.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCTablePad.h"
#include "NCPopupMenu.h"

#include <limits.h>


NCTablePad::NCTablePad( int lines, int cols, const NCWidget & p )
	: NCPad( lines, cols, p )
	, Headpad( 1, 1 )
	, dirtyHead( false )
	, dirtyFormat( false )
	, ItemStyle( p )
	, Headline( 0 )
	, Items( 0 )
	, citem( 0 )
	, sortStrategy ( new NCTableSortDefault )
{
}



NCTablePad::~NCTablePad()
{
    ClearTable();
}



void NCTablePad::assertLine( unsigned idx )
{
    if ( idx >= Lines() )
	SetLines( idx + 1 );
}



void NCTablePad::SetLines( unsigned idx )
{
    if ( idx == Lines() )
	return;

    unsigned olines = Lines();

    if ( idx < Lines() )
    {
	for ( unsigned i = idx; i < Lines(); ++i )
	{
	    delete Items[i];
	}
    }

    Items.resize( idx, 0 );

    for ( unsigned i = olines; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    DirtyFormat();
}



void NCTablePad::SetLines( vector<NCTableLine*> & nItems )
{
    SetLines( 0 );
    Items = nItems;

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    DirtyFormat();
}



void NCTablePad::AddLine( unsigned idx, NCTableLine * item )
{
    assertLine( idx );
    delete Items[idx];
    Items[idx] = item ? item : new NCTableLine( 0 );

    DirtyFormat();
}



void NCTablePad::DelLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	Items[idx]->ClearLine();
	DirtyFormat();
    }
}



const NCTableLine * NCTablePad::GetLine( unsigned idx ) const
{
    if ( idx < Lines() )
	return Items[idx];

    return 0;
}



NCTableLine * NCTablePad::ModifyLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	DirtyFormat();
	return Items[idx];
    }

    return 0;
}



bool NCTablePad::SetHeadline( const vector<NCstring> & head )
{
    bool hascontent = ItemStyle.SetStyleFrom( head );
    DirtyFormat();
    update();
    return hascontent;
}



void NCTablePad::wRecoded()
{
    DirtyFormat();
    update();
}



wpos NCTablePad::CurPos() const
{
    citem.C = srect.Pos.C;
    return citem;
}



wsze NCTablePad::UpdateFormat()
{
    yuiDebug() << endl;
    dirty = true;
    dirtyFormat = false;
    ItemStyle.ResetToMinCols();

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	Items[l]->UpdateFormat( ItemStyle );
    }

    resize( wsze( Lines(), ItemStyle.TableWidth() ) );

    return wsze( Lines(), ItemStyle.TableWidth() );
}



int NCTablePad::DoRedraw()
{
    if ( !Destwin() )
    {
	dirty = true;
	return OK;
    }

    yuiDebug() << "dirtyFormat " << dirtyFormat << endl;

    if ( dirtyFormat )
	UpdateFormat();

    bkgdset( ItemStyle.getBG() );

    clear();

    wsze lSze( 1, width() );

    if ( ! pageing() )
    {
    for ( unsigned l = 0; l < Lines(); ++l )
    {
	Items[l]->DrawAt( *this, wrect( wpos( l, 0 ), lSze ),
			  ItemStyle, (( unsigned )citem.L == l ) );
    }
    }
    // else: item drawing requested via directDraw

    if ( Headpad.width() != width() )
	Headpad.resize( 1, width() );

    Headpad.clear();

    ItemStyle.Headline().DrawAt( Headpad, wrect( wpos( 0, 0 ), lSze ),
				 ItemStyle, false );

    SendHead();

    dirty = false;

    return update();
}



void NCTablePad::directDraw( NCursesWindow & w, const wrect at, unsigned lineno )
{
    if ( lineno < Lines() )
        Items[lineno]->DrawAt( w, at, ItemStyle, ((unsigned)citem.L == lineno) );
    else
        yuiWarning() << "Illegal Lineno " << lineno << " (" << Lines() << ")" << endl;
}



int NCTablePad::setpos( const wpos & newpos )
{
    if ( !Lines() )
    {
	if ( dirty || dirtyFormat )
	    return DoRedraw();

	return OK;
    }

    yuiDebug() << newpos << " : l " << Lines() << " : cl " << citem.L

    << " : d " << dirty << " : df " << dirtyFormat << endl;

    if ( dirtyFormat )
	UpdateFormat();

    // save old values
    int oitem = citem.L;

    int opos  = srect.Pos.C;

    // calc new values
    citem.L = newpos.L < 0 ? 0 : newpos.L;

    if (( unsigned )citem.L >= Lines() )
	citem.L = Lines() - 1;

    srect.Pos = wpos( citem.L - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( dirty )
    {
	return DoRedraw();
    }

    if ( ! pageing() )
    {
    // adjust only
    if ( citem.L != oitem )
    {
	Items[oitem]->DrawAt( *this, wrect( wpos( oitem, 0 ), wsze( 1, width() ) ),
			      ItemStyle, false );
    }

    Items[citem.L]->DrawAt( *this, wrect( wpos( citem.L, 0 ), wsze( 1, width() ) ),

			    ItemStyle, true );
    }
    // else: item drawing requested via directDraw

    if ( srect.Pos.C != opos )
	SendHead();

    return update();
}



void NCTablePad::updateScrollHint()
{
    NCPad::updateScrollHint();
}



bool NCTablePad::setItemByKey( int key )
{
    if ( HotCol() >= Cols() )
	return false;

    if ( key < 0 || UCHAR_MAX < key )
	return false;

    unsigned hcol = HotCol();

    unsigned hkey = tolower( key );

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	if ( Items[l]->GetCol( hcol )->hasHotkey()
	     &&
	     ( unsigned )tolower( Items[l]->GetCol( hcol )->hotkey() ) == hkey )
	{
	    ScrlLine( l );
	    return true;
	}
    }

    return false;
}

//
// setOrder() sorts the table according to given column by calling
// the sort startegy. Sorting in reverse order is only done
// if 'do_reverse' is set to 'true'. 
//
void NCTablePad::setOrder( int col, bool do_reverse )
{
    if ( col < 0 )
	return;
    
    if ( sortStrategy->getColumn() == col && do_reverse )
    {
	std::reverse( Items.begin(), Items.end() );
    }
    else
    {
	sortStrategy->setColumn( col );
	sortStrategy->sort( Items.begin(), Items.end(), col );
    }

    dirty = true;
    update();
}



bool NCTablePad::handleInput( wint_t key )
{
    return NCPad::handleInput( key );
}

void NCTablePad::stripHotkeys()
{
    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( Items[i] )
	{
	    Items[i]->stripHotkeys();
	}
    }
}


ostream & operator<<( ostream & STREAM, const NCTablePad & OBJ )
{
    STREAM << "TablePad: lines " << OBJ.Lines() << endl;

    for ( unsigned idx = 0; idx < OBJ.Lines(); ++idx )
    {
	STREAM << idx << " " << *OBJ.GetLine( idx );
    }

    return STREAM;
}

