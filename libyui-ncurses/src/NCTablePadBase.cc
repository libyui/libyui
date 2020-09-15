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
	, Items( 0 )
	, Headline( 0 )
	, Headpad( 1, 1 )
	, dirtyHead( false )
	, dirtyFormat( false )
	, ItemStyle( p )
	, citem( 0 )
{
}


NCTablePadBase::~NCTablePadBase()
{
    ClearTable();
}


const NCTableLine * NCTablePadBase::GetLine( unsigned idx ) const
{
    if ( idx < Lines() )
	return Items[idx];

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
	    delete Items[i];
	}
    }

    Items.resize( idx, 0 );

    for ( unsigned i = olines; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    setFormatDirty();
}


void NCTablePadBase::SetLines( std::vector<NCTableLine*> & nItems )
{
    SetLines( 0 );
    Items = nItems;

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    setFormatDirty();
}


void NCTablePadBase::AddLine( unsigned idx, NCTableLine * item )
{
    assertLine( idx );
    delete Items[idx];
    Items[idx] = item ? item : new NCTableLine( 0 );

    setFormatDirty();
}


void NCTablePadBase::DelLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	Items[idx]->ClearLine();
	setFormatDirty();
    }
}


NCTableLine * NCTablePadBase::ModifyLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	setFormatDirty();
	return Items[idx];
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
    bool hascontent = ItemStyle.SetStyleFrom( head );
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
    citem.C = srect.Pos.C;
    return citem;
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
