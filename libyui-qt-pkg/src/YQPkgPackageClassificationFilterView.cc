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

  File:		YQPkgPackageClassificationFilterView.cc

  Authors:	Duncan Mac-Vicar Prett <duncan@suse.de>
		Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>

#include "YQPackageSelector.h"

#include "YQPkgPackageClassificationFilterView.h"
#include "YQi18n.h"
#include "utf8.h"


QString
translatedText( YPkgGroupEnum group )
{
    switch ( group )
    {
	case YPKG_GROUP_ALL:			return _( "All Packages"	);
	case YPKG_GROUP_SUGGESTED:		return _( "Suggested Packages"	);
	case YPKG_GROUP_RECOMMENDED:		return _( "Recommended Packages");
	case YPKG_GROUP_ORPHANED:		return _( "Orphaned Packages"   );
	case YPKG_GROUP_UNNEEDED:		return _( "Unneeded Packages"   );
	case YPKG_GROUP_MULTIVERSION:		return _( "Multiversion Packages"   );

	// Intentionally omitting 'default' case so gcc can catch unhandled enums
    }

    // this should never be reached, not marked for translation
    return "Unknown Group";
}


string
groupIcon( YPkgGroupEnum group )
{
    switch ( group )
    {
	case YPKG_GROUP_SUGGESTED:		return( "preferences-desktop-locale"	 	);
	case YPKG_GROUP_RECOMMENDED:		return( "preferences-desktop-locale" 		);
	case YPKG_GROUP_ORPHANED:		return( "preferences-desktop-locale" 		);
	case YPKG_GROUP_UNNEEDED:		return( "preferences-desktop-locale" 		);
	case YPKG_GROUP_MULTIVERSION:		return( "preferences-desktop-locale" 		);
	case YPKG_GROUP_ALL:			return( "preferences-other"			);

	// Intentionally omitting 'default' case so gcc can catch unhandled enums
    }

    return( "" );
}

YQPkgPackageClassificationFilterView::YQPkgPackageClassificationFilterView( QWidget * parent )
    : QTreeWidget( parent )
    , _selectedGroup(YPKG_GROUP_ALL)
{
    setIconSize(QSize(32,32));
    setHeaderLabels( QStringList(_( "Package Classification" )) );
    setRootIsDecorated( false );
    setSortingEnabled( true );
    connect( this, SIGNAL( currentItemChanged	( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, SLOT	 ( slotSelectionChanged ( QTreeWidgetItem * ) ) );
    fillGroups();

    selectSomething();
}


YQPkgPackageClassificationFilterView::~YQPkgPackageClassificationFilterView()
{
}


void
YQPkgPackageClassificationFilterView::fillGroups()
{
    if ( _groupsMap.empty() )
    {
	_groupsMap[ YPKG_GROUP_ALL	   ] =	new YQPkgPackageClassificationGroup( this, YPKG_GROUP_ALL	   );
	_groupsMap[ YPKG_GROUP_RECOMMENDED ] =	new YQPkgPackageClassificationGroup( this, YPKG_GROUP_RECOMMENDED );
	_groupsMap[ YPKG_GROUP_SUGGESTED   ] =	new YQPkgPackageClassificationGroup( this, YPKG_GROUP_SUGGESTED   );
	_groupsMap[ YPKG_GROUP_ORPHANED    ] =	new YQPkgPackageClassificationGroup( this, YPKG_GROUP_ORPHANED   );
	_groupsMap[ YPKG_GROUP_UNNEEDED    ] =	new YQPkgPackageClassificationGroup( this, YPKG_GROUP_UNNEEDED   );
	if ( ! zypp::sat::Pool::instance().multiversion().empty() )
	    _groupsMap[ YPKG_GROUP_MULTIVERSION] = new YQPkgPackageClassificationGroup( this, YPKG_GROUP_MULTIVERSION );

    }
}


void
YQPkgPackageClassificationFilterView::selectSomething()
{
// FIXME
//     QTreeWidgetItem * item = children().first();
//
//     if ( item )
//	setCurrentItem(item);
}


void
YQPkgPackageClassificationFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgPackageClassificationFilterView::filter()
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
            bool match = false;

            // If there is an installed obj, check this first. The bits are set for the installed
            // obj only and the installed obj is not contained in the pick list if there in an
            // identical candidate available from a repo.
            if ( selectable->installedObj())
            {
                match = check( selectable, tryCastToZyppPkg( selectable->installedObj() ) );
            }
            if ( selectable->candidateObj() && !match)
            {
                match = check( selectable, tryCastToZyppPkg( selectable->candidateObj() ) );
            }

            // And then check the pick list which contain all availables and all objects for multi
            // version packages and the installed obj if there isn't same version in a repo.
            if ( !match )
            {
                zypp::ui::Selectable::picklist_iterator it = selectable->picklistBegin();
                while ( it != selectable->picklistEnd() && !match )
                {
                    check( selectable, tryCastToZyppPkg( *it ) );
                    ++it;
                }
            }
        }
    }
    emit filterFinished();
}


