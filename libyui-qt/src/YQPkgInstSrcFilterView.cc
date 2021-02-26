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

  File:		YQPkgInstSrcFilterView.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qsplitter.h>

#include "QY2ComboTabWidget.h"
#include "QY2LayoutUtils.h"
#include "YQPkgInstSrcFilterView.h"
#include "YQPkgInstSrcList.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgStatusFilterView.h"
#include "YQi18n.h"

#define MARGIN			4


YQPkgInstSrcFilterView::YQPkgInstSrcFilterView( QWidget * parent )
    : QVBox( parent )
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, this );
    CHECK_PTR( splitter );

    QVBox * upper_vbox = new QVBox( splitter );
    _instSrcList = new YQPkgInstSrcList( upper_vbox );
    CHECK_PTR( _instSrcList );
    _instSrcList->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Expanding ) );// hor/vert

    addVSpacing( upper_vbox, MARGIN );


    // Directly propagate signals filterStart() and filterFinished()
    // from primary filter to the outside

    connect( _instSrcList,	SIGNAL( filterStart() ),
	     this,		SIGNAL( filterStart() ) );

    connect( _instSrcList,	SIGNAL( filterFinished() ),
	     this, 		SIGNAL( filterFinished() ) );


    // Redirect filterMatch() and filterNearMatch() signals to secondary filter

    connect( _instSrcList,	SIGNAL( filterMatch		( ZyppSel, ZyppPkg ) ),
	     this,		SLOT  ( primaryFilterMatch	( ZyppSel, ZyppPkg ) ) );

    connect( _instSrcList,	SIGNAL( filterNearMatch		( ZyppSel, ZyppPkg ) ),
	     this,		SLOT  ( primaryFilterNearMatch	( ZyppSel, ZyppPkg ) ) );

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
    QVBox *vbox = new QVBox( parent );
    CHECK_PTR( vbox );
    addVSpacing( vbox, MARGIN );

    // Translators: This is a combo box where the user can apply a secondary filter
    // in addition to the primary filter by installation source - one of
    // "all packages", "RPM groups", "search", "summary"
    _secondaryFilters = new QY2ComboTabWidget( _( "&Secondary Filter:" ), vbox );
    CHECK_PTR( _secondaryFilters );

    _secondaryFilters->setFrameStyle( QFrame::Plain );
    _secondaryFilters->setLineWidth( 0 );
    _secondaryFilters->setMidLineWidth( 0 );
    _secondaryFilters->setMargin( 0 );


    //
    // All Packages
    //

    _allPackages = new QWidget( vbox );
    CHECK_PTR( _allPackages );
    _secondaryFilters->addPage( _( "All Packages" ), _allPackages );


    //
    // RPM Groups
    //

    _rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( vbox );
    CHECK_PTR( _rpmGroupTagsFilterView );
    _secondaryFilters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

    connect( _rpmGroupTagsFilterView,	SIGNAL( filterStart() ),
	     _instSrcList,		SLOT  ( filter()      ) );


    //
    // Package search view
    //

    _searchFilterView = new YQPkgSearchFilterView( vbox );
    CHECK_PTR( _searchFilterView );
    _secondaryFilters->addPage( _( "Search" ), _searchFilterView );

    connect( _searchFilterView,	SIGNAL( filterStart() ),
	     _instSrcList,	SLOT  ( filter()      ) );

    connect( _secondaryFilters, SIGNAL( currentChanged( QWidget * ) ),
	     this, 		SLOT  ( filter()		  ) );


    //
    // Status change view
    //

    _statusFilterView = new YQPkgStatusFilterView( parent );
    CHECK_PTR( _statusFilterView );
    _secondaryFilters->addPage( _( "Installation Summary" ), _statusFilterView );

    connect( _statusFilterView,	SIGNAL( filterStart() ),
	     _instSrcList,	SLOT  ( filter()      ) );


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


void YQPkgInstSrcFilterView::primaryFilterMatch( ZyppSel 	selectable,
						 ZyppPkg 	pkg )
{
    if ( secondaryFilterMatch( selectable, pkg ) )
	emit filterMatch( selectable, pkg );
}


void YQPkgInstSrcFilterView::primaryFilterNearMatch( ZyppSel	selectable,
						     ZyppPkg 	pkg )
{
    if ( secondaryFilterMatch( selectable, pkg ) )
	emit filterNearMatch( selectable, pkg );
}


bool
YQPkgInstSrcFilterView::secondaryFilterMatch( ZyppSel	selectable,
					      ZyppPkg 	pkg )
{
    if ( _allPackages->isVisible() )
    {
	return true;
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
    else
    {
	return true;
    }
}






#include "YQPkgInstSrcFilterView.moc"
