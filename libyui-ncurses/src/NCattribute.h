/****************************************************************************

  Copyright (c) 2000 - 2012 Novell, Inc.
  All Rights Reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, contact Novell, Inc.

  To contact Novell about this file by physical or electronic mail,
  you may find current contact information at www.novell.com

 ****************************************************************************/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

   File:       NCattribute.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCattribute_h
#define NCattribute_h

#include <iosfwd>
#include <string>
#include <vector>

#include "ncursesw.h"

using std::vector;
using std::string;


class NCattribute
{

    NCattribute & operator=( const NCattribute & );
    NCattribute( const NCattribute & );

public:

    enum NCAttribute
    {
	NCAdebug = 0	// for debugging and testing only
	, NCATitlewin	  // title line
	// WIDGETS
	, NCAWdumb	  // dumb widget
	, NCAWdisabeled   // disabeled widget
	// normal widget
	, NCAWnormal	  // default
	, NCAWlnormal	  // label
	, NCAWfnormal	  // frame
	, NCAWdnormal	  // data
	, NCAWhnormal	  // hint
	, NCAWsnormal	  // scroll hint
	// active widget
	, NCAWactive	  // default
	, NCAWlactive	  // label
	, NCAWfactive	  // frame
	, NCAWdactive	  // data
	, NCAWhactive	  // hint
	, NCAWsactive	  // scroll hint
	// DIALOG FRAME
	, NCADnormal	  // normal
	, NCADactive	  // active
	// COMMON ATTRIBUTES
	, NCACheadline	  // headlines
	, NCACcursor	  // cursor
	// RICHTEXT ATTRIBUTES
	, NCARTred
	, NCARTgreen
	, NCARTblue
	, NCARTyellow
	, NCARTmagenta
	, NCARTcyan
	// LAST ENTRY:
	, NCATTRIBUTE
    };

    enum NCAttrSet
    {
	ATTRDEF = 0
	, ATTRWARN
	, ATTRINFO
	, ATTRPOPUP
	// LAST ENTRY:
	, NCATTRSET
    };

protected:

    NCAttrSet		    defattrset;
    vector<vector<chtype> > attribset;

    virtual void _init();

    NCattribute( const bool init )
	: defattrset( ATTRDEF )
	, attribset(( unsigned )NCATTRSET, vector<chtype>(( unsigned )NCATTRIBUTE, A_NORMAL ) )
    {
	if ( init )
	    _init();
    }

public:

    NCattribute()
	: defattrset( ATTRDEF )
	, attribset(( unsigned )NCATTRSET, vector<chtype>(( unsigned )NCATTRIBUTE, A_NORMAL ) )
    {
	_init();
    }

    virtual ~NCattribute() {}

    chtype GetAttrib( const NCAttribute attr ) const
    {
	return attribset[defattrset][attr];
    }

    chtype GetAttrib( const NCAttribute attr, const NCAttrSet attrset ) const
    {
	return attribset[attrset][attr];
    }
};



class NCattrcolor : public NCattribute
{

protected:

    virtual void _init();

    bool scanFile( vector<chtype> & attribs );
    void scanLine( vector<chtype> & attribs, const string & line );
    void defInitSet( vector<chtype> & attribs, short f, short b );

public:

    NCattrcolor()
	    : NCattribute( false )
    {
	_init();
    }

    virtual ~NCattrcolor() {}
};


#endif // NCattribute_h
