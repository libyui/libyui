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

  File:	      YQPkgSelList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <qregexp.h>
#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgSelList.h"

using std::set;


YQPkgSelList::YQPkgSelList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    yuiDebug() << "Creating selection list" << endl;

#if FIXME
    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    addColumn( _( "Selection" ) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );
#endif

    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;

    if ( autoFilter )
    {
	connect( this, SIGNAL( currentItemChanged( QTreeWidgetItem * ) ),
		 this, SLOT  ( filter()				   ) );
    }

    if ( autoFill )
    {
	fillList();
	selectSomething();
    }

    yuiDebug() << "Creating selection list done" << endl;
}


YQPkgSelList::~YQPkgSelList()
{
    // NOP
}


void
YQPkgSelList::fillList()
{
    clear();
    yuiDebug() << "Filling selection list" << endl;


    for ( ZyppPoolIterator it = zyppSelectionsBegin();
	  it != zyppSelectionsEnd();
	  ++it )
    {
	ZyppSelection zyppSelection = tryCastToZyppSelection( (*it)->theObj() );

	if ( zyppSelection )
	{
	    if ( zyppSelection->visible() && ! zyppSelection->isBase() )
	    {
		addPkgSelItem( *it, zyppSelection );
	    }
	}
	else
	{
	    yuiError() << "Found non-Selection selectable" << endl;
	}
    }

    yuiDebug() << "Selection list filled" << endl;
}


void
YQPkgSelList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgSelList::filter()
{
    emit filterStart();

    if ( selection() )	// The seleted QListViewItem
    {
	ZyppSelection zyppSelection = selection()->zyppSelection();

	if ( zyppSelection )
	{
	    set<string> wanted = zyppSelection->install_packages();

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		string name = (*it)->theObj()->name();

		if ( contains( wanted, name ) )
		{
		    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );

		    if ( zyppPkg )
		    {
			emit filterMatch( *it, zyppPkg );
		    }
		}
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( ZyppSel		selectable,
			     ZyppSelection	zyppSelection )
{
    if ( ! selectable )
    {
	yuiError() << "NULL ZyppSel!" << endl;
	return;
    }

    YQPkgSelListItem * item = new YQPkgSelListItem( this, selectable, zyppSelection );
    applyExcludeRules( item );
}


YQPkgSelListItem *
YQPkgSelList::selection() const
{
#if FIXME
    QTreeWidgetItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgSelListItem *> (item);
#else
    return 0;
#endif
}






YQPkgSelListItem::YQPkgSelListItem( YQPkgSelList *	pkgSelList,
				    ZyppSel		selectable,
				    ZyppSelection	zyppSelection )
    : YQPkgObjListItem( pkgSelList, selectable, zyppSelection )
    , _pkgSelList( pkgSelList )
    , _zyppSelection( zyppSelection )
{
    if ( ! _zyppSelection )
	_zyppSelection = tryCastToZyppSelection( selectable->theObj() );

    QString text = fromUTF8( _zyppSelection->summary() );

    // You don't want to know why we need this.
    text.replace( QRegExp( "Graphical Basis System" ), "Graphical Base System" );
    text.replace( QRegExp( "Gnome" ), "GNOME" );

    setText( summaryCol(), text );

    setStatusIcon();
}


YQPkgSelListItem::~YQPkgSelListItem()
{
    // NOP
}


void
YQPkgSelListItem::applyChanges()
{
    solveResolvableCollections();
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgSelListItem::compare( QTreeWidgetItem *	otherListViewItem,
			   int			col,
			   bool			ascending ) const
{
    YQPkgSelListItem * other = ( YQPkgSelListItem * ) otherListViewItem;

    if ( ! _zyppSelection || ! other || ! other->zyppSelection() )
	return 0;

    return _zyppSelection->order().compare( other->zyppSelection()->order() );
}



