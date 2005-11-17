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

  File:	      YQPkgInstSrcFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>
#include <qsplitter.h>

#include "QY2ComboTabWidget.h"
#include "YQPkgInstSrcFilterView.h"
#include "YQPkgInstSrcList.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQi18n.h"



YQPkgInstSrcFilterView::YQPkgInstSrcFilterView( QWidget * parent )
    : QVBox( parent )
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, this );
    CHECK_PTR( splitter );

    _instSrcList = new YQPkgInstSrcList( splitter );
    CHECK_PTR( _instSrcList );
    _instSrcList->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Expanding ) );// hor/vert

    // Directly propagate signals filterStart() and filterFinished()
    // from primary filter to the outside

    connect( _instSrcList,	SIGNAL( filterStart() ),
	     this,		SIGNAL( filterStart() ) );

    connect( _instSrcList,	SIGNAL( filterFinished() ),
	     this, 		SIGNAL( filterFinished() ) );


    // Redirect filterMatch() and filterNearMatch() signals to secondary filter

    connect( _instSrcList,	SIGNAL( filterMatch		( PMPackagePtr ) ),
	     this,		SLOT  ( primaryFilterMatch	( PMPackagePtr ) ) );

    connect( _instSrcList,	SIGNAL( filterNearMatch		( PMPackagePtr ) ),
	     this,		SLOT  ( primaryFilterNearMatch	( PMPackagePtr ) ) );

#if 0
    QWidget * secondary_filters =
#endif
    layoutSecondaryFilters( splitter );

#if 0
    splitter->setResizeMode( _instSrcList,	QSplitter::Stretch        );
    splitter->setResizeMode( secondary_filters, QSplitter::FollowSizeHint );
#endif
}


YQPkgInstSrcFilterView::~YQPkgInstSrcFilterView()
{
    // NOP
}


QWidget *
YQPkgInstSrcFilterView::layoutSecondaryFilters( QWidget * parent )
{
    _secondaryFilters = new QY2ComboTabWidget( _( "&Secondary Filter:" ), parent );
    CHECK_PTR( _secondaryFilters );


    //
    // All Packages
    //

    _allPackages = new QWidget( parent );
    CHECK_PTR( _allPackages );
    _secondaryFilters->addPage( _( "All Packages" ), _allPackages );


    //
    // RPM Groups
    //

    _rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( parent );
    CHECK_PTR( _rpmGroupTagsFilterView );
    _secondaryFilters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

    connect( _rpmGroupTagsFilterView,	SIGNAL( filterStart() ),
	     _instSrcList,		SLOT  ( filter()      ) );


    //
    // Package search view
    //

    _searchFilterView = new YQPkgSearchFilterView( parent );
    CHECK_PTR( _searchFilterView );
    _secondaryFilters->addPage( _( "Search" ), _searchFilterView );
    
    connect( _searchFilterView,	SIGNAL( filterStart() ),
	     _instSrcList,	SLOT  ( filter()      ) );


    connect( _secondaryFilters, SIGNAL( currentChanged( QWidget * ) ),
	     this, 		SLOT  ( filter()		  ) );

    return _secondaryFilters;
}

void YQPkgInstSrcFilterView::filter()
{
    _instSrcList->filter();
}


void YQPkgInstSrcFilterView::filterIfVisible()
{
    _instSrcList->filterIfVisible();
}


void YQPkgInstSrcFilterView::primaryFilterMatch( PMPackagePtr pkg )
{
    if ( secondaryFilterMatch( pkg ) )
	emit filterMatch( pkg );
}


void YQPkgInstSrcFilterView::primaryFilterNearMatch( PMPackagePtr pkg )
{
    if ( secondaryFilterMatch( pkg ) )
	emit filterNearMatch( pkg );
}


bool
YQPkgInstSrcFilterView::secondaryFilterMatch( PMPackagePtr pkg )
{
    if ( _allPackages->isVisible() )
    {
	return true;
    }
    else if ( _rpmGroupTagsFilterView->isVisible() )
    {
	return _rpmGroupTagsFilterView->check( pkg );
    }
    else if ( _searchFilterView->isVisible() )
    {
	return _searchFilterView->check( pkg );
    }
    else
    {
	return true;
    }
}






#include "YQPkgInstSrcFilterView.moc"
