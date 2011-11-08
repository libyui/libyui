/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgRpmGroupTagsFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQi18n.h"
#include "utf8.h"



YRpmGroupsTree * YQPkgRpmGroupTagsFilterView::_rpmGroupsTree = 0;


YQPkgRpmGroupTagsFilterView::YQPkgRpmGroupTagsFilterView( QWidget * parent )
    : QTreeWidget( parent )
{
    setHeaderLabels( QStringList(_( "Package Groups" )) );
    setRootIsDecorated( true );
    cloneTree( rpmGroupsTree()->root(), 0 );

    new YQPkgRpmGroupTag( this, _( "zzz All" ), 0 );

    connect( this, SIGNAL( currentItemChanged     ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, SLOT  ( slotSelectionChanged	( QTreeWidgetItem * ) ) );

    selectSomething();
}


YQPkgRpmGroupTagsFilterView::~YQPkgRpmGroupTagsFilterView()
{
}


YRpmGroupsTree *
YQPkgRpmGroupTagsFilterView::rpmGroupsTree()
{
    if ( ! _rpmGroupsTree )
    {
	_rpmGroupsTree = new YRpmGroupsTree();
	Q_CHECK_PTR( _rpmGroupsTree );
	_rpmGroupsTree->setTextdomain("qt-pkg");

	fillRpmGroupsTree();
    }

    return _rpmGroupsTree;
}


void
YQPkgRpmGroupTagsFilterView::fillRpmGroupsTree()
{
    yuiDebug() << "Filling RPM groups tree" << endl;

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );

	if ( zyppPkg )
	    rpmGroupsTree()->addRpmGroup( zyppPkg->group() );
    }

    yuiDebug() << "Filling RPM groups tree done" << endl;
}


void
YQPkgRpmGroupTagsFilterView::cloneTree( YStringTreeItem * 	parentRpmGroup,
					YQPkgRpmGroupTag * 	parentClone )
{
    YStringTreeItem * 	child = parentRpmGroup->firstChild();
    YQPkgRpmGroupTag * 	clone;

    while ( child )
    {
	if ( parentClone )
	    clone = new YQPkgRpmGroupTag( this, parentClone, child );
	else
	    clone = new YQPkgRpmGroupTag( this, child );

	Q_CHECK_PTR( clone );
	//FIXME clone->setExpanded( clone->depth() < 1 );
        clone->setExpanded( true );
	cloneTree( child, clone );
	child = child->next();
    }
}


void
YQPkgRpmGroupTagsFilterView::selectSomething()
{
// FIXME
//     QTreeWidgetItem * item = children().first();
// 
//     if ( item )
// 	setCurrentItem(item);
}


void
YQPkgRpmGroupTagsFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgRpmGroupTagsFilterView::filter()
{
    emit filterStart();
    // yuiDebug() << "Filtering packages for RPM group \"" << selectedRpmGroup() << "\"" << endl;
    
    if ( selection() )
    {
	for ( ZyppPoolIterator it = zyppPkgBegin();
	      it != zyppPkgEnd();
	      ++it )
	{
	    ZyppSel selectable = *it;

	    // Multiple instances of this package may or may not be in the same
	    // RPM group, so let's check both the installed version (if there
	    // is any) and the candidate version.
	    //
	    // Make sure we emit only one filterMatch() signal if both exist
	    // and both are in the same RPM group. We don't want multiple list
	    // entries for the same package!

	    bool match =
		check( selectable, tryCastToZyppPkg( selectable->candidateObj() ) ) ||
		check( selectable, tryCastToZyppPkg( selectable->installedObj() ) );

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.

	    if ( ! match			&&
		 ! selectable->candidateObj()   &&
		 ! selectable->installedObj()	  )
		check( selectable, tryCastToZyppPkg( selectable->theObj() ) );
	}
    }

    emit filterFinished();
}


void
YQPkgRpmGroupTagsFilterView::slotSelectionChanged( QTreeWidgetItem * newSelection )
{
    YQPkgRpmGroupTag * sel = dynamic_cast<YQPkgRpmGroupTag *>( newSelection );

    if ( sel )
    {
	if ( sel->rpmGroup() )
	    _selectedRpmGroup = rpmGroupsTree()->rpmGroup( sel->rpmGroup() );
	else
	    _selectedRpmGroup = "*";	// "zzz_All"
    }
    else
    {
	_selectedRpmGroup = "";
    }

    filter();
}


bool
YQPkgRpmGroupTagsFilterView::check( ZyppSel	selectable,
				    ZyppPkg	pkg		)
{
    if ( ! pkg || ! selection() )
	return false;

    if ( selection()->rpmGroup() == 0 )		// Special case: All packages
    {
	emit filterMatch( selectable, pkg );
	return true;
    }

    if ( selectedRpmGroup().empty() )
	return false;
    
    if ( pkg->group() == selectedRpmGroup() ||			// full match?
	 pkg->group().find( selectedRpmGroup() + "/" ) == 0 )	// starts with selected?
    {
	emit filterMatch( selectable, pkg );
	return true;
    }

    return false;
}


YQPkgRpmGroupTag *
YQPkgRpmGroupTagsFilterView::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgRpmGroupTag *> ( item );
}






YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YStringTreeItem *			rpmGroup	)
    : QTreeWidgetItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  fromUTF8( _rpmGroup->value().translation() ) );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YQPkgRpmGroupTag * 			parentGroupTag,
				    YStringTreeItem *	 		rpmGroup	)
    : QTreeWidgetItem( parentGroupTag )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  fromUTF8( _rpmGroup->value().translation() )  );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    const QString &			rpmGroupName,
				    YStringTreeItem *			rpmGroup	)
    : QTreeWidgetItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  rpmGroupName );
}


YQPkgRpmGroupTag::~YQPkgRpmGroupTag()
{
    // NOP
}



#include "YQPkgRpmGroupTagsFilterView.moc"
