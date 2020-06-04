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

   File:       NCWordWrapper.h

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <cwctype>
#include <iostream>
#include "NCWordWrapper.h"


#define DEFAULT_LINE_WIDTH      78

using std::wstring;
using std::endl;
using std::wcout;


NCWordWrapper::NCWordWrapper():
    _lineWidth( DEFAULT_LINE_WIDTH ),
    _lines( 0 ),
    _dirty( false )
{

}


void NCWordWrapper::setText( const wstring & origText )
{
    if ( origText != _origText )
    {
        _origText = origText;
        _dirty = true;
    }
}


void NCWordWrapper::setLineWidth( int width )
{
    if ( width != _lineWidth )
    {
        _lineWidth = width;
        _dirty = true;
    }
}


void NCWordWrapper::clear()
{
    _origText.clear();
    _wrappedText.clear();
    _lineWidth = DEFAULT_LINE_WIDTH;
    _lines = 0;
    _dirty = false;
}


int NCWordWrapper::lines()
{
    ensureWrapped();

    return _lines;
}


const wstring & NCWordWrapper::wrappedText()
{
    ensureWrapped();

    return _wrappedText;
}


void NCWordWrapper::ensureWrapped()
{
    if ( _dirty )
        wrap();

    _dirty = false;
}


wstring NCWordWrapper::normalizeWhitespace( const wstring & orig )
{
    wstring normalized;
    normalized.reserve( orig.size() );
    bool skippingWhitespace = false;

    for ( wchar_t c: orig )
    {
        switch ( c )
        {
	    case L' ':  // Whitespace
	    case L'\t':
	    case L'\n':
	    case L'\v':
	    case L'\r':
            case L'\f':
                // Don't add any whitespace right now: Wait until there is real content.

                if ( ! normalized.empty() ) // Ignore any leading whitspace
                    skippingWhitespace = true;
                break;

            default:    // Non-whitespace

                // Add one blank for any skipped whitespace.
                //
                // This will not add trailing whitespace which is exactly the
                // desired behaviour.

                if ( skippingWhitespace )
                    normalized += ' ';

                normalized += c;
                skippingWhitespace = false;
                break;
        }
    }

    return normalized;
}


void NCWordWrapper::wrap()
{
    wstring unwrapped = normalizeWhitespace( _origText );
    _wrappedText.clear();
    _wrappedText.reserve( unwrapped.size() );
    _lines = 0;

    while ( ! unwrapped.empty() )
    {
        wstring line = nextLine( unwrapped );

#ifdef WORD_WRAPPER_TESTER
        wcout << "Line: \"" << line << "\"  length: " << line.size() << endl;
        wcout << "Rest: \"" << unwrapped << "\"\n" << endl;
#endif

        if ( ! _wrappedText.empty() )
            _wrappedText += L'\n';

        _wrappedText += line;
        _lines++;
    }

    _dirty = false;
}


wstring NCWordWrapper::nextLine( wstring & unwrapped )
{
    wstring line;

#ifdef WORD_WRAPPER_TESTER
    wcout << "nextLine( \"" << unwrapped << "\" )" << endl;
#endif

    if ( (int) unwrapped.size() <= _lineWidth )
    {
        // The remaining unwrapped text fits into one line

        line = unwrapped;
        unwrapped.clear();

        return line;
    }


    // Try to wrap at the rightmost possible whitespace

    int pos = _lineWidth; // The whitespace will be removed here

    while ( pos > 0 && unwrapped[ pos ] != L' ' )
        --pos;

    if ( unwrapped[ pos ] == L' ' )
    {
        line = unwrapped.substr( 0, pos );
        unwrapped.erase( 0, pos + 1 );

        return line;
    }


    // Try to wrap at the rightmost possible non-alphanum character

    pos = _lineWidth - 1; // We'll need to keep the separator character

    while ( pos > 0 && iswalnum( unwrapped[ pos ] ) )
        --pos;

    if ( ! iswalnum( unwrapped[ pos ] ) )
    {
#ifdef WORD_WRAPPER_TESTER
        wcout << "iswalnum wrap" << endl;
#endif

        line = unwrapped.substr( 0, pos + 1 );
        unwrapped.erase( 0, pos + 1 );

        return line;
    }


    // Still no chance to break the line? So we'll have to break in mid-word.
    // This is crude and brutal, but in some locales (Chinese, Japanese,
    // Korean) there is very little whitespace, so sometimes we have no other
    // choice.

#ifdef WORD_WRAPPER_TESTER
    wcout << "desperation wrap" << endl;
#endif

    pos = _lineWidth - 1;
    line = unwrapped.substr( 0, pos + 1 );
    unwrapped.erase( 0, pos + 1 );

    return line;
}


// ----------------------------------------------------------------------


// Standalone test frame for this class.
//
// Build with
//
//     g++ -D WORD_WRAPPER_TESTER -o word-wrapper-tester NCWordWrapper.cc
//
// Usage:
//
//    ./word-wrapper-tester "text to wrap" <line-length>
//
// Notice that this does not do any fancy UTF-8 recoding of the command line
// arguments, so non-ASCII characters may be slightly broken. This is expected,
// and for the sake of simplicity, this will not be fixed. This only affects
// this test frame; the tested class can handle UTF-8 characters just fine
// (thus "Lörem üpsum" instead of "Lorem ipsum" in the AutoWrap*.cc libyui
// examples).

#ifdef WORD_WRAPPER_TESTER


int main( int argc, char *argv[] )
{
    NCWordWrapper wrapper;

    if ( argc != 3 )
    {
        std::cerr << "\nUsage: " << argv[0] << " \"text to wrap\" <line-length>\n" << endl;
        exit( 1 );
    }

    std::string src( argv[1] );
    wstring input( src.begin(), src.end() );
    int lineWidth = atoi( argv[2] );

    wcout << "Wrapping to " << lineWidth << " columns:\n\"" << input << "\"\n" << endl;

    wrapper.setText( input );
    wrapper.setLineWidth( lineWidth );
    wrapper.wrap();

    wcout << "         10        20        30        40        50" << endl;
    wcout << "12345678901234567890123456789012345678901234567890"  << endl;
    wcout << wrapper.wrappedText() << endl;
    wcout << "-- Wrapped lines: " << wrapper.lines() << endl;
}

#endif
