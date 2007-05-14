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
#include "Y2Log.h"
#include "NCPkgPopupTree.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCPkgNames.h"
#include "NCPackageSelector.h"

#include "NCZypp.h"

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
    createLayout( YCPString(NCPkgNames::RpmTreeLabel()) );

    // clone the tree (fill the NCTree)
    cloneTree( pkg->rpmGroupsTree()->root(), 0 ); 
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
void NCPkgPopupTree::createLayout( const YCPString & label )
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
//	METHOD NAME : NCPkgPopupTree::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPkgPopupTree::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 20 )
	vdim = 20;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()/2 : vdim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTree::addItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
YTreeItem * NCPkgPopupTree::addItem( YTreeItem * parentItem,
				  const YCPString & text,
				  void * data,
				  bool  open )
{
  if ( !filterTree )
    return 0;

  return ( filterTree->addItem( parentItem, text, YCPString( "" ), data, open ) );
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
void NCPkgPopupTree::cloneTree( YStringTreeItem * parentOrig, YTreeItem * parentClone )
{
    // 	methods of YStringTreeItem see ../libyui/src/include/TreeItem.h:  SortedTreeItem
    YStringTreeItem * child = parentOrig->firstChild();
    YTreeItem *	clone;
    
    while ( child )
    {
	NCDBG << "TRANSLATION: " << child->value().translation() << endl;
	clone = addItem( parentClone,
			 YCPString( child->value().translation() ),
			 child,
			 false );

	cloneTree( child, clone );

	child = child->next();
    }
}

YStringTreeItem *  NCPkgPopupTree::getDefaultGroup( )
{
    return packager->rpmGroupsTree()->root()->firstChild();
}
