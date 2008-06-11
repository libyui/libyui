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

   File:       NCstyle.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <fstream>
#include "fnmatch.h"

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCStyleDef.h"

#include "NCstyle.mono.h"
#include "NCstyle.braille.h"
#include "NCstyle.linux.h"
#include "NCstyle.xterm.h"
#include "NCstyle.rxvt.h"


//initialize number of colors and color pairs
int NCattribute::_colors = ::COLORS;
int NCattribute::_pairs = ::COLOR_PAIRS;



unsigned NCstyle::Style::sanitycheck()
{
    return MaxSTglobal;
}

NCattrset NCstyle::Style::attrGlobal( sanitycheck() );



// Initialize subclass StDialog in Style constructor
//		      (esp. copy constructor). Default copy constructor
//		      is not appropriate, because StDialog contains
//		      references to subclass NCattrset.
NCstyle::StDialog NCstyle::Style::initDialog()
{
    return StDialog( StBase( attr( DialogTitle ),
			     attr( DialogBorder ) ),
		     StBase( attr( DialogActiveTitle ),
			     attr( DialogActiveBorder ) ),
		     StBase( attr( DialogHeadline ),
			     attr( DialogText ) ),
		     StWidget( attr( DialogDisabled ),
			       attr( DialogDisabled ),
			       attr( DialogDisabled ),
			       attr( DialogDisabled ),
			       attr( DialogDisabled ) ),
		     StWidget( attr( DialogPlain ),
			       attr( DialogLabel ),
			       attr( DialogData ),
			       attr( DialogHint ),
			       attr( DialogScrl ) ),
		     StWidget( attr( DialogActivePlain ),
			       attr( DialogActiveLabel ),
			       attr( DialogActiveData ),
			       attr( DialogActiveHint ),
			       attr( DialogActiveScrl ) ),
		     StWidget( attr( DialogFramePlain ),
			       attr( DialogFrameLabel ),
			       attr( DialogFrameData ),
			       attr( DialogFrameHint ),
			       attr( DialogFrameScrl ) ),
		     StWidget( attr( DialogActiveFramePlain ),
			       attr( DialogActiveFrameLabel ),
			       attr( DialogActiveFrameData ),
			       attr( DialogActiveFrameHint ),
			       attr( DialogActiveFrameScrl ) ),
		     StList( attr( ListTitle ),
			     StItem( attr( ListPlain ),
				     attr( ListLabel ),
				     attr( ListData ),
				     attr( ListHint ) ),
			     StItem( attr( ListSelPlain ),
				     attr( ListSelLabel ),
				     attr( ListSelData ),
				     attr( ListSelHint ) ) ),
		     StList( attr( ListActiveTitle ),
			     StItem( attr( ListActivePlain ),
				     attr( ListActiveLabel ),
				     attr( ListActiveData ),
				     attr( ListActiveHint ) ),
			     StItem( attr( ListActiveSelPlain ),
				     attr( ListActiveSelLabel ),
				     attr( ListActiveSelData ),
				     attr( ListActiveSelHint ) ) ),
		     StList( attr( DialogDisabled ),
			     StItem( attr( DialogDisabled ),
				     attr( DialogDisabled ),
				     attr( DialogDisabled ),
				     attr( DialogDisabled ) ),
			     StItem( attr( DialogDisabled ),
				     attr( DialogDisabled ),
				     attr( DialogDisabled ),
				     attr( DialogDisabled ) ) ),
		     StProgbar( attr( ProgbarCh ),
				attr( ProgbarBgch ) ),
		     StRichtext( attr( RichTextPlain ),
				 attr( RichTextTitle ),
				 attr( RichTextLink ),
				 attr( RichTextArmedlink ),
				 attr( RichTextActiveArmedlink ),
				 attr( RichTextVisitedLink ),
				 attr( RichTextB ),
				 attr( RichTextI ),
				 attr( RichTextT ),
				 attr( RichTextBI ),
				 attr( RichTextBT ),
				 attr( RichTextIT ),
				 attr( RichTextBIT ) ),
		     attr( TextCursor )
		   );
}



NCstyle::Style::Style()
	: NCattrset( MaxSTlocal )
	, StDialog( initDialog() )
{}



NCstyle::Style::Style( const Style & rhs )
	: NCattrset( rhs )
	, StDialog( initDialog() )
{}



NCstyle::Style::~Style()
{}





#define PRT(t) case NCstyle::t: return #t;

string NCstyle::dumpName( NCstyle::StyleSet a )
{
    switch ( a )
    {
	PRT( DefaultStyle );
	PRT( InfoStyle );
	PRT( WarnStyle );
	PRT( PopupStyle );

	case NCstyle::MaxStyleSet:
	    break;
    }

    return "unknown";
}

string NCstyle::dumpName( NCstyle::STglobal a )
{
    switch ( a )
    {
	PRT( AppTitle );
	PRT( AppText );

	case NCstyle::MaxSTglobal:
	    break;
    }

    return "unknown";
}

