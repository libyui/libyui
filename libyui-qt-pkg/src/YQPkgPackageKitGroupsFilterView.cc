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

  File:		YQPkgPackageKitGroupsFilterView.cc

  Authors:	Duncan Mac-Vicar Prett <duncan@suse.de>
		Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>

#include "YQPackageSelector.h"
#include "zypp/sat/LookupAttr.h"

#include "YQPkgPackageKitGroupsFilterView.h"
#include "YQi18n.h"
#include "utf8.h"


QString
translatedText( YPkgGroupEnum group )
{
    switch ( group )
    {
	case PK_GROUP_ENUM_ACCESSIBILITY:	return _( "Accessibility"	);
	case PK_GROUP_ENUM_ACCESSORIES:		return _( "Accessories"		);
	case PK_GROUP_ENUM_EDUCATION:		return _( "Education"		);
	case PK_GROUP_ENUM_GAMES:		return _( "Games"		);
	case PK_GROUP_ENUM_GRAPHICS:		return _( "Graphics"		);
	case PK_GROUP_ENUM_INTERNET:		return _( "Internet"		);
	case PK_GROUP_ENUM_OFFICE:		return _( "Office"		);
	case PK_GROUP_ENUM_OTHER:		return _( "Other"		);
	case PK_GROUP_ENUM_PROGRAMMING:		return _( "Programming"		);
	case PK_GROUP_ENUM_MULTIMEDIA:		return _( "Multimedia"		);
	case PK_GROUP_ENUM_SYSTEM:		return _( "System"		);
	case PK_GROUP_ENUM_DESKTOP_GNOME:	return _( "GNOME Desktop"	);
	case PK_GROUP_ENUM_DESKTOP_KDE:		return _( "KDE Desktop"		);
	case PK_GROUP_ENUM_DESKTOP_XFCE:	return _( "XFCE Desktop"	);
	case PK_GROUP_ENUM_DESKTOP_OTHER:	return _( "Other Desktops"	);
	case PK_GROUP_ENUM_PUBLISHING:		return _( "Publishing"		);
	case PK_GROUP_ENUM_SERVERS:		return _( "Servers"		);
	case PK_GROUP_ENUM_FONTS:		return _( "Fonts"		);
	case PK_GROUP_ENUM_ADMIN_TOOLS:		return _( "Admin Tools"		);
	case PK_GROUP_ENUM_LEGACY:		return _( "Legacy"		);
	case PK_GROUP_ENUM_LOCALIZATION:	return _( "Localization"	);
	case PK_GROUP_ENUM_VIRTUALIZATION:	return _( "Virtualization"	);
	case PK_GROUP_ENUM_SECURITY:		return _( "Security"		);
	case PK_GROUP_ENUM_POWER_MANAGEMENT:	return _( "Power Management"	);
	case PK_GROUP_ENUM_COMMUNICATION:	return _( "Communication"	);
	case PK_GROUP_ENUM_NETWORK:		return _( "Network"		);
	case PK_GROUP_ENUM_MAPS:		return _( "Maps"		);
	case PK_GROUP_ENUM_REPOS:		return _( "Software Sources"	);
	case PK_GROUP_ENUM_UNKNOWN:		return _( "Unknown Group"	);

	case YPKG_GROUP_ALL:			return _( "All Packages"	);
	case YPKG_GROUP_SUGGESTED:		return _( "Suggested Packages"	);
	case YPKG_GROUP_RECOMMENDED:		return _( "Recommended Packages");
	case YPKG_GROUP_ORPHANED:		return _( "Orphaned Packages"   );
	case YPKG_GROUP_UNNEEDED:		return _( "Unneeded Packages"   );
	case YPKG_GROUP_MULTIVERSION:		return _( "Multiversion Packages"   );

	// Intentionally omitting 'default' case so gcc can catch unhandled enums
    }

    return _("Unknown Group");
}


