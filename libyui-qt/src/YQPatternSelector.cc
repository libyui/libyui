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

  File:	      YQPatternSelector.cc
  See also:   YQPatternSelectorHelp.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <qapplication.h>
#include <qhbox.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qsplitter.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQPatternSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgSelList.h"
#include "YQPkgSelectionsFilterView.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;

#define SPACING			6
#define MARGIN			4



YQPatternSelector::YQPatternSelector( QWidget *			parent,
				      const YWidgetOpt &	opt )
    : YQPackageSelectorBase( parent, opt )
{
    _detailsButton		= 0;
    _selList			= 0;
    _selectionsFilterView	= 0;
    _descriptionView		= 0;

    basicLayout();
    makeConnections();

    if ( _selList )
	_selList->filter();

    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();

    y2milestone( "PatternSelector init done" );
}


void
YQPatternSelector::basicLayout()
{
    QSplitter * outer_splitter = new QSplitter( QSplitter::Horizontal, this );
    CHECK_PTR( outer_splitter );

    QWidget * left_pane	 = layoutLeftPane ( outer_splitter );
    QWidget * right_pane = layoutRightPane( outer_splitter );

    outer_splitter->setResizeMode( left_pane,  QSplitter::Stretch );
    outer_splitter->setResizeMode( right_pane, QSplitter::FollowSizeHint );
}


QWidget *
YQPatternSelector::layoutLeftPane( QWidget * parent )
{
    QVBox * vbox = new QVBox( parent );
    CHECK_PTR( vbox );
    vbox->setMargin( MARGIN );

    //
    // Selections (Patterns) list
    //

    _selectionsFilterView = new YQPkgSelectionsFilterView( vbox );
    CHECK_PTR( _selectionsFilterView );

    _selList = _selectionsFilterView->selList();
    _selList->header()->hide();

    connect( _selList,		 SIGNAL( statusChanged()		),
	     this,		 SLOT  ( resolveSelectionDependencies()	) );

    connect( _selConflictDialog, SIGNAL( updatePackages()		),
	     _selList,		 SLOT  ( updateToplevelItemStates()	) );

    addVSpacing( vbox, MARGIN );

    
    //
    // "Details" button
    //

    QHBox * hbox = new QHBox( vbox );
    CHECK_PTR( hbox );
    
    _detailsButton = new QPushButton( _( "&Details..." ), hbox );
    CHECK_PTR( _detailsButton );

    connect( _detailsButton,	SIGNAL( clicked() ),
	     this,		SLOT  ( detailedPackageSelection() ) );

    addHStretch( hbox );


    return vbox;
}




QWidget *
YQPatternSelector::layoutRightPane( QWidget * parent )
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, parent );
    CHECK_PTR( splitter );
    splitter->setMargin( MARGIN );

    _descriptionView = new YQPkgDescriptionView( splitter );
    CHECK_PTR( _descriptionView );

    _diskUsageList = new YQPkgDiskUsageList( splitter );
    CHECK_PTR( _diskUsageList );

    splitter->setResizeMode( _descriptionView, QSplitter::Stretch );
    splitter->setResizeMode( _diskUsageList,   QSplitter::FollowSizeHint );
    
    return splitter;
}



void
YQPatternSelector::layoutButtons( QWidget * parent )
{
    QHBox * button_box = new QHBox( parent );
    CHECK_PTR( button_box );
    button_box->setSpacing( SPACING );

    addHStretch( button_box );

    QPushButton * cancel_button = new QPushButton( _( "&Cancel" ), button_box );
    CHECK_PTR( cancel_button );
    cancel_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( cancel_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( reject()   ) );


    QPushButton * accept_button = new QPushButton( _( "&Accept" ), button_box );
    CHECK_PTR( accept_button );
    accept_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( accept_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( accept()   ) );

    button_box->setFixedHeight( button_box->sizeHint().height() );
}



void
YQPatternSelector::makeConnections()
{

    if ( _selList && _descriptionView )
    {
	connect( _selList,		SIGNAL( selectionChanged    ( PMObjectPtr ) ),
		 _descriptionView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );
    }


    if ( _diskUsageList )
    {
	connect( _selectionsFilterView,	SIGNAL( updatePackages()  ),
		 _diskUsageList,	SLOT  ( updateDiskUsage() ) );
    }
}


void
YQPatternSelector::detailedPackageSelection()
{
    y2milestone( "\"Details..\" button clicked" );
    YQUI::ui()->sendEvent( new YMenuEvent( YCPSymbol( "details" ) ) );
}



#include "YQPatternSelector.moc"
