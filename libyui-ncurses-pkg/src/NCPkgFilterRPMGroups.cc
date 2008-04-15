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

   File:       NCPkgFilterRPMGroups.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterRPMGroups.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "YTreeItem.h"
#include "NCLayoutBox.h"
#include "NCPkgStrings.h"
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
//	METHOD NAME : NCPkgFilterRPMGroups::NCPkgFilterRPMGroups
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgFilterRPMGroups::NCPkgFilterRPMGroups( YWidget *parent, string label, NCPackageSelector * pkg )
    : NCTree( parent, label )
    , filterTree( this )
    , packager ( pkg )
    , _rpmGroupsTree (0)
{
    // create the layout (the NCTree)
    setNotify(true);

    _rpmGroupsTree = new YRpmGroupsTree ();

    ZyppPoolIterator b = zyppPkgBegin ();
    ZyppPoolIterator e = zyppPkgEnd ();
    ZyppPoolIterator i;

    for ( i = b; i != e;  ++i )
    {
	ZyppPkg zyppPkg = tryCastToZyppPkg( (*i)->theObj() );
	if ( zyppPkg )
	{
	    _rpmGroupsTree->addRpmGroup (zyppPkg->group ());
	    yuiDebug() << "Adding group: " << zyppPkg->group() << endl;
	}
    }

    if (_rpmGroupsTree )
    {
	// clone the tree (fill the NCTree)
	cloneTree( _rpmGroupsTree->root(), 0 );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRPMGroups::~NCPkgFilterRPMGroups
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgFilterRPMGroups::~NCPkgFilterRPMGroups()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRPMGroups::showFilterPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
bool NCPkgFilterRPMGroups::handleEvent( )
{

    YStringTreeItem * origItem;
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
    	    showRPMGroupPackages( label, origItem ); 
    
    	    yuiMilestone() << "Selected RPM group: " << label << endl;
    	}
        }
    }
    else
    {
        yuiError() << "Current item not valid" << endl;	
    }
    
    return true;
}

YStringTreeItem * NCPkgFilterRPMGroups::getDefaultRpmGroup()
{
    if ( _rpmGroupsTree &&
	 _rpmGroupsTree->root() )
	return _rpmGroupsTree->root()->firstChild();
    else
	return 0;
}


bool NCPkgFilterRPMGroups::checkPackage( ZyppObj opkg, ZyppSel slb,
				    YStringTreeItem * rpmGroup )
{
    ZyppPkg pkg = tryCastToZyppPkg (opkg);
    if ( ! pkg || ! rpmGroup )
	return false;

    NCPkgTable * packageList = packager->PackageList();

    if ( !packageList )
    {
	yuiError() << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    string group_str = _rpmGroupsTree->rpmGroup (rpmGroup);
    yuiDebug() << group_str << endl;
    // is the requested rpm group a prefix of this package's group?
    if ( pkg->group ().find (group_str) == 0 )
    {
        yuiError() << slb->name() << endl;
	packageList->createListEntry( pkg, slb );

	return true;
    }
    else
    {
	return false;
    }
}

bool NCPkgFilterRPMGroups::showRPMGroupPackages ( const string & label, YStringTreeItem *rpmGroup )
{
    NCPkgTable * packageList = packager->PackageList();

    if ( !packageList )
    {
	yuiError() << "No valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    // get the package list and sort it
    list<ZyppSel> pkgList( zyppPkgBegin (), zyppPkgEnd () );
    pkgList.sort( sortByName );

    // fill the package table
    list<ZyppSel>::iterator listIt;
    ZyppPkg pkgPtr;


    for ( listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt )
    {
	ZyppSel selectable = *listIt;

	// Multiple instances of this package may or may not be in the same
	// RPM group, so let's check both the installed version (if there
	// is any) and the candidate version.
	//
	// Make sure we emit only one filterMatch() signal if both exist
	// and both are in the same RPM group. We don't want multiple list
	// entries for the same package!

	bool match =
	    checkPackage( selectable->candidateObj(), selectable, rpmGroup ) ||
	    checkPackage( selectable->installedObj(), selectable, rpmGroup );

	// If there is neither an installed nor a candidate package, check
	// any other instance.

	if ( ! match			&&
	     ! selectable->installedObj()	&&
	     ! selectable->candidateObj()     )
	    checkPackage( selectable->theObj(), selectable, rpmGroup );

    }

    // show the package list
    packageList->setCurrentItem( 0 );
    packageList->drawList();
    packageList->showInformation();

    yuiMilestone() << "Fill package list" << endl;

    if ( ! label.empty() )
    {
	YLabel *packageLabel = packager->PackageLabel();
	// show the selected filter label
	if ( packageLabel )
	{
	   packageLabel->setText( label );
	}
    }

    return true;

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRPMGroups::addItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgFilterRPMGroups::addItem( YTreeItem * newItem )
{
  if ( !filterTree )
  {
      yuiError() << "ERROR: rpm groups tree not available" << endl;
      return;
  }

  filterTree->addItem( newItem );

}

/////////////////////////////////////////////////////////////////////
////
////
////	METHOD NAME : NCPopup::wHandleInput
////	METHOD TYPE : NCursesEvent
////
////	DESCRIPTION :
////
NCursesEvent NCPkgFilterRPMGroups::wHandleInput( wint_t ch )
{
    return NCTree::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
// cloneTree
//
// Adds all tree items got from YPkgRpmGroupTagsFilterView to
// the filter popup tree
//
void NCPkgFilterRPMGroups::cloneTree( YStringTreeItem * parentOrig, NCRpmGroupItem * parentClone )
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

