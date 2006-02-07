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

#include "YQZypp.h"
#include <y2pm/InstTarget.h>
#include <zypp/ui/ResPoolProxy.h>
#include <zypp/ui/ResPoolProxy.h>
#include <zypp/ui/ResPoolProxy.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQPatternSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgSelDescriptionView.h"
#include "YQPkgDiskUsageList.h"
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
    _selList			= 0;
    _descriptionView		= 0;
    _wizard			= findWizard();

    basicLayout();
    makeConnections();

    if ( _selList )
    {
	_selList->fillList();
	_selList->selectSomething();
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

    outer_splitter->setResizeMode( left_pane,  QSplitter::Stretch );
    outer_splitter->setResizeMode( right_pane, QSplitter::FollowSizeHint );

    if ( ! _wizard )
	layoutButtons( this );
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

    _selList = new YQPkgSelList( vbox,
				 false ); // no autoFill - need to connect to details view first
    CHECK_PTR( _selList );
    _selList->header()->hide();

    connect( _selList,		 SIGNAL( statusChanged()		),
	     this,		 SLOT  ( resolveSelectionDependencies()	) );

    connect( _selConflictDialog, SIGNAL( updatePackages()		),
	     _selList,		 SLOT  ( updateToplevelItemStates()	) );


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

    if ( _selList && _descriptionView )
    {
	y2milestone( "Connection set up" );
	connect( _selList,		SIGNAL( selectionChanged( zypp::ResObject::Ptr ) ),
		 _descriptionView,	SLOT  ( showDetails	( zypp::ResObject::Ptr ) ) );
    }


    if ( _diskUsageList )
    {
	connect( _selList,		SIGNAL( updatePackages()  ),
		 _diskUsageList,	SLOT  ( updateDiskUsage() ) );
    }

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
