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

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTablePadBase.h"


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


const NCTableLine * NCTablePadBase::GetLine( unsigned idx ) const
{
    if ( idx < Lines() )
	return _items[idx];

    return 0;
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


void NCTablePadBase::Append( std::vector<NCTableCol*> & newItems, int index )
{
    AddLine( Lines(), new NCTableLine( newItems, index ) );
}


void NCTablePadBase::DelLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	_items[idx]->ClearLine();
	setFormatDirty();
    }
}


NCTableLine * NCTablePadBase::ModifyLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	setFormatDirty();
	return _items[idx];
    }

    return 0;
}


void NCTablePadBase::assertLine( unsigned idx )
{
    if ( idx >= Lines() )
	SetLines( idx + 1 );
}


bool NCTablePadBase::SetHeadline( const std::vector<NCstring> & head )
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


std::ostream & operator<<( std::ostream & str, const NCTablePadBase & obj )
{
    str << "TablePadBase: lines " << obj.Lines() << std::endl;

    for ( unsigned idx = 0; idx < obj.Lines(); ++idx )
    {
	str << idx << " " << *obj.GetLine( idx );
    }

    return str;
}