void
YQPkgPackageClassificationFilterView::slotSelectionChanged( QTreeWidgetItem * newSelection )
{
    YQPkgPackageClassificationGroup * sel = dynamic_cast<YQPkgPackageClassificationGroup *>( newSelection );

    if ( sel )
    {
	_selectedGroup = sel->group();
	// for the list of reccommended packages, we need
	// to solve first
	if ( _selectedGroup == YPKG_GROUP_SUGGESTED ||
	     _selectedGroup == YPKG_GROUP_RECOMMENDED ||
             _selectedGroup == YPKG_GROUP_ORPHANED ||
             _selectedGroup == YPKG_GROUP_UNNEEDED )
	{
	    // set the busy cursor for the solving
	    QApplication::setOverrideCursor(Qt::WaitCursor);
	    zypp::getZYpp()->resolver()->resolvePool();
	    QApplication::restoreOverrideCursor();
	}

    }
    else
    {
	_selectedGroup = YPKG_GROUP_ALL;
    }

    filter();
}


bool
YQPkgPackageClassificationFilterView::check( ZyppSel selectable,
					ZyppPkg pkg		)
{
    if ( ! pkg || ! selection() )
	return false;

    if ( selection()->group() == YPKG_GROUP_ALL )		// Special case: All packages
    {
	emit filterMatch( selectable, pkg );
	return true;
    }

    if ( selectedGroup() == YPKG_GROUP_RECOMMENDED &&
        zypp::PoolItem(pkg).status().isRecommended() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }
    if ( selectedGroup() == YPKG_GROUP_SUGGESTED &&
        zypp::PoolItem(pkg).status().isSuggested() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }
    if ( selectedGroup() == YPKG_GROUP_ORPHANED &&
        zypp::PoolItem(pkg).status().isOrphaned() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }
    if ( selectedGroup() == YPKG_GROUP_UNNEEDED &&
        zypp::PoolItem(pkg).status().isUnneeded() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }

    if ( selectedGroup() == YPKG_GROUP_MULTIVERSION &&
        selectable->multiversionInstall() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }


    return false;
}


YQPkgPackageClassificationGroup *
YQPkgPackageClassificationFilterView::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPackageClassificationGroup *> ( item );
}




YQPkgPackageClassificationGroup::YQPkgPackageClassificationGroup( YQPkgPackageClassificationFilterView * parentFilterView,
					    YPkgGroupEnum group )
    : QTreeWidgetItem( parentFilterView )
    , _filterView( parentFilterView )
    , _group( group )
{
    setText( 0, translatedText( group ) );
}


YQPkgPackageClassificationGroup::~YQPkgPackageClassificationGroup()
{
    // NOP
}


bool
YQPkgPackageClassificationGroup::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPackageClassificationGroup * otherCategoryItem =
	dynamic_cast<const YQPkgPackageClassificationGroup *>(&otherListViewItem);

    if ( otherCategoryItem )
	return group() > otherCategoryItem->group();
    else
	return true;
}


