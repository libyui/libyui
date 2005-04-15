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

   File:       Y2Log.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include "Y2Log.h"
#include <ycp/y2log.h>

namespace Y2Log {

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Y2Loglinebuf
//
//	DESCRIPTION :
//
class Y2Loglinebuf : public streambuf {

  private:

    friend class Y2Loglinestream;

    bool         enabled;
    string       name;
    int          level;
    const char * file;
    const char * func;
    int          line;

    string       buffer;

    virtual streamsize xsputn( const char * s, streamsize n ) {
      return writeout( s, n );
    }

    virtual int overflow( int ch = EOF ) {
      if ( ch != EOF ) {
	char tmp = ch;
        writeout( &tmp, 1 );
      }
      return 0;
    }

    virtual int writeout( const char* s, streamsize n ) {
      if ( !enabled )
	return n;

      if ( s && n ) {
	const char * c = s;
	for ( int i = 0; i < n; ++i, ++c ) {
	  if ( *c == '\n' ) {
	    buffer += string( s, c-s );
	    y2_logger( (loglevel_t)level, name.c_str(),
		       file, line, func,
		       "%s", buffer.c_str() );
	    buffer = "";
	    s = c+1;
	  }
	}
	if ( s < c ) {
	  buffer += string( s, c-s );
	}
      }
      return n;
    }

    Y2Loglinebuf( const string & myname, int mylevel, bool dolog ) {
      enabled = dolog;
      name    = myname;
      level   = mylevel;
      file    = func = "";
      line    = -1;
      buffer  = "";
    }
    ~Y2Loglinebuf() {
      if ( buffer.length() )
	writeout( "\n", 1 );
    }
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Y2Loglinebuf
//
//	DESCRIPTION :
//
class Y2Loglinestream {

  private:

    Y2Loglinebuf mybuf;
    ostream      mystream;

  public:

    Y2Loglinestream( const string & name, int level, bool dolog )
      : mybuf( name, level, dolog && should_be_logged( level, name ) )
      , mystream( &mybuf )
    {}
    ~Y2Loglinestream() { mystream.flush(); }

  public:

    ostream & getStream( const char * fil, const char * fnc, int lne ) {
      mybuf.file = fil;
      mybuf.func = fnc;
      mybuf.line = lne;
      return mystream;
    }

    ostream & getStream() {
      return mystream;
    }

};
///////////////////////////////////////////////////////////////////

struct Y2Loglinestreamset {

  Y2Loglinestream                   no_stream;
  vector<vector<Y2Loglinestream*> > Set;

  Y2Loglinestreamset() : no_stream( "no_stream", 0, false ) {
    no_stream.getStream().setstate( ios::badbit );
    Set.resize( C_DBG + 1 );
    for ( unsigned i = 0; i <= C_DBG; ++i ) {
      _init( i );
    }
  }

  void _init( unsigned num ) {
    string name( "ncurses-dbg" );
    bool   dolog = true;
    switch ( num ) {
    case C_Y2UI:
      name = "ui-ncurses";
      break;
    case C_NC:
      name = "ncurses";
      break;
    case C_NC_WIDGET:
      name = "ncurses-widget";
      break;
    case C_NC_IO:
      name = "ncurses-io";
      break;
    case C_TRACE:
      name = "----->";
      dolog = (getenv( "Y2NCDBG" ) != NULL);
      break;
    default:
    case C_DBG:
      name = "ncurses-dbg";
      dolog = (getenv( "Y2NCDBG" ) != NULL);
      break;
    }

    Set[num].resize( 6 );
    for ( unsigned i = 0; i < 6; ++i )
      Set[num][i] = new Y2Loglinestream( name, i, dolog );
  }
};

static Y2Loglinestreamset streamset;

/******************************************************************
**
**
**	FUNCTION NAME : Y2Log::get
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & get( unsigned which, unsigned level,
		      const char * fil, const char * fnc, int lne )
{
  if ( which > C_DBG )
    which = C_DBG;
  return streamset.Set[which][level]->getStream( fil, fnc, lne );
}

/******************************************************************
**
**
**	FUNCTION NAME : Y2Log::dbg_get
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & dbg_get( const char * dbg_class, unsigned level,
			  const char * fil, const char * fnc, int lne )
{
  if ( !dbg_class )
    return streamset.no_stream.getStream();

  if ( *dbg_class )
    return streamset.Set[C_DBG][level]->getStream( dbg_class, fnc, lne );

  return streamset.Set[C_DBG][level]->getStream( fil, fnc, lne );
}

} // namespace Y2Log
