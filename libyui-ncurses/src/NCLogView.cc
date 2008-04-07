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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCLogView.h"



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



NCLogView::~NCLogView()
{
  yuiDebug() << endl;
}

int NCLogView::preferredWidth()
{
    defsze.W = ( 5 > labelWidth() ? 5 : labelWidth() ) + 2;
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



void NCLogView::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}



void NCLogView::setLabel( const string & nlabel )
{
  YLogView::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}



void NCLogView::displayLogText( const string & ntext )
{
  DelPad();
  text = NCtext( NCstring(ntext), Columns() );
  Redraw();
}



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



void NCLogView::wRecoded()
{
  DelPad();
  wRedraw();
}



NCursesEvent NCLogView::wHandleInput( wint_t key )
{
  handleInput( key );
  return NCursesEvent::none;
}



NCPad * NCLogView::CreatePad()
{
  wsze psze( defPadSze() );
  NCPad * npad = new NCPad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  return npad;
}



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

