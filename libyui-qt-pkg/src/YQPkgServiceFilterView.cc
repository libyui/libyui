/**************************************************************************
Copyright (C) 2018 SUSE LLC
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

Textdomain	"qt-pkg"

*/

#define YUILogComponent "qt-pkg"
#include <YUILog.h>
#include <YUIException.h>

#include <QVBoxLayout>
#include <QSplitter>

#include "QY2ComboTabWidget.h"
#include "QY2LayoutUtils.h"
#include "YQPkgServiceFilterView.h"
#include "YQPkgServiceList.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgStatusFilterView.h"
#include "YQi18n.h"


YQPkgServiceFilterView::YQPkgServiceFilterView( QWidget * parent )
    : QWidget( parent )
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    QSplitter * splitter = new QSplitter( Qt::Vertical, this );
    YUI_CHECK_NEW( splitter );

    layout->addWidget( splitter );

    _serviceList = new YQPkgServiceList( this );
    splitter->addWidget(_serviceList);

    YUI_CHECK_NEW( _serviceList );
    _serviceList->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Expanding ) );// hor/vert

    // Directly propagate signals filterStart() and filterFinished()
    // from primary filter to the outside
    connect( _serviceList,	SIGNAL( filterStart() ),
	     this,		SIGNAL( filterStart() ) );

    connect( _serviceList,	SIGNAL( filterFinished() ),
	     this, 		SIGNAL( filterFinished() ) );

    // Redirect filterMatch() and filterNearMatch() signals to secondary filter
    connect( _serviceList,	SIGNAL( filterMatch		( ZyppSel, ZyppPkg ) ),
	     this,		SLOT  ( primaryFilterMatch	( ZyppSel, ZyppPkg ) ) );

    connect( _serviceList,	SIGNAL( filterNearMatch		( ZyppSel, ZyppPkg ) ),
	     this,		SLOT  ( primaryFilterNearMatch	( ZyppSel, ZyppPkg ) ) );

    layoutSecondaryFilters( splitter );

    splitter->setStretchFactor(0, 5);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 3);
}

YQPkgServiceFilterView::~YQPkgServiceFilterView()
{
    // NOP
}

QWidget *
YQPkgServiceFilterView::layoutSecondaryFilters( QWidget * parent )
{
    QWidget *vbox = new QWidget( parent );
    YUI_CHECK_NEW( vbox );

    QVBoxLayout *layout = new QVBoxLayout();
    YUI_CHECK_NEW( layout );

    vbox->setLayout( layout );
    layout->setContentsMargins( 0, 0, 0, 0 );

    // Translators: This is a combo box where the user can apply a secondary filter
    // in addition to the primary filter by repository - one of
    // "All packages", "RPM groups", "search", "summary"
    //
    // And yes, the colon really belongs there since this is one of the very
    // few cases where a combo box label is left to the combo box rather than
    // above it.
    _secondaryFilters = new QY2ComboTabWidget( _( "&Secondary Filter:" ));
    YUI_CHECK_NEW( _secondaryFilters );
    layout->addWidget(_secondaryFilters);

    //
    // All Packages
    //
    _allPackages = new QWidget( this );
    YUI_CHECK_NEW( _allPackages );
    _secondaryFilters->addPage( _( "All Packages" ), _allPackages );


    // Unmaintaned packages: Packages that are not provided in any of
    // the configured repositories
    _unmaintainedPackages = new QWidget( this );
    YUI_CHECK_NEW( _unmaintainedPackages );
    _secondaryFilters->addPage( _( "Unmaintained Packages" ), _unmaintainedPackages );

    //
    // RPM Groups
    //
    _rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( this );
    YUI_CHECK_NEW( _rpmGroupTagsFilterView );
    _secondaryFilters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

    connect( _rpmGroupTagsFilterView, SIGNAL( filterStart() ),
	     _serviceList,	SLOT ( filter() ) );

    //
    // Package search view
    //

    _searchFilterView = new YQPkgSearchFilterView( this );
    YUI_CHECK_NEW( _searchFilterView );
    _secondaryFilters->addPage( _( "Search" ), _searchFilterView );

    connect( _searchFilterView,	SIGNAL( filterStart() ),
	     _serviceList,	SLOT  ( filter()      ) );

    connect( _secondaryFilters, &QY2ComboTabWidget::currentChanged,
             this,              &YQPkgServiceFilterView::filter );

    //
    // Status change view
    //
    _statusFilterView = new YQPkgStatusFilterView( parent );
    YUI_CHECK_NEW( _statusFilterView );
    _secondaryFilters->addPage( _( "Installation Summary" ), _statusFilterView );

    connect( _statusFilterView,	SIGNAL( filterStart() ),
	     _serviceList,	SLOT  ( filter() ) );

    return _secondaryFilters;
}

void YQPkgServiceFilterView::filter()
{
    _serviceList->filter();
}

void YQPkgServiceFilterView::filterIfVisible()
{
    _serviceList->filterIfVisible();
}

void YQPkgServiceFilterView::primaryFilterMatch( ZyppSel 	selectable,
						 ZyppPkg 	pkg )
{
    if ( secondaryFilterMatch( selectable, pkg ) )
	   emit filterMatch( selectable, pkg );
}

void YQPkgServiceFilterView::primaryFilterNearMatch( ZyppSel	selectable,
						     ZyppPkg 	pkg )
{
    if ( secondaryFilterMatch( selectable, pkg ) )
	   emit filterNearMatch( selectable, pkg );
}

bool
YQPkgServiceFilterView::secondaryFilterMatch( ZyppSel	selectable,
		      ZyppPkg 	pkg )
{
    if ( _allPackages->isVisible() )
    {
	    return true;
    }
    else if ( _unmaintainedPackages->isVisible() )
    {
        return ( selectable->availableSize() == 0 );
    }
    else if ( _rpmGroupTagsFilterView->isVisible() )
    {
	    return _rpmGroupTagsFilterView->check( selectable, pkg );
    }
    else if ( _searchFilterView->isVisible() )
    {
	    return _searchFilterView->check( selectable, pkg );
    }
    else if ( _statusFilterView->isVisible() )
    {
	    return _statusFilterView->check( selectable, pkg );
    }

	return true;
}

// check if a libzypp service is present
bool YQPkgServiceFilterView::any_service()
{
	bool ret = std::any_of(ZyppRepositoriesBegin(), ZyppRepositoriesEnd(), [&](const zypp::Repository& repo) {
		// if the repository does not belong to any service then the service name is empty
		return !repo.info().service().empty();
	});

	yuiMilestone() << "Found a libzypp service: " << ret << std::endl;
	return ret;
}

#include "YQPkgServiceFilterView.moc"
