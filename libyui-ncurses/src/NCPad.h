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

   File:       NCPad.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCPad_h
#define NCPad_h

#include <iosfwd>

#include "NCurses.h"
#include "NCWidget.h"


class NCSchrollCB {

  public:
  
    virtual ~NCSchrollCB () {}

    // callback
    virtual void HScroll( unsigned total, unsigned visible, unsigned start ) {}
    virtual void VScroll( unsigned total, unsigned visible, unsigned start ) {}

    virtual void ScrollHead( NCursesWindow & w, unsigned ccol ) {}

    //
    virtual void AdjustPadSize( wsze & minsze ) {}
};


class NCScrollHint : protected NCSchrollCB {

  private:

    NCSchrollCB * redirect;

  protected:

    NCScrollHint() : redirect( this ) {}
    virtual ~NCScrollHint() {}

  protected:

    virtual void SetHead( NCursesWindow & w, unsigned ccol ) {
      redirect->ScrollHead( w, ccol );
    }

    void VSet( unsigned total, unsigned visible, unsigned start ) {
      redirect->VScroll( total, visible, start );
    }

    void HSet( unsigned total, unsigned visible, unsigned start ) {
      redirect->HScroll( total, visible, start );
    }

    virtual void SetPadSize( wsze & minsze ) {
      redirect->AdjustPadSize( minsze );
    }

  public:

    // set redirect
    void SendSchrollCB( NCSchrollCB * to ) { redirect = ( to ? to : this ); }

    virtual void SendHead() {}
};


class NCPad : public NCursesPad, public NCScrollHint {

  protected:

    const NCWidget & parw;

    NCursesWindow * destwin;
    wrect drect;
    wrect srect;
    wpos  maxdpos;
    wpos  maxspos;

    bool  dclear;
    bool  dirty;

  protected:

    virtual int dirtyPad() { dirty = false; return setpos( CurPos() ); }
    virtual int setpos( const wpos & newpos );

    int adjpos( const wpos & offset ) {
      return setpos( CurPos() + offset );
    }

    virtual void updateScrollHint();

  public:

    NCPad( int lines, int cols, const NCWidget & p )
      : NCursesPad( lines, cols )
      , parw( p )
      , destwin ( 0 )
      , maxdpos ( 0 )
      , maxspos ( 0 )
      , dclear  ( false )
      , dirty   ( false )
    {}
    virtual ~NCPad() {}

  public:

    NCursesWindow * Destwin() { return destwin; }
    virtual void Destwin( NCursesWindow * dwin );

    virtual void resize( wsze nsze );
    virtual void wRecoded();
    virtual void setDirty() { dirty = true; }

    int update();
    virtual int setpos() { return setpos( CurPos() ); }

    virtual wpos CurPos() const { return srect.Pos; }

    int ScrlTo( const wpos & newpos ) {
      return setpos( newpos );
    }

    int ScrlLine( const int line ) {
      return setpos( wpos( line, srect.Pos.C ) );
    }

    int ScrlCol( const int col ) {
      return setpos( wpos( srect.Pos.L, col ) );
    }

    int ScrlDown( const int lines = 1 ) {
      return adjpos( wpos( lines, 0 ) );
    }

    int ScrlUp( const int lines = 1 ) {
      return adjpos( wpos( -lines, 0 ) );
    }

    int ScrlRight( const int cols = 1 ) {
      return adjpos( wpos( 0, cols ) );
    }

    int ScrlLeft( const int cols = 1 ) {
      return adjpos( wpos( 0, -cols ) );
    }

    virtual bool handleInput( wint_t key );
};


#endif // NCPad_h
