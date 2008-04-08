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


#include <unistd.h>	// access()

#include <qapplication.h>
#include <qhbox.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qsplitter.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQPatternSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgSelDescriptionView.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgPatternList.h"
#include "YQPkgSelList.h"
#include "YQWizard.h"
#include "YQDialog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;

#define SPACING			6
#define MARGIN			6


YQPatternSelector::YQPatternSelector( QWidget *			parent,
				      const YWidgetOpt &	opt )
    : YQPackageSelectorBase( parent, opt )
{
    _patternList		= 0;
    _selList			= 0;
    _descriptionView		= 0;
    _wizard			= findWizard();

    basicLayout();
    makeConnections();

    if ( _patternList )
    {
	_patternList->fillList();
	_patternList->selectSomething();
    }
    else if ( _selList )
    {
	_selList->fillList();
	_selList->selectSomething();
    }

    if ( zyppPool().empty<zypp::Pattern  >() &&
	 zyppPool().empty<zypp::Selection>()   )
    {
	y2warning( "Neither patterns nor selections in ZyppPool" );
    }


    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();
}



YQWizard *
YQPatternSelector::findWizard() const
{
    YQWizard * wizard = 0;

    YQDialog * dialog = dynamic_cast<YQDialog *> ( YUI::ui()->currentDialog() );

    if ( dialog )
	wizard = dialog->findWizard();

    return wizard;
}



void
YQPatternSelector::basicLayout()
{
    QSplitter * outer_splitter = new QSplitter( QSplitter::Horizontal, this );
    CHECK_PTR( outer_splitter );

    QWidget * left_pane	 = layoutLeftPane ( outer_splitter );
    QWidget * right_pane = layoutRightPane( outer_splitter );

    int left_pane_width = (int) ( 0.3 * YQUI::ui()->defaultSize( YD_HORIZ ) );
    left_pane->resize( QSize( left_pane_width, left_pane->height() ) );
    
    outer_splitter->setResizeMode( left_pane,  QSplitter::KeepSize );
    outer_splitter->setResizeMode( right_pane, QSplitter::Stretch  );

    if ( ! _wizard )
	layoutButtons( this );
}



QWidget *
YQPatternSelector::layoutLeftPane( QWidget * parent )
{
    QVBox * vbox = new QVBox( parent );
    CHECK_PTR( vbox );
    vbox->setMargin( MARGIN );

    if ( ! zyppPool().empty<zypp::Pattern>() )
    {
	//
	// Patterns list
	//

	_patternList = new YQPkgPatternList( vbox,
					     false,	// no autoFill - need to connect to details view first
					     false );	// no autoFilter - filterMatch() is not connected
	CHECK_PTR( _patternList );
	_patternList->header()->hide();
    }

    if ( ! _patternList )
    {
	//
	// Fallback: selections list
	//

	/**
	 * Create a selections list even if there are no selections, otherwise
	 * the layout will look very weird. An empty selections list still
	 * looks better than a lot of grey empty space.
	 **/

	y2warning( "No patterns in ZyppPool - using selections instead" );
	_selList = new YQPkgSelList( vbox,
				     false,	// no autoFill - need to connect to details view first
				     false );	// no autoFilter - filterMatch() is not connected
	CHECK_PTR( _selList );
	_selList->header()->hide();
    }

    if ( _wizard )	// No button box - add "Details..." button here
    {
	//
	// "Details" button
	//

	addVSpacing( vbox, SPACING );

	QHBox * hbox = new QHBox( vbox );
	CHECK_PTR( hbox );

	QPushButton * details_button = new QPushButton( _( "&Details..." ), hbox );
	CHECK_PTR( details_button );

	connect( details_button, SIGNAL( clicked() ),
		 this,		 SLOT  ( detailedPackageSelection() ) );

	addHStretch( hbox );
    }

    return vbox;
}



QWidget *
YQPatternSelector::layoutRightPane( QWidget * parent )
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, parent );
    CHECK_PTR( splitter );
    splitter->setMargin( MARGIN );


    //
    // Selection / Pattern description
    //

    QVBox * upper_vbox = new QVBox( splitter );
    CHECK_PTR( upper_vbox );

    _descriptionView = new YQPkgSelDescriptionView( upper_vbox );
    CHECK_PTR( _descriptionView );

    addVSpacing( upper_vbox, MARGIN );


    //
    // Disk usage
    //

    QVBox * lower_vbox = new QVBox( splitter );
    CHECK_PTR( lower_vbox );
    addVSpacing( lower_vbox, MARGIN );

    _diskUsageList = new YQPkgDiskUsageList( lower_vbox );
    CHECK_PTR( _diskUsageList );

    splitter->setResizeMode( upper_vbox, QSplitter::Stretch );
    splitter->setResizeMode( lower_vbox, QSplitter::FollowSizeHint );

    return splitter;
}



void
YQPatternSelector::layoutButtons( QWidget * parent )
{
    QHBox * button_box = new QHBox( parent );
    CHECK_PTR( button_box );
    button_box->setMargin ( MARGIN  );
    button_box->setSpacing( SPACING );


    QPushButton * details_button = new QPushButton( _( "&Details..." ), button_box );
    CHECK_PTR( details_button );
    details_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( details_button,	SIGNAL( clicked() ),
	     this,		SLOT  ( detailedPackageSelection() ) );


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
    if ( _patternList )
    {
	if ( _descriptionView )
	{
	    connect( _patternList,	SIGNAL( selectionChanged( ZyppSel ) ),
		     _descriptionView,	SLOT  ( showDetails	( ZyppSel ) ) );
	}

	if ( _diskUsageList )
	{
	    connect( _patternList,	SIGNAL( updatePackages()  ),
		     _diskUsageList,	SLOT  ( updateDiskUsage() ) );
	}

	if ( access( FORCE_FULL_SOLVER_RUN_FILE, F_OK ) == 0 ) // file exists
	{
	    y2milestone( "Forcing full solver run upon pattern status change" );
	    
	    connect( _patternList,	SIGNAL( statusChanged()			),
		     this,		SLOT  ( resolvePackageDependencies()	) );
	}
    }

    if ( _selList )
    {
	if ( _descriptionView )
	{
	    connect( _selList,		SIGNAL( selectionChanged( ZyppSel ) ),
		     _descriptionView,	SLOT  ( showDetails	( ZyppSel ) ) );
	}

	if ( _diskUsageList )
	{
	    connect( _selList,		SIGNAL( updatePackages()  ),
		     _diskUsageList,	SLOT  ( updateDiskUsage() ) );
	}
    }

    y2milestone( "Connection set up" );


    if ( _wizard )
    {
	connect( _wizard, 	SIGNAL( nextClicked()	),
		 this,		SLOT  ( accept()        ) );

	connect( _wizard, 	SIGNAL( backClicked()	),
		 this,		SLOT  ( reject()	) );

	connect( _wizard, 	SIGNAL( abortClicked()	),
		 this,		SLOT  ( reject()	) );
    }
}


void
YQPatternSelector::detailedPackageSelection()
{
    y2milestone( "\"Details..\" button clicked" );
    YQUI::ui()->sendEvent( new YMenuEvent( YCPSymbol( "details" ) ) );
}


void
YQPatternSelector::debugTrace()
{
    y2warning( "debugTrace" );
}



#include "YQPatternSelector.moc"