string
groupIcon( YPkgGroupEnum group )
{
    switch ( group )
    {
	case PK_GROUP_ENUM_ACCESSIBILITY:	return( "package_main"			);
	case PK_GROUP_ENUM_ACCESSORIES:		return( "package_applications"		);
	case PK_GROUP_ENUM_EDUCATION:		return( "package_edutainment"		);
	case PK_GROUP_ENUM_GAMES:		return( "package_games"			);
	case PK_GROUP_ENUM_GRAPHICS:		return( "package_graphics"		);
	case PK_GROUP_ENUM_INTERNET:		return( "package_network"		);
	case PK_GROUP_ENUM_OFFICE:		return( "package_office_addressbook"	);
	case PK_GROUP_ENUM_OTHER:		return( "package_main"			);
	case PK_GROUP_ENUM_PROGRAMMING:		return( "package_development"		);
	case PK_GROUP_ENUM_MULTIMEDIA:		return( "package_multimedia"		);
	case PK_GROUP_ENUM_SYSTEM:		return( "yast-system"			);
	case PK_GROUP_ENUM_DESKTOP_GNOME:	return( "pattern-gnome"			);
	case PK_GROUP_ENUM_DESKTOP_KDE:		return( "pattern-kde"			);
	case PK_GROUP_ENUM_DESKTOP_XFCE:	return( "package_utility_desktop"	);
	case PK_GROUP_ENUM_DESKTOP_OTHER:	return( "package_utility_desktop"	);
	case PK_GROUP_ENUM_PUBLISHING:		return( "package_main"			);
	case PK_GROUP_ENUM_SERVERS:		return( "package_editors"		);
	case PK_GROUP_ENUM_FONTS:		return( "package_main"			);
	case PK_GROUP_ENUM_ADMIN_TOOLS:		return( "yast-sysconfig"		);
	case PK_GROUP_ENUM_LEGACY:		return( "package_main"			);
	case PK_GROUP_ENUM_LOCALIZATION:	return( "yast-language"			);
	case PK_GROUP_ENUM_VIRTUALIZATION:	return( "yast-create-new-vm"		);
	case PK_GROUP_ENUM_SECURITY:		return( "yast-security"			);
	case PK_GROUP_ENUM_POWER_MANAGEMENT:	return( "package_settings_power"	);
	case PK_GROUP_ENUM_COMMUNICATION:	return( "yast-modem"			);
	case PK_GROUP_ENUM_NETWORK:		return( "package_network"		);
	case PK_GROUP_ENUM_MAPS:		return( "package_main"			);
	case PK_GROUP_ENUM_REPOS:		return( "package_main"			);
	case PK_GROUP_ENUM_UNKNOWN:		return( "package_main"			);

	case YPKG_GROUP_SUGGESTED:		return( "package_edutainment_languages" );
	case YPKG_GROUP_RECOMMENDED:		return( "package_edutainment_languages" );
	case YPKG_GROUP_ORPHANED:		return( "package_edutainment_languages" );
	case YPKG_GROUP_UNNEEDED:		return( "package_edutainment_languages" );
	case YPKG_GROUP_MULTIVERSION:		return( "package_edutainment_languages" );
	case YPKG_GROUP_ALL:			return( "package_main"			);

	// Intentionally omitting 'default' case so gcc can catch unhandled enums
    }

    return( "" );
}


YPkgGroupEnum
rpmGroupToPackageKitGroup( const string & rpmGroup )
{
    string group = zypp::str::toLower( rpmGroup );

    if ( group.find( "amusements/teaching"	) != string::npos ) return PK_GROUP_ENUM_EDUCATION;
    if ( group.find( "amusements"		) != string::npos ) return PK_GROUP_ENUM_GAMES;
    if ( group.find( "development"		) != string::npos ) return PK_GROUP_ENUM_PROGRAMMING;
    if ( group.find( "hardware"			) != string::npos ) return PK_GROUP_ENUM_SYSTEM;
    if ( group.find( "archiving"		) != string::npos ) return PK_GROUP_ENUM_ADMIN_TOOLS;
    if ( group.find( "clustering"		) != string::npos ) return PK_GROUP_ENUM_ADMIN_TOOLS;
    if ( group.find( "system/monitoring"	) != string::npos ) return PK_GROUP_ENUM_ADMIN_TOOLS;
    if ( group.find( "databases"		) != string::npos ) return PK_GROUP_ENUM_ADMIN_TOOLS;
    if ( group.find( "system/management"	) != string::npos ) return PK_GROUP_ENUM_ADMIN_TOOLS;
    if ( group.find( "graphics"			) != string::npos ) return PK_GROUP_ENUM_GRAPHICS;
    if ( group.find( "multimedia"		) != string::npos ) return PK_GROUP_ENUM_MULTIMEDIA;
    if ( group.find( "network"			) != string::npos ) return PK_GROUP_ENUM_NETWORK;
    if ( group.find( "office"			) != string::npos ) return PK_GROUP_ENUM_OFFICE;
    if ( group.find( "text"			) != string::npos ) return PK_GROUP_ENUM_OFFICE;
    if ( group.find( "editors"			) != string::npos ) return PK_GROUP_ENUM_OFFICE;
    if ( group.find( "publishing"		) != string::npos ) return PK_GROUP_ENUM_PUBLISHING;
    if ( group.find( "security"			) != string::npos ) return PK_GROUP_ENUM_SECURITY;
    if ( group.find( "telephony"		) != string::npos ) return PK_GROUP_ENUM_COMMUNICATION;
    if ( group.find( "gnome"			) != string::npos ) return PK_GROUP_ENUM_DESKTOP_GNOME;
    if ( group.find( "kde"			) != string::npos ) return PK_GROUP_ENUM_DESKTOP_KDE;
    if ( group.find( "xfce"			) != string::npos ) return PK_GROUP_ENUM_DESKTOP_XFCE;
    if ( group.find( "gui/other"		) != string::npos ) return PK_GROUP_ENUM_DESKTOP_OTHER;
    if ( group.find( "localization"		) != string::npos ) return PK_GROUP_ENUM_LOCALIZATION;
    if ( group.find( "system"			) != string::npos ) return PK_GROUP_ENUM_SYSTEM;
    if ( group.find( "scientific"		) != string::npos ) return PK_GROUP_ENUM_EDUCATION;

    return PK_GROUP_ENUM_UNKNOWN;
}




