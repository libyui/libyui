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

   File:       NCLogView.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCLogView.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::NCLogView
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCLogView::NCLogView( YWidget * parent,
		      const string & nlabel,
		      int visibleLines,
		      int maxLines )
    : YLogView( parent, nlabel, visibleLines, maxLines )
    , NCPadWidget( parent )
{
  yuiDebug() << endl;
  defsze = wsze( visibleLines, 5 ) + 2;
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::~NCLogView
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCLogView::~NCLogView()
{
  yuiDebug() << endl;
}

int NCLogView::preferredWidth()
{
    defsze.W = ( 5 > labelWidht() ? 5 : labelWidht() ) + 2;
    return wGetDefsze().W;
}

int NCLogView::preferredHeight()
{
    return wGetDefsze().H;
}

void NCLogView::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YLogView::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::setLabel( const string & nlabel )
{
  YLogView::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::displayLogText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::displayLogText( const string & ntext )
{
  DelPad();
  text = NCtext( NCstring(ntext), Columns() );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::wRedraw()
{
  if ( !win )
    return;

  bool initial = ( !myPad() || !myPad()->Destwin() );
  if ( myPad() )
    myPad()->bkgd( listStyle().item.plain );
  NCPadWidget::wRedraw();

  if ( initial )
    myPad()->ScrlTo( wpos( text.Lines(), 0 ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::wRecoded()
{
  DelPad();
  wRedraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCLogView::wHandleInput( wint_t key )
{
  handleInput( key );
  return NCursesEvent::none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCLogView::CreatePad()
{
  wsze psze( defPadSze() );
  NCPad * npad = new NCPad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::DrawPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::DrawPad()
{
    // maximal value for lines is 32000!
    unsigned int maxLines = 20000;
    unsigned int skipLines = 0;
    unsigned int lines = text.Lines();
    unsigned int cl = 0;

    if ( lines > maxLines )
    {
	skipLines = lines - maxLines;
	lines = maxLines;
    }

    AdjustPad( wsze( lines, Columns() ) );

    for ( NCtext::const_iterator line = text.begin(); line != text.end(); ++line )
    {
	if ( skipLines == 0 )
	{
	    myPad()->move( cl++, 0 );
	    wstring cline = (*line).str();
	    myPad()->addwstr( cline.c_str() ); 
	}
	else
	{
	    skipLines--;
	}
    }
}

