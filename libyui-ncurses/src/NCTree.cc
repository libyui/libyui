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

   File:       NCTree.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCTree.h"

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableLine
//
//	DESCRIPTION :
//
class NCTreeLine : public NCTableLine {

  private:

    const YTreeItem & yitem;
    const unsigned    level;

    NCTreeLine * parent;
    NCTreeLine * nsibling;
    NCTreeLine * fchild;

    mutable chtype * prefix;

    unsigned prefixLen() const { return level + 3; }

  public:

    NCTreeLine( NCTreeLine * p, const YTreeItem & item )
      : NCTableLine( 0 )
      , yitem( item )
      , level   ( p ? p->level + 1 : 0 )
      , parent  ( p )
      , nsibling( 0 )
      , fchild  ( 0 )
      , prefix  ( 0 )
    {
      if ( parent ) {
	if ( parent->fchild ) {
	  NCTreeLine * s = parent->fchild;
	  for ( ; s->nsibling; s = s->nsibling )
	    ;
	  s->nsibling = this;
	} else {
	  parent->fchild = this;
	}
	if ( !parent->yitem.isOpenByDefault() ) {
	  SetState( S_HIDDEN );
	}
      }

      Append( new NCTableCol( YCPString( string( prefixLen(), ' ' )
					 + yitem.getText()->value() ) ) );
    }

    virtual ~NCTreeLine() { delete [] prefix; }

  public:

    const YTreeItem & Yitem() const { return yitem; }
    unsigned          Level() const { return level; }

    virtual bool isVisible() const {
      return !parent || ( !isHidden() && parent->isVisible() );
    }

    virtual int ChangeToVisible() {
      if ( isVisible() )
	return 0;

      if ( parent ) {
	parent->ChangeToVisible();
	for ( NCTreeLine * c = parent->fchild; c; c = c->nsibling )
	  c->ClearState( S_HIDDEN  );
      } else {
	ClearState( S_HIDDEN );
      }

      return 1;
    }

    virtual unsigned Hotspot( unsigned & at ) const {
      at = Level(); return 6;
    }

    virtual int  handleInput( wint_t key ) {
      if ( !fchild )
	return 0;
      switch ( key ) {
      case KEY_IC:
	if ( fchild->isVisible() )
	  return 0;
	break;
      case KEY_DC:
	if ( !fchild->isVisible() )
	  return 0;
	break;
      case KEY_SPACE:
      case '+':
      case '-':
	break;
      default:
	return 0;
	break;
      }

      if ( fchild->isVisible() ) {
	for ( NCTreeLine * c = fchild; c; c = c->nsibling )
	  c->SetState( S_HIDDEN );
      } else {
	for ( NCTreeLine * c = fchild; c; c = c->nsibling )
	  c->ClearState( S_HIDDEN  );
      }

      return 1;
    }

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 bool active ) const {

      NCTableLine::DrawAt( w, at, tableStyle, active );

      if ( !isSpecial() )
	w.bkgdset( tableStyle.getBG( vstate, NCTableCol::SEPARATOR ) );

      if ( ! prefix ) {
	prefix = new chtype[prefixLen()];
	chtype * tagend = &prefix[prefixLen()-1];
	*tagend-- = ACS_HLINE;
	*tagend-- = fchild ? ACS_TTEE : ACS_HLINE;
	if ( parent ) {
	  *tagend-- = nsibling ? ACS_LTEE : ACS_LLCORNER;
	  for ( NCTreeLine * p = parent; p; p = p->parent ) {
	    *tagend-- = p->nsibling ? ACS_VLINE : (' '&A_CHARTEXT);
	  }
	} else
	  *tagend-- = ACS_HLINE;
      }

      w.move( at.Pos.L, at.Pos.C );
      unsigned i = 0;
      for ( ; i < prefixLen(); ++i )
	w.addch( prefix[i] );

      w.move( at.Pos.L, at.Pos.C + prefixLen() - 2 );
      if ( fchild && !isSpecial() )
	w.bkgdset( tableStyle.highlightBG( vstate, NCTableCol::HINT,
					   NCTableCol::SEPARATOR ) );
      if ( fchild && !fchild->isVisible() )
	w.addch( '+' );
      else
	w.addch( prefix[prefixLen() - 2] );
    }
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::NCTree
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTree::NCTree( NCWidget * parent, const YWidgetOpt & opt,
		const YCPString & nlabel )
    : YTree( opt, nlabel )
    , NCPadWidget( parent )
    , pad( (NCTreePad *&)NCPadWidget::pad )
{
  WIDDBG << endl;
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::~NCTree
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTree::~NCTree()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::getTreeLine
//	METHOD TYPE : const NCTreeLine *
//
//	DESCRIPTION :
//
inline const NCTreeLine * NCTree::getTreeLine( unsigned idx ) const
{
  if ( pad ) {
    return dynamic_cast<const NCTreeLine *>(pad->GetLine( idx ));
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::modifyTreeLine
//	METHOD TYPE : NCTreeLine *
//
//	DESCRIPTION :
//
inline NCTreeLine * NCTree::modifyTreeLine( unsigned idx )
{
  if ( pad ) {
    return dynamic_cast<NCTreeLine *>(pad->ModifyLine( idx ));
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCTree::nicesize( YUIDimension dim )
{
  wsze sze = wsze::max( defsze, wsze( 0, labelWidht()+2 ) );
  return dim == YD_HORIZ ? sze.W : sze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTree::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YTree::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
const YTreeItem * NCTree::getCurrentItem() const
{
  const YTreeItem * yitem = 0;

  if ( pad ) {
    const NCTreeLine * cline = dynamic_cast<const NCTreeLine *>(pad->GetCurrentLine());
    if ( cline ) {
      yitem = &cline->Yitem();
    }
  }
  DDBG << "-> " << (yitem?yitem->getText()->value().c_str():"noitem") << endl;
  return yitem;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTree::setCurrentItem( YTreeItem * it )
{
  if ( !pad )
    return;
  for ( unsigned i = 0; i < pad->Lines(); ++i ) {
    const NCTreeLine * cline = getTreeLine( i );
    if ( &cline->Yitem() == it ) {
      DDBG << "got " << it->getText()->value() << " at " << i << endl;
      pad->ShowItem( cline );
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTree::setLabel( const YCPString & nlabel )
{
  YTree::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::rebuildTree
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTree::rebuildTree()
{
  DelPad();
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCTree::CreatePad()
{
  wsze    psze( defPadSze() );
  NCPad * npad = new NCTreePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::DrawPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
static void Dit( NCTreeLine * p, NCTreePad * pad, const YTreeItem * item )
{
  NCTreeLine * c = new NCTreeLine( p, *item );
  pad->Append( c );
  for ( YTreeItemListConstIterator it = item->itemList().begin();
	it < item->itemList().end(); ++it ) {
    Dit( c, pad, *it );
  }
}

void NCTree::DrawPad()
{
  for (  YTreeItemListIterator it = items.begin(); it < items.end(); ++it ) {
    Dit( 0, pad, *it );
  }
  NCPadWidget::DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTree::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCTree::wHandleInput( wint_t key )
{
  NCursesEvent ret = NCursesEvent::none;
  const YTreeItem * oldCurrentItem = getCurrentItem();

  if ( ! handleInput( key ) )
  {
    switch ( key ) {
      case KEY_SPACE:
      case KEY_RETURN:
        if ( getNotify() )
	  return NCursesEvent::Activated;
      break;
    }
  }
  
  if ( getNotify() && oldCurrentItem != getCurrentItem() )
      ret = NCursesEvent::SelectionChanged;

  return ret;
}

