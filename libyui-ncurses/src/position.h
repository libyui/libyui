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

   File:       position.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef position_h
#define position_h

#include <iosfwd>



class wpair {

  friend std::ostream & operator<<( std::ostream & STREAM, const wpair & OBJ );

  protected:

  int A;
  int B;

  public:

    wpair( const int v = 0 )          { A = B = v; }
    wpair( const int a, const int b ) { A = a; B = b; }
    wpair( const wpair & Rhs )        { A = Rhs.A; B = Rhs.B; }

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


    wpair between( const wpair & Min, const wpair & Max ) const {
      return min( max( *this, Min ), Max );
    }

    static wpair min( const wpair & Lhs, const wpair & Rhs ) {
      return wpair( Lhs.A < Rhs.A ? Lhs.A : Rhs.A,
		    Lhs.B < Rhs.B ? Lhs.B : Rhs.B );
    }
    static wpair max( const wpair & Lhs, const wpair & Rhs ) {
      return wpair( Lhs.A > Rhs.A ? Lhs.A : Rhs.A,
		    Lhs.B > Rhs.B ? Lhs.B : Rhs.B );
    }

};



// screen position in (line,col)
class wpos : public wpair {

  public:

    int & L;
    int & C;

    wpos( const int v = 0 )          : wpair( v ),    L( A ), C( B ) {}
    wpos( const int l, const int c ) : wpair( l, c ), L( A ), C( B ) {}
    wpos( const wpair & Rhs )        : wpair( Rhs ),  L( A ), C( B ) {}
    wpos( const wpos & Rhs )         : wpair( Rhs ),  L( A ), C( B ) {}

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

extern std::ostream & operator<<( std::ostream & STREAM, const wpos & OBJ );



// screen dimension in (height,width)
class wsze : public wpair {

  public:

    int & H;
    int & W;

    wsze( const int v = 0 )          : wpair( v ),    H( A ), W( B ) {}
    wsze( const int h, const int w ) : wpair( h, w ), H( A ), W( B ) {}
    wsze( const wpair & Rhs )        : wpair( Rhs ),  H( A ), W( B ) {}
    wsze( const wsze & Rhs )         : wpair( Rhs ),  H( A ), W( B ) {}

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

extern std::ostream & operator<<( std::ostream & STREAM, const wsze & OBJ );



// rectangle {wpos,wsze}
class wrect {

  public:

    wpos Pos;
    wsze Sze;

    wrect() : Pos( 0 ), Sze( 0 ) {}
    wrect( const wpos & pos, const wsze & sze ) : Pos( pos ), Sze( sze ) {}

    virtual ~wrect() {}

  public:

    bool operator==( const wrect & Rhs ) const {
      return Pos == Rhs.Pos && Sze == Rhs.Sze;
    }
    bool operator!=( const wrect & Rhs ) const { return !operator==( Rhs ); }

    wrect inside() const {
      wpos incpos( 1 );
      wsze decsze( 2 );
      if ( Sze.H < 2 )
	incpos.L = decsze.H = 0;
      if ( Sze.W < 2 )
	incpos.C = decsze.W = 0;
      return wrect( Pos+incpos, Sze-decsze );
    }

    wrect intersectRelTo( const wrect & par ) const {
      // Pos is relative to parent
      if ( ! (Pos < par.Sze) )
	return wrect(); // UL is right or above par

      wrect ret( *this );
      // expand negative Sze to par limit
      if ( ret.Sze.H < 0 )
	ret.Sze.H = par.Sze.H - ret.Pos.L;
      if ( ret.Sze.W < 0 )
	ret.Sze.W = par.Sze.W - ret.Pos.C;

      if ( ! (ret.Pos+ret.Sze >= 0) )
	return wrect(); // LR is left or below par

      // HERE we know, there's an intersection

      // adjust Pos if it is left or below par
      if ( ret.Pos.L < 0 ) {
	ret.Sze.H += ret.Pos.L;
	ret.Pos.L = 0;
      }
      if ( ret.Pos.C < 0 ) {
	ret.Sze.W += ret.Pos.C;
	ret.Pos.C = 0;
      }
      // adjust Sze
      ret.Sze = wpair::min( ret.Sze, par.Sze-ret.Pos );
      return ret;
    }

};

extern std::ostream & operator<<( std::ostream & STREAM, const wrect & OBJ );


#endif // wpair_h
