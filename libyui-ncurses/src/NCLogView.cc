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
#include "Y2Log.h"
#include "NCLogView.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::NCLogView
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCLogView::NCLogView( NCWidget * parent, YWidgetOpt & opt,
		      const YCPString & nlabel,
		      int visibleLines,
		      int maxLines )
    : YLogView( opt, nlabel, visibleLines, maxLines )
    , NCPadWidget( parent )
{
  WIDDBG << endl;
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
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCLogView::nicesize( YUIDimension dim )
{
  defsze.W = ( 5 > labelWidht() ? 5 : labelWidht() ) + 2;
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YLogView::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::setLabel( const YCPString & nlabel )
{
  YLogView::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLogView::setLogText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLogView::setLogText( const YCPString & ntext )
{
  DelPad();
  text = NCstring( ntext );
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

  bool initial = ( !pad || !pad->Destwin() );
  if ( pad )
    pad->bkgd( listStyle().item.plain );
  NCPadWidget::wRedraw();

  if ( initial )
    pad->ScrlTo( wpos( text.Lines(), 0 ) );
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
    size_t columns = Columns();
    unsigned int lines = text.Lines();
    AdjustPad( wsze( lines, columns ) );
    unsigned cl = 0;

    for ( NCtext::const_iterator line = text.begin(); line != text.end(); ++line )
    {
	pad->move( cl++, 0 );
	wstring cline = (*line).str();

	if ( cl >= lines-1 )
	{
	    lines += 50;
	    AdjustPad( wsze( lines, columns ) );
	    pad->move( lines, 0 );
	}

	if ( cline.size() <= columns )
	{
	    pad->addwstr( cline.c_str() );
	}
	else
	{
	    size_t start = columns;
	    pad->addwstr( cline.substr( 0, columns).c_str() );

	    while ( start < cline.size() )
	    {
		pad->move( cl++, 0 );
		pad->addch( '~' );
		pad->addwstr( cline.substr( start, columns-1).c_str() );
		start += columns-1;
	    }
	}
    }
}

