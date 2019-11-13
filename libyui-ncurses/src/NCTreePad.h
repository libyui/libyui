/*
  Copyright (C) 2000-2012 Novell, Inc
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

   File:       NCTreePad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTreePad_h
#define NCTreePad_h

#include <iosfwd>
#include <vector>

#include "NCTableItem.h"
#include "NCPad.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;


class NCTreePad : public NCPad
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCTreePad & obj );

    NCTreePad & operator=( const NCTreePad & );
    NCTreePad( const NCTreePad & );


    NCursesPad	Headpad;
    bool	dirtyHead;
    bool	dirtyFormat;

    NCTableStyle	 ItemStyle;
    NCTableLine		 Headline;
    std::vector<NCTableLine*> Items;
    std::vector<NCTableLine*> visItems;
    wpos		 citem;

    void assertLine( unsigned idx );

protected:

    void	 DirtyFormat() { dirty = dirtyFormat = true; }

    virtual wsze UpdateFormat();

    virtual int  dirtyPad() { return setpos( CurPos() ); }

    virtual int  setpos( const wpos & newpos );
    virtual int  DoRedraw();
    virtual void updateScrollHint();

public:

    NCTreePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTreePad();

public:

    NCursesWindow * Destwin() { return NCPad::Destwin(); }

    virtual void Destwin( NCursesWindow * dwin );

    virtual void wRecoded();

    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

public:

    bool SetHeadline( const std::vector<NCstring> & head );

    virtual void SendHead()
    {
	SetHead( Headpad, srect.Pos.C );
	dirtyHead = false;
    }

    unsigned Cols()	const { return ItemStyle.Cols(); }

    unsigned Lines()	const { return Items.size(); }

    unsigned visLines() const { return visItems.size(); }

    void     SetLines( unsigned idx );
    void     SetLines( std::vector<NCTableLine*> & nItems );
    void     ClearTable()  { SetLines( 0 ); }

    void Append( NCTableLine * item )		{ AddLine( Lines(), item ); }

    void Append( std::vector<NCTableCol*> & nItems ) { AddLine( Lines(), new NCTableLine( nItems ) ); }

    void AddLine( unsigned idx, NCTableLine * item );
    void DelLine( unsigned idx );

    const NCTableLine * GetCurrentLine() const ;
    const NCTableLine * GetLine( unsigned idx ) const;

    NCTableLine *	ModifyLine( unsigned idx );

    void ShowItem( const NCTableLine * item );
};


#endif // NCTreePad_h
