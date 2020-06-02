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


#ifndef NCWordWrapper_h
#define NCWordWrapper_h

#include <string>

/**
 * Helper class to word-wrap text into a specified maximum line width.
 * Whitespace is normalized in the process, i.e. any sequence of whitespace
 * (blanks, newlines, tabs, ...) is replaced by a single blank. All lines end
 * with a single newline character except the last one which has no newline.
 **/
class NCWordWrapper
{
public:

    /**
     * Constructor.
     **/
    NCWordWrapper();

    /**
     * Set the original text to wrap.
     **/
    void setText( const std::wstring & origText );

    /**
     * Set the maximum line width to wrap into.
     **/
    void setLineWidth( int width );

    /**
     * Return the number of lines after wrapping the original text.
     **/
    int lines();

    /**
     * Wrap the original text and return the wrapped text.
     **/
    const std::wstring & wrappedText();

    /**
     * Return the original unwrapped text.
     **/
    const std::wstring & origText() const { return _origText; }

    /**
     * Return the last used maximum line width.
     **/
    int lineWidth() const { return _lineWidth; }

    /**
     * Return a string where any sequence of whitespace in the original text is
     * replaced with a single blank and without leading or trailing whitespace.
     **/
    static std::wstring normalizeWhitespace( const std::wstring & orig );

    /**
     * Do the wrapping.
     **/
    void wrap();


protected:

    /**
     * Do the wrapping if necessary.
     **/
    void ensureWrapped();

    /**
     * Return the next line that fits into the line width and removed it from
     * 'unwrapped'.
     **/
    std::wstring nextLine( std::wstring & unwrapped );
    
    //
    // Data members
    //

    std::wstring _origText;
    std::wstring _wrappedText;
    int          _lineWidth;
    int          _lines;
    bool         _dirty;
};

#endif  // NCWordWrapper_h
