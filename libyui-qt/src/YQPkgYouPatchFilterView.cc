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

  File:	      YQPkgYouPatchFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qsplitter.h>
#include <qtabwidget.h>
#include <qcheckbox.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgYouPatchFilterView.h"
#include "YQPkgYouPatchList.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgSelList.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgYouPatchFilterView::YQPkgYouPatchFilterView( QWidget * parent )
    : QVBox( parent )
{
    QVBox * vbox;
    
    _splitter			= new QSplitter( QSplitter::Vertical, this );	CHECK_PTR( _splitter 		);
    
    vbox			= new QVBox( _splitter );			CHECK_PTR( vbox		);
    _youPatchList		= new YQPkgYouPatchList( vbox );		CHECK_PTR( _youPatchList 	);
    
    addVSpacing( vbox, 4 );
    _showInstalledPatches	= new QCheckBox( _( "Include &Installed Patches" ), vbox );
    CHECK_PTR( _showInstalledPatches );
    _showInstalledPatches->setChecked( false );
    connect( _showInstalledPatches, SIGNAL( clicked() ), this, SLOT( fillPatchList() ) );
    _youPatchList->setShowInstalledPatches( _showInstalledPatches->isChecked() );
    addVSpacing( vbox, 4 );
    
    vbox			= new QVBox( _splitter );			CHECK_PTR( vbox		);
    addVSpacing( vbox, 8 );
    
    _detailsViews		= new QTabWidget( vbox );			CHECK_PTR( _detailsViews	);
    _detailsViews->setMargin( MARGIN );
    
    _descriptionView		= new YQPkgDescriptionView( _detailsViews );	CHECK_PTR( _descriptionView	);
    
    _descriptionView->setMinimumSize( 0, 0 );
    _detailsViews->addTab( _descriptionView, _( "Patch Description" ) );

#if 0
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert
#endif

    connect( _youPatchList,	SIGNAL( selectionChanged    ( PMObjectPtr ) ),
	     _descriptionView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );
}


YQPkgYouPatchFilterView::~YQPkgYouPatchFilterView()
{
    // NOP
}


void
YQPkgYouPatchFilterView::fillPatchList()
{
    _youPatchList->setShowInstalledPatches( _showInstalledPatches->isChecked() );
    _youPatchList->fillList();
    _youPatchList->selectSomething();
}


QSize
YQPkgYouPatchFilterView::sizeHint() const
{
    return QSize( 600, 350 );
}





#include "YQPkgYouPatchFilterView.moc.cc"
