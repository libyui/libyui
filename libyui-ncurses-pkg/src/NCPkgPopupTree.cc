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

   File:       NCPkgPopupTree.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPkgPopupTree.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "YTreeItem.h"
#include "NCLayoutBox.h"
#include "NCPkgNames.h"
#include "NCPackageSelector.h"

#include "NCZypp.h"


///////////////////////////////////////////////////////////////////
//
//
//	CLASS NAME  :	NCRpmGroupItem
//	
//	DESCRIPTION :	class derived from YTreeItem with additional
//			property to store the original rpm group item
//
//
class NCRpmGroupItem : public YTreeItem {

private:
    YStringTreeItem * rpmGroupItem;

public:

    NCRpmGroupItem( YTreeItem * 	parent,
		    const string & 	label,
		    YStringTreeItem * origItem )
	: YTreeItem( parent, label ),
	  rpmGroupItem( origItem )
	{
	    
	}

    NCRpmGroupItem( const string & 	label,
		    YStringTreeItem * origItem )
	: YTreeItem( label ),
	  rpmGroupItem( origItem )
	{
	    
	}

    YStringTreeItem * getOrigItem() const { return rpmGroupItem; }

};


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::NCPkgPopupTree
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupTree::NCPkgPopupTree( const wpos at, NCPackageSelector * pkg )
    : NCPopup( at, false )
    , filterTree( 0 )
    , packager ( pkg )
{
    // create the layout (the NCTree)
    createLayout( NCPkgNames::RpmTreeLabel() );

    if ( pkg->rpmGroupsTree() )
    {
	// clone the tree (fill the NCTree)
	cloneTree( pkg->rpmGroupsTree()->root(), 0 );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::~NCPkgPopupTree
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupTree::~NCPkgPopupTree()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::createList
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupTree::createLayout( const string & label )
{
  // the vertical split is the (only) child of the dialog
  NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

  // create the tree 
  filterTree = new NCTree( split, label );
  YUI_CHECK_NEW( filterTree );
  
  filterTree->setNotify( true );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::showFilterPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupTree::showFilterPopup( )
{
    postevent = NCursesEvent();
    
    // event loop
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    if ( !packager || !filterTree )
	return postevent;

    YStringTreeItem * origItem;

    // get the currently selected rpm group and show the package list
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	const YTreeItem * item = filterTree->getCurrentItem();

	if ( item )
	{
	    const NCRpmGroupItem * rpmGroupItem = dynamic_cast<const NCRpmGroupItem *>(item);

	    if ( rpmGroupItem )
	    {
		// get the original rpm group item (YStringTreeItem)
		origItem = rpmGroupItem->getOrigItem();
		
		if ( origItem )
		{
		    string label =  origItem->value().translation();

		    // fill the package list 
		    packager->fillPackageList( label, origItem ); 

		    yuiMilestone() << "Selected RPM group: " << label << endl;
		}
	    }
	}
	else
	{
	    yuiError() << "Current item not valid" << endl;	
	}
    }
    
    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPopupTree::preferredWidth()
{
    return NCurses::cols()/2;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPopupTree::preferredHeight()
{
    int vdim;
    if ( NCurses::lines() > 20 )
	vdim = 20;
    else
	vdim = NCurses::lines()-4;

    return vdim;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::addItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupTree::addItem( YTreeItem * newItem )
{
  if ( !filterTree )
  {
      yuiError() << "ERROR: rpm groups tree not available" << endl;
      return;
  }

  filterTree->addItem( newItem );

}

			   
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupTree::wHandleHotkey( wint_t key )
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
NCursesEvent NCPkgPopupTree::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupTree::postAgain()
{
    postevent.detail = NCursesEvent::NODETAIL;

    if (  postevent == NCursesEvent::button
	  && postevent.reason == YEvent::Activated )
    {
	postevent.detail = NCursesEvent::USERDEF ;

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
void NCPkgPopupTree::cloneTree( YStringTreeItem * parentOrig, NCRpmGroupItem * parentClone )
{
    YStringTreeItem * child = parentOrig->firstChild();
    NCRpmGroupItem  * clone;
    
    while ( child )
    {
	yuiDebug() << "Rpm group (translated): " << child->value().translation() << endl;

	if ( parentClone )
	{
	    // YTreeItems which have a parent will automatically register
	    // this item with the parent item.
	    clone = new NCRpmGroupItem( parentClone, child->value().translation(), child );
	}
	else
	{
	    clone = new NCRpmGroupItem( child->value().translation(), child );
	    // use addItem() only for the toplevel items
	    addItem( clone );
	}

	cloneTree( child, clone );
	child = child->next();
    }
}

