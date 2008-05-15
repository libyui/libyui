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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCTree.h"

#include "YTreeItem.h"
#include "YSelectionWidget.h"


class NCTreeLine : public NCTableLine
{

private:

    YTreeItem *		yitem;
    const unsigned	level;

    NCTreeLine * parent;
    NCTreeLine * nsibling;
    NCTreeLine * fchild;

    mutable chtype * prefix;

    unsigned prefixLen() const { return level + 3; }

public:

    NCTreeLine( NCTreeLine * p, YTreeItem * item )
      : NCTableLine( 0 )
      , yitem( item )
      , level   ( p ? p->level + 1 : 0 )
      , parent  ( p )
      , nsibling( 0 )
      , fchild  ( 0 )
      , prefix  ( 0 )
    {
	if ( parent )
	{
	    if ( parent->fchild )
	    {
		NCTreeLine * s = parent->fchild;
		for ( ; s->nsibling; s = s->nsibling )
		    ;
		s->nsibling = this;
	    }
	    else
	    {
		parent->fchild = this;
	    }
	    if ( !parent->yitem->isOpen() )
	    {
		SetState( S_HIDDEN );
	    }
	}

	Append( new NCTableCol( NCstring( string( prefixLen(), ' ' )
					  + yitem->label() ) ) );
    }

    virtual ~NCTreeLine() { delete [] prefix; }

  public:

    YTreeItem *	YItem() const { return yitem; }
    unsigned    Level() const { return level; }

    virtual bool isVisible() const
    {
	return !parent || ( !isHidden() && parent->isVisible() );
    }

    
    virtual int ChangeToVisible()
    {
	if ( isVisible() )
	    return 0;

	if ( parent )
	{
	    parent->ChangeToVisible();
	    
	    for ( NCTreeLine * c = parent->fchild; c; c = c->nsibling )
	    {
		c->ClearState( S_HIDDEN  );
		c->YItem()->setOpen( true );
	    }
	}
	else
	{
	    ClearState( S_HIDDEN );
	    yitem->setOpen( true );
	}

	return 1;
    }

    
    virtual unsigned Hotspot( unsigned & at ) const
    {
	at = Level();
	return 6;
    }

    
    virtual int  handleInput( wint_t key )
    {
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
//      case KEY_RETURN: - see bug 67350
	    case '+':
	    case '-':
		break;
	    default:
		return 0;
		break;
	}

	if ( fchild->isVisible() )
	{
	    yitem->setOpen( false );
	    yuiMilestone() << "Closing item " << yitem->label() << endl;
	    
	    for ( NCTreeLine * c = fchild; c; c = c->nsibling )
		c->SetState( S_HIDDEN );
	}
	else
	{
	    yitem->setOpen( true );
	    yuiMilestone() << "Opening item " << yitem->label() << endl;
	    
	    for ( NCTreeLine * c = fchild; c; c = c->nsibling )
		c->ClearState( S_HIDDEN  );
	}

	return 1;
    }

    
    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 bool active ) const
    {

	NCTableLine::DrawAt( w, at, tableStyle, active );

	if ( !isSpecial() )
	    w.bkgdset( tableStyle.getBG( vstate, NCTableCol::SEPARATOR ) );

	if ( ! prefix )
	{
	    prefix = new chtype[prefixLen()];
	    chtype * tagend = &prefix[prefixLen()-1];
	    *tagend-- = ACS_HLINE;
	    *tagend-- = fchild ? ACS_TTEE : ACS_HLINE;
	    
	    if ( parent )
	    {
		*tagend-- = nsibling ? ACS_LTEE : ACS_LLCORNER;
		
		for ( NCTreeLine * p = parent; p; p = p->parent )
		{
		    *tagend-- = p->nsibling ? ACS_VLINE : (' '&A_CHARTEXT);
		}
	    }
	    else
	    {
		*tagend-- = ACS_HLINE;
	    }
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






NCTree::NCTree( YWidget * parent, const string & nlabel )
    : YTree( parent, nlabel )
    , NCPadWidget( parent )
{
  yuiDebug() << endl;
  setLabel( nlabel );
}



NCTree::~NCTree()
{
  yuiDebug() << endl;
}




// Return pointer to tree line  at given index
inline const NCTreeLine * NCTree::getTreeLine( unsigned idx ) const
{
  if ( myPad() )
    return dynamic_cast<const NCTreeLine *>(myPad()->GetLine( idx ));
  else
      return 0;
}




// Modify tree line at given index
inline NCTreeLine * NCTree::modifyTreeLine( unsigned idx )
{
  if ( myPad() ) {
    return dynamic_cast<NCTreeLine *>(myPad()->ModifyLine( idx ));
  }
  return 0;
}




// Set preferred width
int NCTree::preferredWidth()
{
    wsze sze = wsze::max( defsze, wsze( 0, labelWidth()+2 ) ); 
    return sze.W; 
}




// Set preferred height
int NCTree::preferredHeight()
{
    wsze sze = wsze::max( defsze, wsze( 0, labelWidth()+2 ) );
    return sze.H;
}




// Enable/disable widget
void NCTree::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YWidget::setEnabled( do_bv );
}




void NCTree::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}




