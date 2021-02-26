/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCattribute.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <fstream>

#define	 YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCattribute.h"
#include "stringutil.h"



// BW defaults
void NCattribute::_init()
{
    vector<chtype> & attribs( attribset[ATTRDEF] );

    attribs[NCAdebug]		= A_NORMAL;
    attribs[NCATitlewin]	= A_REVERSE;
    // WIDGETS
    attribs[NCAWdumb]		= A_NORMAL;
    attribs[NCAWdisabeled]	= A_DIM;
    attribs[NCAWnormal]		= A_NORMAL;
    attribs[NCAWlnormal]	= A_NORMAL;
    attribs[NCAWfnormal]	= A_NORMAL;
    attribs[NCAWdnormal]	= A_BOLD;
    attribs[NCAWhnormal]	= A_NORMAL;
    attribs[NCAWsnormal]	= A_NORMAL;
    attribs[NCAWactive]		= A_REVERSE;
    attribs[NCAWlactive]	= A_NORMAL;
    attribs[NCAWfactive]	= A_BOLD;
    attribs[NCAWdactive]	= A_REVERSE | A_BOLD;
    attribs[NCAWhactive]	= A_NORMAL;
    attribs[NCAWsactive]	= A_NORMAL;
    // DIALOG FRAME
    attribs[NCADnormal]		= A_NORMAL;
    attribs[NCADactive]		= A_BOLD;
    // COMMON ATTRIBUTES
    attribs[NCACheadline]	= A_BOLD;
    attribs[NCACcursor]		= A_BOLD;
    // RICHTEXT ATTRIBUTES
    attribs[NCARTred]		= A_BOLD;
    attribs[NCARTgreen]		= A_BOLD;
    attribs[NCARTblue]		= A_BOLD;
    attribs[NCARTyellow]	= A_REVERSE;
    attribs[NCARTmagenta]	= A_REVERSE;
    attribs[NCARTcyan]		= A_REVERSE;

    for ( unsigned i = 1; i < attribset.size(); ++i )
	attribset[i] = attribs;
}



static short dfg = -1;
static short dbg = -1;


inline short scanColor( const string & str, const short def )
{
    short ret = def;

    if ( str.length() == 3 )
    {
	if ( str == "BLK" )
	    ret = COLOR_BLACK;
	else if ( str == "RED" )
	    ret = COLOR_RED;
	else if ( str == "GRE" )
	    ret = COLOR_GREEN;
	else if ( str == "YEL" )
	    ret = COLOR_YELLOW;
	else if ( str == "BLU" )
	    ret = COLOR_BLUE;
	else if ( str == "MAG" )
	    ret = COLOR_MAGENTA;
	else if ( str == "CYA" )
	    ret = COLOR_CYAN;
	else if ( str == "WHT" )
	    ret = COLOR_WHITE;
    }

    return ret;
}


inline chtype scanAttrib( const string & str )
{
    chtype ret = 0;
#define IF(a) if ( str == #a ) ret = a

    if ( str.length() > 5 )
    {
	if ( str.substr( 0, 2 ) == "A_" )
	{
	    IF( A_BOLD );
	    else IF( A_REVERSE );
	    else IF( A_DIM );
	    else IF( A_NORMAL );
	    else IF( A_STANDOUT );
	    else IF( A_UNDERLINE );
	    else IF( A_BLINK );
	}
	else if ( str.substr( 0, 4 ) == "ACS_" )
	{
	    IF( ACS_BOARD );
	    else IF( ACS_CKBOARD );
	    else IF( ACS_BLOCK );
	    else IF( ACS_BULLET );
	    else IF( ACS_DIAMOND );
	    else IF( ACS_ULCORNER );
	    else IF( ACS_LLCORNER );
	    else IF( ACS_URCORNER );
	    else IF( ACS_LRCORNER );
	    else IF( ACS_LTEE );
	    else IF( ACS_RTEE );
	    else IF( ACS_BTEE );
	    else IF( ACS_TTEE );
	    else IF( ACS_HLINE );
	    else IF( ACS_VLINE );
	    else IF( ACS_PLUS );
	    else IF( ACS_LARROW );
	    else IF( ACS_RARROW );
	    else IF( ACS_DARROW );
	    else IF( ACS_UARROW );
	    else IF( ACS_S1 );
	    else IF( ACS_S3 );
	    else IF( ACS_S7 );
	    else IF( ACS_S9 );
	    else IF( ACS_DEGREE );
	    else IF( ACS_PLMINUS );
	    else IF( ACS_LANTERN );
	    else IF( ACS_LEQUAL );
	    else IF( ACS_GEQUAL );
	    else IF( ACS_PI );
	    else IF( ACS_NEQUAL );
	    else IF( ACS_STERLING );
	}
    }

#undef IF
    return ret;
}


