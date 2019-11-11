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

   File:       position.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef position_h
#define position_h

#include <iosfwd>


class wpair
{

    friend std::ostream & operator<<( std::ostream & str, const wpair & obj );

protected:

    int A;
    int B;

public:

    wpair( int v = 0 )	      { A = B = v; }

    wpair( int a, int b ) { A = a; B = b; }

    wpair( const wpair & Rhs )	      { A = Rhs.A; B = Rhs.B; }

    virtual ~wpair() {}

protected:

    wpair & operator= ( const wpair & Rhs ) { A =  Rhs.A; B =  Rhs.B; return *this; }

    wpair & operator+=( const wpair & Rhs ) { A += Rhs.A; B += Rhs.B; return *this; }

    wpair & operator-=( const wpair & Rhs ) { A -= Rhs.A; B -= Rhs.B; return *this; }

    wpair & operator*=( const wpair & Rhs ) { A *= Rhs.A; B *= Rhs.B; return *this; }

    wpair & operator/=( const wpair & Rhs ) { A /= Rhs.A; B /= Rhs.B; return *this; }

    wpair operator+( const wpair & Rhs ) const { return wpair( A + Rhs.A, B + Rhs.B ); }

    wpair operator-( const wpair & Rhs ) const { return wpair( A - Rhs.A, B - Rhs.B ); }

    wpair operator*( const wpair & Rhs ) const { return wpair( A * Rhs.A, B * Rhs.B ); }

    wpair operator/( const wpair & Rhs ) const { return wpair( A / Rhs.A, B / Rhs.B ); }

public:

    bool operator==( const wpair & Rhs ) const { return A == Rhs.A && B == Rhs.B; }

    bool operator!=( const wpair & Rhs ) const { return A != Rhs.A || B != Rhs.B; }

    bool operator> ( const wpair & Rhs ) const { return A >  Rhs.A && B >  Rhs.B; }

    bool operator< ( const wpair & Rhs ) const { return A <  Rhs.A && B <  Rhs.B; }

    bool operator>=( const wpair & Rhs ) const { return A >= Rhs.A && B >= Rhs.B; }

    bool operator<=( const wpair & Rhs ) const { return A <= Rhs.A && B <= Rhs.B; }


    wpair between( const wpair & Min, const wpair & Max ) const
	{
	    return min( max( *this, Min ), Max );
	}

    static wpair min( const wpair & Lhs, const wpair & Rhs )
	{
	    return wpair( Lhs.A < Rhs.A ? Lhs.A : Rhs.A,
			  Lhs.B < Rhs.B ? Lhs.B : Rhs.B );
	}

    static wpair max( const wpair & Lhs, const wpair & Rhs )
	{
	    return wpair( Lhs.A > Rhs.A ? Lhs.A : Rhs.A,
			  Lhs.B > Rhs.B ? Lhs.B : Rhs.B );
	}

};



// screen position in (line,col)

class wpos : public wpair
{

public:

    int & L;
    int & C;

    wpos( int v = 0 )	     : wpair( v ),    L( A ), C( B ) {}

    wpos( int l, int c ) : wpair( l, c ), L( A ), C( B ) {}

    wpos( const wpair & Rhs )	     : wpair( Rhs ),  L( A ), C( B ) {}

    wpos( const wpos & Rhs )	     : wpair( Rhs ),  L( A ), C( B ) {}

    virtual ~wpos() {}

public:

    wpos & operator= ( const wpos & Rhs )  { wpair::operator= ( Rhs ); return *this; }

    wpos & operator+=( const wpair & Rhs ) { wpair::operator+=( Rhs ); return *this; }

    wpos & operator-=( const wpair & Rhs ) { wpair::operator-=( Rhs ); return *this; }

    wpos & operator*=( const wpair & Rhs ) { wpair::operator*=( Rhs ); return *this; }

    wpos & operator/=( const wpair & Rhs ) { wpair::operator/=( Rhs ); return *this; }

    wpos operator+( const wpair & Rhs ) const { return wpair::operator+( Rhs ); }

