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
#include "PackageSelector.h"

#include <Y2PM.h>
#include <y2pm/PMManager.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::NCPopupTree
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupTree::NCPopupTree( const wpos at, PackageSelector * pkg )
    : NCPopup( at, false )
    , filterTree( 0 )
    , packager ( pkg )
{
    // create the layout (the NCTree)
    createLayout( PkgNames::RpmTreeLabel() );

    // clone the tree (fill the NCTree)
    cloneTree( Y2PM::packageManager().rpmGroupsTree()->root(), 0 ); 
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
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTree::createList
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupTree::createLayout( const YCPString & label )
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

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    if ( !packager || !filterTree )
	return postevent;

    // get the currently selected rpm group and show the package list
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	const YTreeItem * item = filterTree->getCurrentItem();

	if ( item )
	{
	    // get the data pointer
	    YStringTreeItem * origItem = (YStringTreeItem *) (item->data());

	    if ( origItem )
	    {
		string label =  origItem->value().translation();

		// fill the package list 
		packager->fillPackageList( YCPString( label ), origItem ); 

		NCMIL << "Selected RPM group: " << label << endl;
	    }
	}
	else
	{
	    NCERR << "No RPM group tree existing" << endl;	
	}
    }
    
    return postevent;
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

    return NCDialog::wHandleInput( ch );
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
    postevent.detail = NCursesEvent::NODETAIL;

    if (  postevent == NCursesEvent::button )
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
void NCPopupTree::cloneTree( YStringTreeItem * parentOrig, YTreeItem * parentClone )
{
    // 	methods of YStringTreeItem see ../libyui/src/include/TreeItem.h:  SortedTreeItem
    YStringTreeItem * child = parentOrig->firstChild();
    YTreeItem *	clone;
    
    while ( child )
    {
	NCMIL << "TRANSLATION: " << child->value().translation() << endl;
	clone = addItem( parentClone,
			 YCPString( child->value().translation() ),
			 child,
			 false );

	cloneTree( child, clone );

	child = child->next();
    }
}

YStringTreeItem *  NCPopupTree::getDefaultGroup( )
{
    return Y2PM::packageManager().rpmGroupsTree()->root()->firstChild();
}