YQPkgPackageKitGroupsFilterView::YQPkgPackageKitGroupsFilterView( QWidget * parent )
    : QTreeWidget( parent )
    , _selectedGroup(YPKG_GROUP_ALL)
{
    setIconSize(QSize(32,32));
    setHeaderLabels( QStringList(_( "Package Groups" )) );
    setRootIsDecorated( false );
    setSortingEnabled( true );
    connect( this, SIGNAL( currentItemChanged	( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, SLOT	 ( slotSelectionChanged ( QTreeWidgetItem * ) ) );
    fillGroups();

    selectSomething();
}


YQPkgPackageKitGroupsFilterView::~YQPkgPackageKitGroupsFilterView()
{
}


void
YQPkgPackageKitGroupsFilterView::fillGroups()
{
    if ( _groupsMap.empty() )
    {
	zypp::sat::LookupAttr rpmgroups( zypp::sat::SolvAttr::group );
	for ( zypp::sat::LookupAttr::iterator it = rpmgroups.begin();
	      it != rpmgroups.end();
	      ++it )
	{
	    YPkgGroupEnum group;
	    std::map<std::string, YPkgGroupEnum>::const_iterator itg = _groupsCache.find(it.asString());
	    if ( itg == _groupsCache.end() )
	    {
		group = rpmGroupToPackageKitGroup( it.asString() );
		_groupsCache[it.asString()] = group;
	    }
	    else
	    {
		group = itg->second;
	    }

	    //std::cout << it.asString() << std::endl;

	    if ( _groupsMap.find(group) == _groupsMap.end() )
	    {
		_groupsMap[group] =  new YQPkgPackageKitGroup( this, group );
	    }
	}

	_groupsMap[ YPKG_GROUP_ALL	   ] =	new YQPkgPackageKitGroup( this, YPKG_GROUP_ALL	   );
	_groupsMap[ YPKG_GROUP_RECOMMENDED ] =	new YQPkgPackageKitGroup( this, YPKG_GROUP_RECOMMENDED );
	_groupsMap[ YPKG_GROUP_SUGGESTED   ] =	new YQPkgPackageKitGroup( this, YPKG_GROUP_SUGGESTED   );
	_groupsMap[ YPKG_GROUP_ORPHANED    ] =	new YQPkgPackageKitGroup( this, YPKG_GROUP_ORPHANED   );
	_groupsMap[ YPKG_GROUP_UNNEEDED    ] =	new YQPkgPackageKitGroup( this, YPKG_GROUP_UNNEEDED   );
	if ( ! zypp::sat::Pool::instance().multiversion().empty() )
	    _groupsMap[ YPKG_GROUP_MULTIVERSION] = new YQPkgPackageKitGroup( this, YPKG_GROUP_MULTIVERSION );

    }
}


void
YQPkgPackageKitGroupsFilterView::selectSomething()
{
// FIXME
//     QTreeWidgetItem * item = children().first();
//
//     if ( item )
//	setCurrentItem(item);
}


void
YQPkgPackageKitGroupsFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgPackageKitGroupsFilterView::filter()
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
YQPkgPackageKitGroupsFilterView::slotSelectionChanged( QTreeWidgetItem * newSelection )
{
    YQPkgPackageKitGroup * sel = dynamic_cast<YQPkgPackageKitGroup *>( newSelection );

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
YQPkgPackageKitGroupsFilterView::check( ZyppSel selectable,
					ZyppPkg pkg		)
{
    if ( ! pkg || ! selection() )
	return false;

    if ( selection()->group() == YPKG_GROUP_ALL )		// Special case: All packages
    {
	emit filterMatch( selectable, pkg );
	return true;
    }

    YPkgGroupEnum pk_group = rpmGroupToPackageKitGroup( pkg->group() );

    if ( pk_group == selectedGroup() )
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


YQPkgPackageKitGroup *
YQPkgPackageKitGroupsFilterView::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPackageKitGroup *> ( item );
}




YQPkgPackageKitGroup::YQPkgPackageKitGroup( YQPkgPackageKitGroupsFilterView * parentFilterView,
					    YPkgGroupEnum group )
    : QTreeWidgetItem( parentFilterView )
    , _filterView( parentFilterView )
    , _group( group )
{
    QFont f = font(0);
    QFontMetrics fm(f);
    f.setPixelSize( (int) ( fm.height() * 1.1 ) );
    setFont(0,f);

    string iconName = groupIcon( group );
    setText( 0, translatedText( group ) );
    setIcon( 0, QIcon( QString( YQPackageSelector::iconPath( iconName, 32 ).c_str() ) ) );
}


YQPkgPackageKitGroup::~YQPkgPackageKitGroup()
{
    // NOP
}


bool
YQPkgPackageKitGroup::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPackageKitGroup * otherCategoryItem =
	dynamic_cast<const YQPkgPackageKitGroup *>(&otherListViewItem);

    if ( otherCategoryItem )
	return group() > otherCategoryItem->group();
    else
	return true;
}


#include "YQPkgPackageKitGroupsFilterView.moc"