    wpos operator-( const wpair & Rhs ) const { return wpair::operator-( Rhs ); }

    wpos operator*( const wpair & Rhs ) const { return wpair::operator*( Rhs ); }

    wpos operator/( const wpair & Rhs ) const { return wpair::operator/( Rhs ); }
};

extern std::ostream & operator<<( std::ostream & str, const wpos & obj );



// screen dimension in (height,width)

class wsze : public wpair
{

public:

    int & H;
    int & W;

    wsze( int v = 0 )	     : wpair( v ),    H( A ), W( B ) {}

    wsze( int h, int w ) : wpair( h, w ), H( A ), W( B ) {}

    wsze( const wpair & Rhs )	     : wpair( Rhs ),  H( A ), W( B ) {}

    wsze( const wsze & Rhs )	     : wpair( Rhs ),  H( A ), W( B ) {}

    virtual ~wsze() {}

    wsze & operator= ( const wsze & Rhs )  { wpair::operator= ( Rhs ); return *this; }

    wsze & operator+=( const wpair & Rhs ) { wpair::operator+=( Rhs ); return *this; }

    wsze & operator-=( const wpair & Rhs ) { wpair::operator-=( Rhs ); return *this; }

    wsze & operator*=( const wpair & Rhs ) { wpair::operator*=( Rhs ); return *this; }

    wsze & operator/=( const wpair & Rhs ) { wpair::operator/=( Rhs ); return *this; }

    wsze operator+( const wpair & Rhs ) const { return wpair::operator+( Rhs ); }

    wsze operator-( const wpair & Rhs ) const { return wpair::operator-( Rhs ); }

    wsze operator*( const wpair & Rhs ) const { return wpair::operator*( Rhs ); }

    wsze operator/( const wpair & Rhs ) const { return wpair::operator/( Rhs ); }
};

extern std::ostream & operator<<( std::ostream & str, const wsze & obj );



// rectangle {wpos,wsze}

class wrect
{

public:

    wpos Pos;
    wsze Sze;

    wrect() : Pos( 0 ), Sze( 0 ) {}

    wrect( const wpos & pos, const wsze & sze ) : Pos( pos ), Sze( sze ) {}

    virtual ~wrect() {}

public:

    bool operator==( const wrect & Rhs ) const
    {
	return Pos == Rhs.Pos && Sze == Rhs.Sze;
    }

    bool operator!=( const wrect & Rhs ) const { return !operator==( Rhs ); }


    wrect inside() const
    {
	wpos incpos( 1 );
	wsze decsze( 2 );

	if ( Sze.H < 2 )
	    incpos.L = decsze.H = 0;

	if ( Sze.W < 2 )
	    incpos.C = decsze.W = 0;

	return wrect( Pos + incpos, Sze - decsze );
    }


    wrect intersectRelTo( const wrect & par ) const
    {
	// Pos is relative to parent
	if ( !( Pos < par.Sze ) )
	    return wrect(); // UL is right or above par

	wrect ret( *this );

	// expand negative Sze to par limit
	if ( ret.Sze.H < 0 )
	    ret.Sze.H = par.Sze.H - ret.Pos.L;

	if ( ret.Sze.W < 0 )
	    ret.Sze.W = par.Sze.W - ret.Pos.C;

	if ( !( ret.Pos + ret.Sze >= 0 ) )
	    return wrect(); // LR is left or below par

	// HERE we know, there's an intersection

	// adjust Pos if it is left or below par
	if ( ret.Pos.L < 0 )
	{
	    ret.Sze.H += ret.Pos.L;
	    ret.Pos.L = 0;
	}

	if ( ret.Pos.C < 0 )
	{
	    ret.Sze.W += ret.Pos.C;
	    ret.Pos.C = 0;
	}

	// adjust Sze
	ret.Sze = wpair::min( ret.Sze, par.Sze - ret.Pos );

	return ret;
    }

};

extern std::ostream & operator<<( std::ostream & str, const wrect & obj );


#endif // wpair_h