// Return YTreeItem pointer for a current line
//		      (under the cursor)
YTreeItem * NCTree::getCurrentItem() const
{
    YTreeItem * yitem = 0;

    if ( myPad() )
    {
	const NCTreeLine * cline = dynamic_cast<const NCTreeLine *>(myPad()->GetCurrentLine());
	
	if ( cline )
	    yitem = cline->YItem();
    }
    yuiDebug() << "-> " << (yitem?yitem->label().c_str():"noitem") << endl;
    
    return yitem;
}




// Set current item (under the cursor) to selected
void NCTree::selectItem( YItem *item, bool selected )
{
    if ( !myPad() )
	return;

    YTreeItem * treeItem =  dynamic_cast<YTreeItem *> (item);
    YUI_CHECK_PTR( treeItem );

    YTreeItem *citem = getCurrentItem();
    
    if ( !selected && ( treeItem == citem ))
    {
	YTree::deselectAllItems();
    }
    else
    {
	//retrieve position of item
	int at = treeItem->index();

	YTree::selectItem( treeItem, selected );
	//this highlights selected item, possibly unpacks the tree
	//should it be in currently hidden branch
	myPad()->ShowItem( getTreeLine (at) );
    }
}




// Set current item (at given index) to selected
//		      (overloaded for convenience) 
void NCTree::selectItem( int index )
{
    YItem * item = YTree::itemAt( index );

    if ( item )
    {
	selectItem( item, true);
    }
    else
	YUI_THROW( YUIException( "Can't find selected item" ) );
}



void NCTree::setLabel( const string & nlabel )
{
  YTree::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}



void NCTree::rebuildTree()
{
  DelPad();
  Redraw();
}




// Creates empty pad
NCPad * NCTree::CreatePad()
{
  wsze    psze( defPadSze() );
  NCPad * npad = new NCTreePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  return npad;
}


// Creates tree lines and appends them to TreePad
// (called recursively for each child of an item)
void NCTree::CreateTreeLines( NCTreeLine * parentLine, NCTreePad * pad, YItem * item )
{
    //static index counter
    static int idx = 0;
    //set item index explicitely, it is set to -1 by default
    //which makes selecting items painful
    item->setIndex( idx++ );

    YTreeItem * treeItem = dynamic_cast<YTreeItem *> (item);
    YUI_CHECK_PTR( treeItem );

    NCTreeLine * line = new NCTreeLine( parentLine, treeItem );
    pad->Append( line );

    // iterate over children
    for ( YItemIterator it = item->childrenBegin();  it < item->childrenEnd(); ++it )
    {
	CreateTreeLines( line, pad, *it );
    }
}




// Fills TreePad with lines (uses CreateTreeLines to create them)
void NCTree::DrawPad()
{
  if ( !myPad() )
  {
      yuiWarning() << "PadWidget not valid" << endl;
      return;
  }

  // YItemIterator iterates over the toplevel items
  for ( YItemIterator it = itemsBegin(); it < itemsEnd(); ++it )
  {
      CreateTreeLines( 0, myPad(), *it );
  }
   
  NCPadWidget::DrawPad();
}



NCursesEvent NCTree::wHandleInput( wint_t key )
{
  NCursesEvent ret = NCursesEvent::none;
  YTreeItem * oldCurrentItem = getCurrentItem();

  if ( ! handleInput( key ) )	// NCTreePad::handleInput()
  {
    switch ( key ) {
      case KEY_SPACE:		// KEY_SPACE is handled in NCTreeLine::handleInput
      case KEY_RETURN:
        if ( notify() )
	{
	  return NCursesEvent::Activated;
	}
      break;
    }
  }

  const YItem * currentItem = getCurrentItem();
  if ( !currentItem )
      return ret;
  
  YTree::selectItem( const_cast<YItem *>(currentItem), true );
   
  yuiDebug() << "Old item: " << oldCurrentItem->label() << " Current: " << currentItem->label() << endl;
  if ( notify() && immediateMode() && (oldCurrentItem != currentItem) ) 
      ret = NCursesEvent::SelectionChanged;
  

  yuiDebug() << "Notify: " << (notify()?"true":"false") <<  " Return event: " << ret << endl;
  return ret;
}





// clears the table and the lists holding
//                    the values
void NCTree::deleteAllItems()
{
	YTree::deleteAllItems();
	myPad()->ClearTable();
}
