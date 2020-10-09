/*
  Copyright (C) 2020 SUSE LLC
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

   File:       NCTablePadBase.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTablePadBase.h"

using std::vector;


NCTablePadBase::NCTablePadBase( int lines, int cols, const NCWidget & p )
    : NCPad( lines, cols, p )
    , _items( 0 )
    , _headpad( 1, 1 )
    , _dirtyHead( false )
    , _dirtyFormat( false )
    , _itemStyle( p )
    , _citem( 0 )
{
}


NCTablePadBase::~NCTablePadBase()
{
    ClearTable();
}


void NCTablePadBase::ClearTable()
{
    for ( unsigned i = 0; i < Lines(); ++i )
        delete _items[i];

    _items.clear();
    _visibleItems.clear();
    setFormatDirty();
}


NCTableLine * NCTablePadBase::getLineWithIndex( unsigned idx ) const
{
    NCTableLine * line = 0;

    if ( idx < Lines() )
	line = _items[ idx ];

    if ( (unsigned) line->index() == idx )
        return line;

    int pos = findIndex( idx );

    if ( pos >= 0 )
    {
        yuiWarning() << "Found item with index " << idx << " at pos " << pos << endl;

        return _items[ pos ];
    }

    yuiError() << "Can't find item with index " << idx << endl;

    return 0;
}


const NCTableLine * NCTablePadBase::GetLine( unsigned idx ) const
{
    return getLineWithIndex( idx );
}


NCTableLine * NCTablePadBase::ModifyLine( unsigned idx )
{
    setFormatDirty();
    return getLineWithIndex( idx );
}


int NCTablePadBase::findIndex( unsigned idx ) const
{
    for ( unsigned i=0; i < Lines(); i++ )
    {
        if ( (unsigned) _items[ i ]->index() == idx )
            return i;
    }

    return -1;
}


void NCTablePadBase::SetLines( unsigned idx )
{
    if ( idx == Lines() )
	return;

    unsigned olines = Lines();

    if ( idx < Lines() )
    {
	for ( unsigned i = idx; i < Lines(); ++i )
	{
	    delete _items[i];
	}
    }

    _items.resize( idx, 0 );

    for ( unsigned i = olines; i < Lines(); ++i )
    {
	if ( !_items[i] )
	    _items[i] = new NCTableLine( 0 );
    }

    setFormatDirty();
}


void NCTablePadBase::SetLines( std::vector<NCTableLine*> & newItems )
{
    SetLines( 0 );
    _items = newItems;

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( !_items[i] )
	    _items[i] = new NCTableLine( 0 );
    }

    setFormatDirty();
}


void NCTablePadBase::AddLine( unsigned idx, NCTableLine * item )
{
    assertLine( idx );
    delete _items[idx];
    _items[idx] = item ? item : new NCTableLine( 0 );

    setFormatDirty();
}


void NCTablePadBase::assertLine( unsigned idx )
{
    if ( idx >= Lines() )
	SetLines( idx + 1 );
}


bool NCTablePadBase::SetHeadline( const vector<NCstring> & head )
{
    bool hascontent = _itemStyle.SetStyleFrom( head );
    setFormatDirty();
    update();

    return hascontent;
}


void NCTablePadBase::wRecoded()
{
    setFormatDirty();
    update();
}


wpos NCTablePadBase::CurPos() const
{
    _citem.C = srect.Pos.C;
    return _citem;
}


wsze NCTablePadBase::tableSize()
{
    if ( _dirtyFormat )
        UpdateFormat();

    return wsze( Lines(), _itemStyle.TableWidth() );
}


wsze NCTablePadBase::UpdateFormat()
{
    dirty = true;
    _itemStyle.ResetToMinCols();

    for ( unsigned i = 0; i < Lines(); ++i )
	_items[i]->UpdateFormat( _itemStyle );

    _dirtyFormat = false;
    updateVisibleItems();

    maxspos.L = visibleLines() > (unsigned) srect.Sze.H ? visibleLines() - srect.Sze.H : 0;

    wsze size( visibleLines(), _itemStyle.TableWidth() );
    resize( size );

    return size;
}


// Update the internal _visibleItems() vector.
// This does NOT do a screen update of the visible items!

void NCTablePadBase::updateVisibleItems()
{
    _visibleItems.clear();

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( _items[ i ]->isVisible() )
	    _visibleItems.push_back( _items[ i ] );
    }
}


int NCTablePadBase::DoRedraw()
{
    // Notice that this is only a fallback implementation of this method.
    // Derived classes usually overwrite this with subtle changes.
    // See NCTreePad::DoRedraw() or NCTablePad::DoRedraw().

    if ( !Destwin() )
    {
	dirty = true;
	return OK;
    }

    prepareRedraw();
    drawContentLines();
    drawHeader();

    dirty = false;

    return update();
}


void NCTablePadBase::prepareRedraw()
{
    if ( _dirtyFormat )
	UpdateFormat();

    bkgdset( _itemStyle.getBG() );
    clear();
}


void NCTablePadBase::drawContentLines()
{
    wsze lineSize( 1, width() );

    for ( unsigned lineNo = 0; lineNo < visibleLines(); ++lineNo )
    {
	_visibleItems[ lineNo ]->DrawAt( *this,
                                         wrect( wpos( lineNo, 0 ), lineSize ),
                                         _itemStyle,
                                         ( lineNo == (unsigned) currentLineNo() ) );
    }
}


void NCTablePadBase::drawHeader()
{
    wsze lineSize( 1, width() );

    if ( _headpad.width() != width() )
	_headpad.resize( 1, width() );

    _headpad.clear();
    _itemStyle.Headline().DrawAt( _headpad,
                                  wrect( wpos( 0, 0 ), lineSize ),
                                  _itemStyle,
                                  false );
    SendHead();
}


int NCTablePadBase::setpos( const wpos & newpos )
{
    if ( !visibleLines() )
    {
	if ( dirty || _dirtyFormat )
	    return DoRedraw();

	return OK;
    }

    if ( _dirtyFormat )
	UpdateFormat();

    // Save old values
    int oldLineNo = currentLineNo();
    int oldPos    = srect.Pos.C;

    // Calc new values
    setCurrentLineNo( newpos.L < 0 ? 0 : newpos.L );

    // Prevent scrolling out of the visible lines
    if ( (unsigned) currentLineNo() >= visibleLines() )
	setCurrentLineNo( visibleLines() - 1 );

    srect.Pos = wpos( currentLineNo() - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( currentLineNo() != oldLineNo )
    {
	unsigned at  = 0;
	unsigned len = 0;

	if ( currentLineNo() >= 0 && _visibleItems[ currentLineNo() ] )
        {
	    len = _visibleItems[ currentLineNo() ]->Hotspot( at );
        }
	else
        {
	    return ERR;
        }

	if ( len )
	{
	    if ( (int) at < srect.Pos.C )
	    {
		srect.Pos.C = at;
	    }
	    else if ( (int) ( at + len - srect.Pos.C ) > drect.Sze.W )
	    {
		srect.Pos.C = (int) at < maxspos.C ? at : maxspos.C;
	    }
	}
    }

    if ( dirty )
	return DoRedraw();

    if ( ! paging() )
    {
        // adjust only

    if ( currentLineNo() != oldLineNo )
    {
	_visibleItems[ oldLineNo ]->DrawAt( *this,
                                            wrect( wpos( oldLineNo, 0 ),
                                                   wsze( 1, width() ) ),
                                            _itemStyle,
                                            false );
    }

    _visibleItems[ currentLineNo() ]->DrawAt( *this,
                                              wrect( wpos( currentLineNo(), 0 ),
                                                     wsze( 1, width() ) ),
                                              _itemStyle,
                                              true );
    }
    // else
    //   item drawing requested via directDraw()

    if ( srect.Pos.C != oldPos )
	SendHead();

    return update();
}


bool NCTablePadBase::handleInput( wint_t key )
{
    // First, give the current item (the item at the cursor position) a chance
    // to handle item-specific keys. The item handles opening and closing
    // branches and item selection.

    bool handled = currentItemHandleInput( key );

    if ( ! handled )
    {
        switch ( key )
        {
            // At this time, there are no more special keys to handle on this
            // level. This switch is a stub for future extension if any more
            // keys need to be handled.
            //
            // Add 'case KEY_XXX' branches here if there should be any
            // and don't forget to set 'handled' to 'true'.
#if 0
            case KEY_SOMETHING:     // Sample
                doSomething();
                handled = true;
                break;
#endif

            default: // Call parent class input handler

                // The NCPad handles scrolling with the cursor keys, PgDn,
                // PgUp, Home, End.
                handled = NCPad::handleInput( key );
                break;
        }
    }

    return handled;
}


bool NCTablePadBase::currentItemHandleInput( wint_t key )
{
    bool handled = false;
    NCTableLine * currentLine = GetCurrentLine();

    if ( currentLine )
    {
        handled = currentLine->handleInput( key );

        if ( handled )
        {
            UpdateFormat();
            setpos( wpos( currentLineNo(), srect.Pos.C ) );
        }
    }

    return handled;
}


NCTableLine * NCTablePadBase::GetCurrentLine() const
{
    if ( currentLineNo() >= 0 && (unsigned) currentLineNo() < visibleLines() )
	return _visibleItems[ currentLineNo() ];

    return 0;
}


//----------------------------------------------------------------------


std::ostream & operator<<( std::ostream & str, const NCTablePadBase & obj )
{
    str << "TablePadBase: lines " << obj.Lines() << std::endl;

    for ( unsigned idx = 0; idx < obj.Lines(); ++idx )
    {
	str << idx << " " << *obj.GetLine( idx );
    }

    return str;
}
