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

   File:       NCtext.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCtext_h
#define NCtext_h

#include <iosfwd>
#include <list>

#include "NCstring.h"
#include "NCWidget.h"

class NCursesWindow;

/// Multi-line string
class NCtext
{

    friend std::ostream & operator<<( std::ostream & str, const NCtext & obj );

public:

    typedef std::list<NCstring>::iterator	   iterator;
    typedef std::list<NCstring>::const_iterator const_iterator;

private:

    static const NCstring emptyStr;

protected:

    std::list<NCstring> mtext;

    virtual void lset( const NCstring & ntext );
    void lbrset( const NCstring & ntext, size_t columns );

public:

    NCtext( const NCstring & nstr = "" );
    NCtext( const NCstring & nstr, size_t columns );

    virtual ~NCtext();

    unsigned Lines()   const;
    size_t Columns() const;

    void append( const NCstring & line );

    const std::list<NCstring> & Text() const { return mtext; }

    const NCstring &	   operator[]( std::wstring::size_type idx ) const;

    const_iterator begin() const { return mtext.begin(); }

    const_iterator end()   const { return mtext.end(); }
};


/// Multi-line string, with optional hotkey, drawable
class NClabel : protected NCtext
{

    friend std::ostream & operator<<( std::ostream & str, const NClabel & obj );

protected:

    std::wstring::size_type hotline;

    virtual void lset( const NCstring & ntext )
    {
	NCtext::lset( ntext );
    }

public:

    void stripHotkey();

    NClabel( const NCstring & nstr = "" )
	    : NCtext( nstr )
    {}

    virtual ~NClabel() {}

    size_t   width()  const { return Columns(); }

    unsigned height() const { return Lines(); }

    wsze     size()   const { return wsze( Lines(), Columns() ); }

    const std::list<NCstring> & getText() const { return Text(); }

    void drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		 const wrect & dim,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const;

    void drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, style, hotstyle, wrect( 0, -1 ), adjust, fillup );
    }

    void drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		 const wpos & pos,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, style, hotstyle, wrect( pos, -1 ), adjust, fillup );
    }

    void drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		 const wpos & pos, const wsze & sze,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, style, hotstyle, wrect( pos, sze ), adjust, fillup );
    }

    //
    void drawAt( NCursesWindow & w, const NCstyle::StItem & istyle,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, istyle.label, istyle.hint, wrect( 0, -1 ), adjust, fillup );
    }

    void drawAt( NCursesWindow & w, const NCstyle::StItem & istyle,
		 const wpos & pos,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, istyle.label, istyle.hint, wrect( pos, -1 ), adjust, fillup );
    }

    void drawAt( NCursesWindow & w, const NCstyle::StItem & istyle,
		 const wpos & pos, const wsze & sze,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, istyle.label, istyle.hint, wrect( pos, sze ), adjust, fillup );
    }

    void drawAt( NCursesWindow & w, const NCstyle::StItem & istyle,
		 const wrect & dim,
		 const NC::ADJUST adjust = NC::TOPLEFT,
		 bool fillup = true ) const
    {
	drawAt( w, istyle.label, istyle.hint, dim, adjust, fillup );
    }

    //


    bool	hasHotkey() const { return hotline != std::wstring::npos; }

    wchar_t	hotkey() const { return hasHotkey() ? operator[]( hotline ).hotkey() : L'\0'; }

    std::wstring::size_type   hotpos() const { return hasHotkey() ? operator[]( hotline ).hotpos() : std::wstring::npos; }
};


#endif // NCtext_h
