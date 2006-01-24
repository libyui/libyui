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

   File:       NCTable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCTable.h"
#include "NCPopupMenu.h"
#include <yui/YMenuButton.h>

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCTable_"
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::NCTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTable::NCTable( NCWidget * parent, const YWidgetOpt & opt,
		  vector<string> head, bool sort )
    : YTable( opt, head.size() )
    , NCPadWidget( parent )
    , immediate ( opt.immediateMode.value() )
    , sortable (sort)
    , header (head)
    , biglist( false )
{
  WIDDBG << endl;
  InitPad();
  // !!! head is UTF8 encoded, thus should be vector<YCPstring>
  vector<NCstring> headline( head.size() );
  for ( unsigned i = 0; i < head.size(); ++i ) {
    headline[i] = NCstring( head[i] );
  }
  hasHeadline = myPad()->SetHeadline( headline );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::~NCTable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTable::~NCTable()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCTable::nicesize( YUIDimension dim )
{
  wsze sze = ( biglist ) ? myPad()->tableSize() + 2 : wGetDefsze();
  return dim == YD_HORIZ ? sze.W : sze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YTable::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::setLabel( const YCPString & nlabel )
{
  // not implemented: YTable::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::setHeader
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::setHeader( const vector<string> & head )
{
    header = head;
    vector<NCstring> headline( head.size() );
    for ( unsigned i = 0; i < head.size(); ++i ) {
	headline[i] = NCstring( head[i] );
    }
    hasHeadline = myPad()->SetHeadline( headline );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCTable::getCurrentItem()
{
  if ( !myPad()->Lines() )
    return -1;
  return sortable ? myPad()->GetLine( myPad()->CurPos().L )->getIndex ()
    : myPad()->CurPos().L;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::setCurrentItem( int index )
{
  myPad()->ScrlLine( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::setItemByKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTable::setItemByKey( int key )
{
  return myPad()->setItemByKey( key );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::itemAdded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::itemAdded( vector<string> elements, int index )
{
  vector<NCTableCol*> Items( elements.size(), 0 );
  for ( unsigned i = 0; i < elements.size(); ++i ) {
    // use YCPString to enforce recoding from 'utf8'
    Items[i] = new NCTableCol( YCPString( elements[i] ) );
  }
  myPad()->Append( Items, index );
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::itemsCleared
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::itemsCleared()
{
  myPad()->ClearTable();
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::cellChanged
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTable::cellChanged( int index, int colnum, const YCPString & newtext )
{
  NCTableLine * cl = myPad()->ModifyLine( index );
  if ( !cl ) {
    NCINT << "No such line: " << wpos( index, colnum ) << newtext->value() << endl;
  } else {
    NCTableCol * cc = cl->GetCol( colnum );
    if ( !cc ) {
      NCINT << "No such colnum: " << wpos( index, colnum ) << newtext->value() << endl;
    } else {
      // use YCPString to enforce recoding from 'utf8'
      cc->SetLabel( NCstring( newtext ) );
      DrawPad();
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCTable::CreatePad()
{
  wsze    psze( defPadSze() );
  NCPad * npad = new NCTablePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );

  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTable::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCTable::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  int citem  = getCurrentItem();
  
  if ( ! handleInput( key ) ) {
    switch ( key ) {

      case CTRL('o'):
        {
	    if ( ! sortable ) 
		break;

    	    // get the column
	    wpos at( ScreenPos() + wpos( win->height()/2, 1) );
    	    YMenu a(YCPString ("Menu"));

	    int idx = 0;
	    
    	    for ( vector<string>::const_iterator it = header.begin ();
        	it != header.end () ; it++ )
    	    {
		// strip the align mark
		string col = (*it);
		col.erase(0,1);
        	a.addMenuItem ( new YMenuItem ( YCPString ( col ), &a, idx++ ) );
    	    }

    	    NCPopupMenu dialog( at, a );
    	    int column = dialog.post();

	    if( column != -1 )
		myPad ()->setOrder (column);

    	    return NCursesEvent::none;
	}
    case KEY_SPACE:
    case KEY_RETURN:
      if ( getNotify() && citem != -1 )
	return NCursesEvent::Activated;
      break;
    }
  }

  if ( getNotify() && immediate && citem != getCurrentItem() )
    ret = NCursesEvent::SelectionChanged;

  return ret;
}

