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

   File:       NCPad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPad_h
#define NCPad_h

#include <iosfwd>

#include "NCurses.h"
#include "NCWidget.h"


//! Interface for scroll callbacks
class NCSchrollCB
{
public:

    virtual ~NCSchrollCB() {}

    /// @param total    virtual size
    /// @param visible  size of the visible part
    /// @param start    position of the visible part
    virtual void HScroll( unsigned total, unsigned visible, unsigned start ) {}

    /// @param total    virtual size
    /// @param visible  size of the visible part
    /// @param start    position of the visible part
    virtual void VScroll( unsigned total, unsigned visible, unsigned start ) {}

    virtual void ScrollHead( NCursesWindow & w, unsigned ccol ) {}

    virtual void AdjustPadSize( wsze & minsze ) {}
};

/**
 * Forward the scroll callbacks to another object.
 * By default it forwards to itself
 */
class NCScrollHint : protected NCSchrollCB
{
private:

    NCSchrollCB * redirect;

protected:

    NCScrollHint() : redirect( this ) {}

    virtual ~NCScrollHint() {}

protected:

    virtual void SetHead( NCursesWindow & w, unsigned ccol )
    {
	redirect->ScrollHead( w, ccol );
    }

    void VSet( unsigned total, unsigned visible, unsigned start )
    {
	redirect->VScroll( total, visible, start );
    }

    void HSet( unsigned total, unsigned visible, unsigned start )
    {
	redirect->HScroll( total, visible, start );
    }

    virtual void SetPadSize( wsze & minsze )
    {
	redirect->AdjustPadSize( minsze );
    }

public:

    //! Set the receiver of callbacks to *dest*
    void SendSchrollCB( NCSchrollCB * dest ) { redirect = ( dest ? dest : this ); }

    virtual void SendHead() {}
};


class NCPad : public NCursesPad, public NCScrollHint
{
private:

    /** The real height in case the NCursesPad is truncated, otherwise \c 0.
     *
     * \note Don't use _vheight directly, but \ref vheight.
     *
     * Up to ncurses5, ncurses uses \c short for window dimensions (can't hold
     * more than 32768 lines). If \ref resize truncated the window, the real
     * size is in \ref _vheight. Longer lists need to be paged.
     *
     * \todo Once all NCPad based types are able to page, \a maxPadHeight could be
     * std::set to e.g \c 1024 to avoid bigger widgets in memory. Currently just
     * \ref NCTablePad supports paging. If paging is \c ON, all content lines are
     * written via \ref directDraw. Without pageing \ref DoRedraw is reponsible for this.
     */
    int   _vheight;

protected:

    const NCWidget & parw;

    NCursesWindow * destwin;
    wrect drect;
    wrect srect;
    wpos  maxdpos;
    wpos  maxspos;

    bool  dclear;
    bool  dirty;

    /** The (virtual) height of the Pad (even if truncated). */
    int vheight() const        { return _vheight ? _vheight : height(); }

    /** Whether the Pad is truncated (we're pageing). */
    bool pageing() const { return _vheight; }

    virtual int dirtyPad() { dirty = false; return setpos( CurPos() ); }

    virtual int setpos( const wpos & newpos );

    int adjpos( const wpos & offset )
    {
	return setpos( CurPos() + offset );
    }

    virtual void updateScrollHint();

    /** Directly draw a table item at a specific location.
     *
     * \ref update usually copies the visible table content from the
     * \ref NCursesPad to \ref destwin. In case the \ref NCursesPad
     * is truncated, the visible lines are prepared immediately before
     * they are written to \ref destwin
     * .
     * \see \ref _vheight.
     */
    virtual void directDraw( NCursesWindow & w, const wrect at, unsigned lineno ) {}

public:

    NCPad( int lines, int cols, const NCWidget & p );
    virtual ~NCPad() {}

public:

    NCursesWindow * Destwin() { return destwin; }

    virtual void Destwin( NCursesWindow * dwin );

    virtual void resize( wsze nsze );
    virtual int resize( int lines, int columns ) { return NCursesWindow::resize(lines, columns );}
    virtual void wRecoded();
    virtual void setDirty() { dirty = true; }

    int update();
    virtual int setpos() { return setpos( CurPos() ); }

    virtual wpos CurPos() const { return srect.Pos; }

    int ScrlTo( const wpos & newpos )
    {
	return setpos( newpos );
    }

    int ScrlLine( int line )
    {
	return setpos( wpos( line, srect.Pos.C ) );
    }

    int ScrlCol( int col )
    {
	return setpos( wpos( srect.Pos.L, col ) );
    }

    int ScrlDown( int lines = 1 )
    {
	return adjpos( wpos( lines, 0 ) );
    }

    int ScrlUp( int lines = 1 )
    {
	return adjpos( wpos( -lines, 0 ) );
    }

    int ScrlRight( int cols = 1 )
    {
	return adjpos( wpos( 0, cols ) );
    }

    int ScrlLeft( int cols = 1 )
    {
	return adjpos( wpos( 0, -cols ) );
    }

    int ScrlToLastLine()
    {
        return ScrlDown( vheight() );
    }

    virtual bool handleInput( wint_t key );
};


#endif // NCPad_h
