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

   File:       NCPopupTree.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupTree.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "PkgNames.h"
#include "YPkgTreeFilterView.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::NCPopupTree
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupTree::NCPopupTree( const wpos at, bool hasDescription )
    : NCPopup( at, false )
    , filterTree( 0 )
    , description ( 0 )
{
    // create the layout (the NCTree)
    createLayout( PkgNames::RpmTreeLabel(), hasDescription );

    // get the group tags
    groups = new YPkgRpmGroupTagsFilterView();
    YPkgStringTreeItem * root = groups->root();
    // clone the tree (fill the NCTree)
    cloneTree( root, 0 );
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::~NCPopupTree
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupTree::~NCPopupTree()
{
    if ( groups )
    {
	delete groups;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::createList
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupTree::createLayout( const YCPString & label, bool hasDescription )
{

  YWidgetOpt opt;
  opt.notifyMode.setValue( true );
  opt.vWeight.setValue( 70 );

  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );

  // create the tree 
  filterTree = new NCTree( split, opt, label );
  split->addChild( filterTree );

  if ( hasDescription )
  {
      opt.notifyMode.setValue( false );
      opt.vWeight.setValue( 30 );
  
      // add the description section
      description = new NCRichText( split, opt, YCPString ("") );
 
      split->addChild( description );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::showFilterPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent NCPopupTree::showFilterPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    return postevent;
}

//
//	DESCRIPTION :
//
string  NCPopupTree::getCurrentItem() const
{
  if ( !filterTree )
    return 0;

  const YTreeItem * citem = filterTree->getCurrentItem();

  DDBG << "Selected item: " << citem->getText()->value() << endl;
  
  return ( citem?citem->getText()->value():"noitem" );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupTree::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 40 : 20 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::addItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
YTreeItem * NCPopupTree::addItem( YTreeItem * parentItem,
				  const YCPString & text,
				  void * data,
				  bool  open )
{
  if ( !filterTree )
    return 0;

  return ( filterTree->addItem( parentItem, text, data, open ) );
}

 

			   
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupTree::wHandleHotkey( int key )
{

    return NCursesEvent::none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupTree::wHandleInput( int ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    NCursesEvent event = NCDialog::wHandleInput( ch );

    switch ( ch ) {
	case KEY_UP:
	case KEY_DOWN: {
	    if ( description )
	    {
		YCPValue item = filterTree->getCurrentItem()->getText();
		description->setText( item->asString() );
	    }
	}
    }
    
    return event;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupTree::postAgain()
{
    if ( !filterTree || !groups )
	return false;

    postevent.item = YCPNull();

    if (  postevent == NCursesEvent::button )
    {
	// get the currently selected item
	const YTreeItem * item = filterTree->getCurrentItem();
	// get the data pointer
	YPkgStringTreeItem * origItem = (YPkgStringTreeItem *) (item->data());

	if ( origItem )
	{
	    postevent.item =  origItem->value().translation();
	
	    NCDBG << "Orig item: " << (postevent.item)->toString() << endl;

	    // get the complete rpm tags path
	    string completePath = groups->completePath( origItem,  '/', false );
	    postevent.itemList.push_back( completePath );

	    NCMIL << "Selected RPM group: " << completePath << endl;
	}
	else
	{
	    NCERR << "Orig item not set" << endl;	
	}

        // return false means: close the popup
	return false;
    }
    
    if ( postevent == NCursesEvent::cancel )
	return false;

    return true;
}

///////////////////////////////////////////////////////////////////
//
// cloneTree
//
// Adds all tree items got from YPkgRpmGroupTagsFilterView to
// the filter popup tree
//
void NCPopupTree::cloneTree( YPkgStringTreeItem * parentOrig, YTreeItem * parentClone )
{
    // 	methods of YPkgStringTreeItem see ../libyui/src/include/TreeItem.h:  SortedTreeItem
    YPkgStringTreeItem * child = parentOrig->firstChild();
    YTreeItem *	clone;

    while ( child )
    {
	clone = addItem( parentClone,
			 YCPString( child->value().translation() ),
			 child,
			 false );

	cloneTree( child, clone );

	child = child->next();
    }
}
