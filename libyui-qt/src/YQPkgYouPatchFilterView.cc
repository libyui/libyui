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

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qsplitter.h>
#include <qtabwidget.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgYouPatchFilterView.h"
#include "YQPkgYouPatchList.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgSelList.h"
#include "YQi18n.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgYouPatchFilterView::YQPkgYouPatchFilterView( QWidget * parent )
    : QVBox( parent )
{
    QVBox *	details_vbox;
    QWidget *	spacer;
    
    _splitter		= new QSplitter( QSplitter::Vertical, this );	CHECK_PTR( _splitter 		);
    _youPatchList	= new YQPkgYouPatchList( _splitter );		CHECK_PTR( _youPatchList 	);
    
    details_vbox	= new QVBox( _splitter );			CHECK_PTR( details_vbox		);
    spacer		= new QWidget   ( details_vbox );		CHECK_PTR( spacer		);
    spacer->setFixedHeight( 8 );
    
    _detailsViews	= new QTabWidget( details_vbox );		CHECK_PTR( _detailsViews	);
    _detailsViews->setMargin( MARGIN );
    
    _descriptionView	= new YQPkgDescriptionView( _detailsViews );	CHECK_PTR( _descriptionView	);
    
    _descriptionView->setMinimumSize( 0, 0 );
    _detailsViews->addTab( _descriptionView, _( "&Patch Description" ) );

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


QSize
YQPkgYouPatchFilterView::sizeHint() const
{
    return QSize( 600, 350 );
}





#include "YQPkgYouPatchFilterView.moc.cc"
