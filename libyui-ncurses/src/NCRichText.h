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

   File:       NCRichText.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCRichText_h
#define NCRichText_h

#include <iosfwd>
#include <stack>

#include <yui/YRichText.h>
#include "NCPadWidget.h"


class NCRichText : public YRichText, public NCPadWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCRichText & OBJ );

    NCRichText & operator=( const NCRichText & );
    NCRichText( const NCRichText & );

    /**
     * Lookup std::map for character entities (e.g. '&gt;'). Initialized
     * and used by entityLookup.
     **/
    static std::map<std::wstring, std::wstring> _charentity;

    /**
     * Lookup and return replacement for a character entity. Expects
     * the leading <code>'&'</code> and trailing <code>';'<.code> to
     * be stripped from <code>val_r</code>. Returns <code>NULL</code>,
     * if the character entity should not be replaced.
     **/
    static const std::wstring entityLookup( const std::wstring & val_r );

    /**
     * Lookup and replace all replacements for a character entity.
     **/
    static const std::wstring filterEntities( const std::wstring & text );

private:

    NCstring text;

    bool plainText;

    unsigned textwidth;
    unsigned cl;
    unsigned cc;
    unsigned cindent;
    bool     atbol;

    bool     preTag;		// <pre> tag

    unsigned Tattr;

    static const unsigned Tfontmask = 0xff00;
    enum TOKEN
    {
	T_UNKNOWN = 0x0000,
	T_IGNORE  = 0x0001,
	T_BR	  = 0x0002,
	T_PAR	  = 0x0004,
	T_LEVEL   = 0x0008,
	T_LI	  = 0x0010,
	T_PLAIN	= 0x0012,
	// font
	T_BOLD	  = 0x0100,
	T_IT	  = 0x0200,
	T_TT	  = 0x0400,
	T_ANC	  = 0x0800,
	T_HEAD	  = 0x1000
    };

private:

    static const unsigned listindent;
    static const std::wstring   listleveltags;

    std::stack<int> liststack;

    void PadChangeLevel( bool down, int tag );
    void PadSetLevel();
    size_t textWidth( std::wstring wstr );

private:

    class Anchor
    {

    public:

	static const unsigned unset = ( unsigned ) - 1;

	unsigned sline;
	unsigned scol;
	unsigned eline;
	unsigned ecol;

	std::wstring target;

	Anchor()
	{
	    sline = scol = eline = ecol = unset;
	}

	Anchor( int sl, int sc )
	{
	    open( sl, sc );
	}

	void open( int sl, int sc )
	{
	    sline = sl;
	    scol  = sc;
	    eline = ecol = unset;
	    target = L"";
	}

	void close( int el, int ec )
	{
	    eline = el;
	    ecol  = ec;
	}

	bool valid()
	{
	    if ( sline == unset || scol == unset
		 || eline == unset || ecol == unset )
		return false;

	    if (( eline == sline && ecol <= scol )
		|| eline < sline )
		return false;

	    return true;
	}

	bool within( unsigned firstvisible, unsigned nextinvisible )
	{
	    return sline < nextinvisible && eline >= firstvisible;
	}

	void draw( NCPad & pad, const chtype attr, int color );
    };

    static bool showLinkTarget;

    Anchor		canchor;
    std::vector<Anchor>	anchors;
    unsigned		armed;

    unsigned vScrollFirstvisible;
    unsigned vScrollNextinvisible;

    void openAnchor( std::wstring args );
    void closeAnchor();

    void arm( unsigned i );
    void disarm() { arm( Anchor::unset ); }

private:

    void PadSetAttr();

    void DrawPlainPad();
    void DrawHTMLPad();

    void PadNL();
    void PadBOL();
    void PadWS( bool tab = false );
    void PadTXT( const wchar_t * sch, const unsigned len );
    void PadPreTXT( const wchar_t * sch, const unsigned len );
    void AdjustPrePad( const wchar_t * sch );
    bool PadTOKEN( const wchar_t * sch, const wchar_t *& ech );

protected:

    virtual const char * location() const { return "NCRichText"; }

    virtual void wRedraw();
    virtual void wRecoded();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

    virtual void HScroll( unsigned total, unsigned visible, unsigned start ) override;
    virtual void VScroll( unsigned total, unsigned visible, unsigned start ) override;

    virtual bool handleInput( wint_t key );

public:

    NCRichText( YWidget * parent, const std::string & text,
		bool plainTextMode = false );
    virtual ~NCRichText();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setValue( const std::string & ntext ) override;

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    virtual std::string vScrollValue() const override;

    virtual void setVScrollValue( const std::string & newValue ) override;

    virtual std::string hScrollValue() const override;

    virtual void setHScrollValue( const std::string & newValue ) override;

};


#endif // NCRichText_h