void NCattrcolor::scanLine( vector<chtype> & attribs, const string & line )
{
    vector<string> field;
    strutil::split( line, field );

    if ( !field.size() || field[0][0] == '#' )
    {
	return; // empty line or comment
    }

    if ( strutil::split( line, field ) < 3 )
    {
	LWAR << "Short line \"" << line << '"' << field.size() << endl;
	return;
    }

    short fg = scanColor( field[1], dfg );
    short bg = scanColor( field[2], dbg );
    chtype attr = A_NORMAL;

    if ( fg == -1 || bg == -1 )
    {
	LWAR << "Undefined color on line \"" << line << '"' << endl;
    }
    else
    {
	attr = NCurses::color_pair( fg, bg );
    }

    for ( unsigned i = 3; i < field.size(); ++i )
    {
	attr |= scanAttrib( field[i] );
    }

    if ( field[0] == "NCADEFAULT" )
    {
	if ( dfg == -1 )
	{
	    attribs.clear();
	    attribs.resize(( unsigned )NCATTRIBUTE, attr );
	}

	dfg = fg;

	dbg = bg;
    }
    else
    {
#define IF(a) if ( field[0] == #a ) attribs[a] = attr
	IF( NCAdebug );
	else IF( NCATitlewin );

	// WIDGETS
	else IF( NCAWdumb );
	else IF( NCAWdisabeled );
	else IF( NCAWnormal );
	else IF( NCAWlnormal );
	else IF( NCAWfnormal );
	else IF( NCAWdnormal );
	else IF( NCAWhnormal );
	else IF( NCAWsnormal );
	else IF( NCAWactive );
	else IF( NCAWlactive );
	else IF( NCAWfactive );
	else IF( NCAWdactive );
	else IF( NCAWhactive );
	else IF( NCAWsactive );

	// DIALOG FRAME
	else IF( NCADnormal );
	else IF( NCADactive );

	// COMMON ATTRIBUTES
	else IF( NCACheadline );
	else IF( NCACcursor );

	// RICHTEXT ATTRIBUTES
	else IF( NCARTred );
	else IF( NCARTgreen );
	else IF( NCARTblue );
	else IF( NCARTyellow );
	else IF( NCARTmagenta );
	else IF( NCARTcyan );
	else
	{
	    LWAR << "Unknown attribute on line \"" << line << '"' << endl;
	}
    }

#undef IF
}


bool NCattrcolor::scanFile( vector<chtype> & attribs )
{
#if 0
    char * tmp = getenv( "Y2NC_COLORDEF" );

    if ( tmp && *tmp )
    {
	std::ifstream In( tmp );

	if ( In.good() )
	{
	    for ( ; In.gets( &tmp ); free( tmp ) )
	    {
		scanLine( attribs, tmp );
	    }

	    if ( tmp )
	    {
		free( tmp );
		tmp = 0;
	    }

	    return true;
	}
	else
	{
	    LWAR << "No file Y2NC_COLORDEF=\"" << tmp << '"' << endl;
	}
    }

#endif
    return false;
}


void NCattrcolor::defInitSet( vector<chtype> & attribs, short f, short b )
{
    //
    // default color
    //
    attribs.clear();
    attribs.resize(( unsigned )NCATTRIBUTE, NCurses::color_pair( f, b ) );
    dfg = f;
    dbg = b;
    //
    scanLine( attribs,	"NCAdebug	BLK	MAG" );
    scanLine( attribs,	"NCATitlewin	BLK	GRE" );
    //
    // WIDGETS
    //
    scanLine( attribs,	"NCAWdumb	.	.	A_BOLD" );
    scanLine( attribs,	"NCAWdisabeled	WHT	.	A_BOLD" );
    //
    scanLine( attribs,	"NCAWnormal	.	." );
    scanLine( attribs,	"NCAWlnormal	.	." );
    scanLine( attribs,	"NCAWdnormal	BLU	." );
    scanLine( attribs,	"NCAWfnormal	.	." );
    scanLine( attribs,	"NCAWhnormal	RED	." );
    scanLine( attribs,	"NCAWsnormal	.	." );
    //
    scanLine( attribs,	"NCAWactive	WHT	BLU	A_BOLD" );
    scanLine( attribs,	"NCAWlactive	.	." );
    scanLine( attribs,	"NCAWdactive	YEL	BLU	A_BOLD" );
    scanLine( attribs,	"NCAWfactive	BLU	." );
    scanLine( attribs,	"NCAWhactive	RED	BLU	A_BOLD" );
    scanLine( attribs,	"NCAWsactive	GRE	.	A_BOLD" );
    //
    // DIALOG FRAME
    //
    scanLine( attribs,	"NCADnormal	.	.	A_BOLD" );
    scanLine( attribs,	"NCADactive	.	." );
    //
    // COMMON ATTRIBUTES
    //
    scanLine( attribs,	"NCACheadline	MAG	." );
    scanLine( attribs,	"NCACcursor	BLK	YEL" );
    //
    // RICHTEXT ATTRIBUTES
    //
    scanLine( attribs,	"NCARTred	RED	WHT" );
    scanLine( attribs,	"NCARTgreen	GRE	WHT" );
    scanLine( attribs,	"NCARTblue	BLU	WHT" );
    scanLine( attribs,	"NCARTyellow	YEL	WHT" );
    scanLine( attribs,	"NCARTmagenta	MAG	WHT" );
    scanLine( attribs,	"NCARTcyan	CYA	WHT" );
}