string NCstyle::dumpName( NCstyle::STlocal a )
{
    switch ( a )
    {
	PRT( DialogBorder );
	PRT( DialogTitle );
	PRT( DialogActiveBorder );
	PRT( DialogActiveTitle );
	//
	PRT( DialogText );
	PRT( DialogHeadline );
	//
	PRT( DialogDisabled );
	//
	PRT( DialogPlain );
	PRT( DialogLabel );
	PRT( DialogData );
	PRT( DialogHint );
	PRT( DialogScrl );
	PRT( DialogActivePlain );
	PRT( DialogActiveLabel );
	PRT( DialogActiveData );
	PRT( DialogActiveHint );
	PRT( DialogActiveScrl );
	//
	PRT( DialogFramePlain );
	PRT( DialogFrameLabel );
	PRT( DialogFrameData );
	PRT( DialogFrameHint );
	PRT( DialogFrameScrl );
	PRT( DialogActiveFramePlain );
	PRT( DialogActiveFrameLabel );
	PRT( DialogActiveFrameData );
	PRT( DialogActiveFrameHint );
	PRT( DialogActiveFrameScrl );
	//
	PRT( ListTitle );
	PRT( ListPlain );
	PRT( ListLabel );
	PRT( ListData );
	PRT( ListHint );
	PRT( ListSelPlain );
	PRT( ListSelLabel );
	PRT( ListSelData );
	PRT( ListSelHint );
	//
	PRT( ListActiveTitle );
	PRT( ListActivePlain );
	PRT( ListActiveLabel );
	PRT( ListActiveData );
	PRT( ListActiveHint );
	PRT( ListActiveSelPlain );
	PRT( ListActiveSelLabel );
	PRT( ListActiveSelData );
	PRT( ListActiveSelHint );
	//
	PRT( RichTextPlain );
	PRT( RichTextTitle );
	PRT( RichTextLink );
	PRT( RichTextArmedlink );
	PRT( RichTextActiveArmedlink );
	PRT( RichTextVisitedLink );
	PRT( RichTextB );
	PRT( RichTextI );
	PRT( RichTextT );
	PRT( RichTextBI );
	PRT( RichTextBT );
	PRT( RichTextIT );
	PRT( RichTextBIT );
	//
	PRT( ProgbarCh );
	PRT( ProgbarBgch );
	//
	PRT( TextCursor );

	case NCstyle::MaxSTlocal:
	    break;
    }

    return "unknown";
}

#undef PRT



NCstyle::NCstyle( string term_t )
    : styleName( "linux" )
    , term( term_t )
    , styleSet( MaxStyleSet )
    , fakestyle_e( MaxStyleSet )
{
    char *user_defined_style = getenv( "Y2NCURSES_COLOR_THEME" );

    if ( user_defined_style && *user_defined_style )
    {
	styleName = user_defined_style;
	yuiMilestone() << "User-defined style found: " << styleName.c_str() << endl;
    }
    else
    {
	if ( NCattribute::colors() )
	{
	    if ( getenv( "Y2_BRAILLE" ) != NULL )
	    {
		styleName = "braille";
	    }
	    else
	    {
		if ( ! fnmatch( "xterm*", term_t.c_str(), 0 ) )
		    styleName = "xterm";
		else if ( ! fnmatch( "rxvt*", term_t.c_str(), 0 ) )
		    styleName = "rxvt";
	    }
	}
	else
	{
	    styleName = "mono";
	}
    }

    yuiMilestone() << "Init " << term_t << " using " << ( NCattribute::colors() ? "color" : "bw" )

		   << " => " << MaxStyleSet << " styles in " << styleName << endl;

#define IF_STYLE_INIT(n) if ( styleName == #n ) { NCstyleInit_##n( styleSet ); }
    IF_STYLE_INIT( linux )
	else IF_STYLE_INIT( xterm )
		 else IF_STYLE_INIT( rxvt )
			  else IF_STYLE_INIT( mono )
				   else IF_STYLE_INIT( braille )
					    else NCstyleInit_linux( styleSet );
}



NCstyle::~NCstyle()
{
}



void NCstyle::nextStyle()
{
    if ( !NCattribute::colors() ) return;

    if ( styleName == "mono" )
    {
	if ( term == "xterm" )
	{
	    styleName = "xterm";
	    NCstyleInit_xterm( styleSet );
	}
	else if ( term == "rxvt" )
	{
	    styleName = "rxvt";
	    NCstyleInit_rxvt( styleSet );
	}
	else
	{
	    styleName = "linux";
	    NCstyleInit_linux( styleSet );
	}
    }
    else if ( styleName == "braille" )
    {
	styleName = "mono";
	NCstyleInit_mono( styleSet );
    }
    else
    {
	styleName = "braille";
	NCstyleInit_braille( styleSet );
    }
}



void NCstyle::changeSyle()
{
    NCDefineStyle( *this );
}



void NCstyle::fakestyle( StyleSet f )
{
    fakestyle_e = f;
    NCurses::Redraw();
}
