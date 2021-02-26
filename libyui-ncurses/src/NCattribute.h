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

   File:       NCattribute.h

   Author:     Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef NCattribute_h
#define NCattribute_h

#include <iosfwd>

#include <string>
#include <vector>
using namespace std;

#include "ncursesw.h"


class NCattribute {

  NCattribute & operator=( const NCattribute & );
  NCattribute            ( const NCattribute & );

  public:

    enum NCAttribute {
        NCAdebug = 0    // for debugging and testing only
      , NCATitlewin     // title line
	// WIDGETS
      , NCAWdumb        // dumb widget
      , NCAWdisabeled   // disabeled widget
	// normal widget
      , NCAWnormal      // default
      , NCAWlnormal     // label
      , NCAWfnormal     // frame
      , NCAWdnormal     // data
      , NCAWhnormal     // hint
      , NCAWsnormal     // scroll hint
	// active widget
      , NCAWactive      // default
      , NCAWlactive     // label
      , NCAWfactive     // frame
      , NCAWdactive     // data
      , NCAWhactive     // hint
      , NCAWsactive     // scroll hint
	// DIALOG FRAME
      , NCADnormal      // normal
      , NCADactive      // active
	// COMMON ATTRIBUTES
      , NCACheadline    // headlines
      , NCACcursor      // cursor
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

    enum NCAttrSet {
        ATTRDEF = 0
      , ATTRWARN
      , ATTRINFO
      , ATTRPOPUP
	// LAST ENTRY:
      , NCATTRSET
    };

  protected:

    NCAttrSet               defattrset;
    vector<vector<chtype> > attribset;

    virtual void _init();

    NCattribute( const bool init )
      : defattrset( ATTRDEF )
      , attribset( (unsigned)NCATTRSET, vector<chtype>( (unsigned)NCATTRIBUTE, A_NORMAL ) )
    {
      if ( init )
	_init();
    }

  public:

    NCattribute()
      : defattrset( ATTRDEF )
      , attribset( (unsigned)NCATTRSET, vector<chtype>( (unsigned)NCATTRIBUTE, A_NORMAL ) )
    {
      _init();
    }
    virtual ~NCattribute() {}

    chtype GetAttrib( const NCAttribute attr ) const {
      return attribset[defattrset][attr];
    }
    chtype GetAttrib( const NCAttribute attr, const NCAttrSet attrset ) const {
      return attribset[attrset][attr];
    }
};



class NCattrcolor : public NCattribute {

  protected:

    virtual void _init();

    bool scanFile( vector<chtype> & attribs );
    void scanLine( vector<chtype> & attribs, const string & line );
    void defInitSet( vector<chtype> & attribs, short f, short b );

  public:

    NCattrcolor()
      : NCattribute( false ) {
	  _init();
    }
    virtual ~NCattrcolor() {}
};


#endif // NCattribute_h