// Color defaults
void NCattrcolor::_init()
{
    defInitSet( attribset[ATTRDEF], COLOR_BLACK, COLOR_WHITE );
    scanFile( attribset[ATTRDEF] );

    for ( unsigned i = 1; i < attribset.size(); ++i )
    {
	switch ( i )
	{
	    case ATTRWARN:
		defInitSet( attribset[i], COLOR_BLACK, COLOR_RED );
		scanLine( attribset[i], "NCAWdisabeled	BLK	.	A_BOLD" );
		scanLine( attribset[i], "NCAWdumb	BKL	." );
		scanLine( attribset[i], "NCAWnormal	WHT	." );
		scanLine( attribset[i], "NCAWlnormal	WHT	." );
		scanLine( attribset[i], "NCAWfnormal	WHT	." );
		scanLine( attribset[i], "NCAWhnormal	GRE	." );
		scanLine( attribset[i], "NCAWsnormal	WHT	." );
		scanLine( attribset[i], "NCAWlactive	WHT	." );
		scanLine( attribset[i], "NCAWfactive	WHT	.	A_BOLD" );
		scanLine( attribset[i], "NCAWhactive	GRE	BLU	A_BOLD" );
		scanLine( attribset[i], "NCAWsactive	GRE	.	A_BOLD" );
		scanLine( attribset[i], "NCAWdnormal	BLU	." );
		scanLine( attribset[i], "NCACheadline	YEL	.	A_BOLD" );
		scanLine( attribset[i], "NCADnormal	.	." );
		scanLine( attribset[i], "NCADactive	YEL	.	A_BOLD" );
		break;

	    case ATTRINFO:
		defInitSet( attribset[i], COLOR_BLACK, COLOR_YELLOW );
		scanLine( attribset[i], "NCAWdisabeled	BLK	.	A_BOLD" );
		scanLine( attribset[i], "NCAWdumb	BKL	." );
		scanLine( attribset[i], "NCAWnormal	WHT	." );
		scanLine( attribset[i], "NCAWlnormal	WHT	." );
		scanLine( attribset[i], "NCAWfnormal	WHT	." );
		scanLine( attribset[i], "NCAWsnormal	WHT	." );
		scanLine( attribset[i], "NCAWlactive	WHT	." );
		scanLine( attribset[i], "NCAWfactive	WHT	.	A_BOLD" );
		scanLine( attribset[i], "NCAWsactive	GRE	.	A_BOLD" );
		scanLine( attribset[i], "NCAWdnormal	BLU	." );
		scanLine( attribset[i], "NCACheadline	YEL	.	A_BOLD" );
		scanLine( attribset[i], "NCADnormal	.	." );
		scanLine( attribset[i], "NCADactive	YEL	.	A_BOLD" );
		scanLine( attribset[i], "NCACcursor	BLK	CYA" );
		break;

	    case ATTRPOPUP:
		defInitSet( attribset[i], COLOR_BLACK, COLOR_CYAN );
		scanLine( attribset[i], "NCAWdisabeled	WHT	." );
		scanLine( attribset[i], "NCAWdumb	BLK	.	A_BOLD" );
		scanLine( attribset[i], "NCAWnormal	BLK	." );
		scanLine( attribset[i], "NCAWlnormal	BLK	." );
		scanLine( attribset[i], "NCAWfnormal	BLK	." );
		scanLine( attribset[i], "NCAWsnormal	BLK	." );
		scanLine( attribset[i], "NCAWlactive	BLK	." );
		scanLine( attribset[i], "NCAWfactive	YEL	.	A_BOLD" );
		scanLine( attribset[i], "NCAWsactive	GRE	.	A_BOLD" );
		scanLine( attribset[i], "NCAWdnormal	BLU	." );
		scanLine( attribset[i], "NCACheadline	YEL	.	A_BOLD" );
		scanLine( attribset[i], "NCADnormal	.	." );
		scanLine( attribset[i], "NCADactive	YEL	.	A_BOLD" );
		break;

	    default:
		attribset[i] = attribset[ATTRDEF];
	}
    }
}
