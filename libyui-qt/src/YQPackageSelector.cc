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

  File:	      YQPackageSelector.cc
  See also:   YQPackageSelectorHelp.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define CHECK_DEPENDENCIES_ON_STARTUP			1
#define DEPENDENCY_FEEDBACK_IF_OK			1
#define AUTO_CHECK_DEPENDENCIES_DEFAULT			false
#define ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE	0
#define GLOBAL_UPDATE_CONFIRMATION_THRESHOLD		20

#include <fstream>
#include <boost/bind.hpp>

#include <qaction.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qmap.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtimer.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQPackageSelector.h"
#include "YQPkgChangeLogView.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"
#include "YQPkgDependenciesView.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgFileListView.h"
#include "YQPkgInstSrcFilterView.h"
#include "YQPkgInstSrcList.h"
#include "YQPkgLangList.h"
#include "YQPkgList.h"
#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQPkgPatternList.h"
#include "YQPkgProductDialog.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgSelList.h"
#include "YQPkgStatusFilterView.h"
#include "YQPkgTechnicalDetailsView.h"
#include "YQPkgTextDialog.h"
#include "YQPkgUpdateProblemFilterView.h"
#include "YQPkgVersionsView.h"
#include "zypp/SysContent.h"

#include "QY2ComboTabWidget.h"
#include "YQDialog.h"
#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;
using std::map;
using std::pair;

#define SPACING				6
#define MARGIN				4
#define DEFAULT_EXPORT_FILE_NAME	"user-packages.xml"



YQPackageSelector::YQPackageSelector( QWidget *			parent,
				      const YWidgetOpt &	opt )
    : YQPackageSelectorBase( parent, opt )
{
    _showChangesDialog		= true;
    _autoDependenciesCheckBox	= 0;
    _detailsViews		= 0;
    _diskUsageList		= 0;
    _filters			= 0;
    _instSrcFilterView		= 0;
    _langList			= 0;
    _patternList		= 0;
    _pkgChangeLogView		= 0;
    _pkgDependenciesView	= 0;
    _pkgDescriptionView		= 0;
    _pkgFileListView		= 0;
    _pkgList			= 0;
    _pkgTechnicalDetailsView	= 0;
    _pkgVersionsView		= 0;
    _rpmGroupTagsFilterView	= 0;
    _searchFilterView		= 0;
    _selList			= 0;
    _statusFilterView		= 0;
    _updateProblemFilterView	= 0;
    _patchFilterView		= 0;
    _patchList			= 0;

    _searchMode		= opt.searchMode.value();
    _testMode		= opt.testMode.value();
    _updateMode		= opt.updateMode.value();
    _summaryMode	= opt.summaryMode.value();
    _instSourcesMode	= opt.instSourcesMode.value();

    if ( _youMode )	y2milestone( "YOU mode" );
    if ( _updateMode )	y2milestone( "Update mode" );


    basicLayout();
    addMenus();		// Only after all widgets are created!
    makeConnections();
    emit loadData();

    if ( _pkgList )
	_pkgList->clear();

    if ( _patchFilterView && _youMode )
    {
	if ( _filters && _patchFilterView && _patchList )
	{
	    _filters->showPage( _patchFilterView );
	    _patchList->filter();
	}
    }
    else if ( _instSrcFilterView && _instSourcesMode )
    {
	if ( YQPkgInstSrcList::countEnabledSources() > 1 )
	{
	    _filters->showPage( _instSrcFilterView );
	    _instSrcFilterView->filter();
	}
	else if ( _searchFilterView )
	{
	    y2milestone( "No multiple inst sources - falling back to search mode" );
	    _filters->showPage( _searchFilterView );
	    _searchFilterView->filter();
	    QTimer::singleShot( 0, _searchFilterView, SLOT( setFocus() ) );
	}
    }
    else if ( _updateProblemFilterView )
    {
	_filters->showPage( _updateProblemFilterView );
	_updateProblemFilterView->filter();

    }
    else if ( _searchMode && _searchFilterView )
    {
	_filters->showPage( _searchFilterView );
	_searchFilterView->filter();
	QTimer::singleShot( 0, _searchFilterView, SLOT( setFocus() ) );
    }
    else if ( _summaryMode && _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->filter();
    }
    else if ( _patternList )
    {
	_filters->showPage( _patternList );
	_patternList->filter();
    }
    else if ( _selList )
    {
	_filters->showPage( _selList );
	_selList->filter();
    }


    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();

    y2milestone( "PackageSelector init done" );


#if CHECK_DEPENDENCIES_ON_STARTUP

    if ( ! _testMode && ! _youMode )
    {
	// Fire up the first dependency check in the main loop.
	// Don't do this right away - wait until all initializations are finished.
	QTimer::singleShot( 0, this, SLOT( resolvePackageDependencies() ) );
    }
#endif
}


void
YQPackageSelector::basicLayout()
{
    layoutMenuBar( this );

    QSplitter * outer_splitter = new QSplitter( QSplitter::Horizontal, this );
    CHECK_PTR( outer_splitter );

    QWidget * left_pane	 = layoutLeftPane ( outer_splitter );
    QWidget * right_pane = layoutRightPane( outer_splitter );

    outer_splitter->setResizeMode( left_pane,  QSplitter::FollowSizeHint );
    outer_splitter->setResizeMode( right_pane, QSplitter::Stretch );
}


QWidget *
YQPackageSelector::layoutLeftPane( QWidget * parent )
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, parent );
    CHECK_PTR( splitter );
    splitter->setMargin( MARGIN );

    QVBox * upper_vbox = new QVBox( splitter );
    CHECK_PTR( upper_vbox );
    layoutFilters( upper_vbox );
    addVSpacing( upper_vbox, MARGIN );

    QVBox * lower_vbox = new QVBox( splitter );
    addVSpacing( lower_vbox, MARGIN );
    _diskUsageList = new YQPkgDiskUsageList( lower_vbox );
    CHECK_PTR( _diskUsageList );

    splitter->setResizeMode( upper_vbox, QSplitter::Stretch );
    splitter->setResizeMode( lower_vbox, QSplitter::FollowSizeHint );

    return splitter;
}


void
YQPackageSelector::layoutFilters( QWidget * parent )
{
    _filters = new QY2ComboTabWidget( _( "Fi&lter:" ), parent );
    CHECK_PTR( _filters );


    //
    // Update problem view
    //

    if ( _updateMode )
    {
	if ( YQPkgUpdateProblemFilterView::haveProblematicPackages()
	     || _testMode )
	{
	    _updateProblemFilterView = new YQPkgUpdateProblemFilterView( parent );
	    CHECK_PTR( _updateProblemFilterView );
	    _filters->addPage( _( "Update Problems" ), _updateProblemFilterView );
	}
    }


    //
    // Patches view
    //

    if ( _youMode
#if ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE
	 || ! zyppPool().empty<zypp::Patch>()
#endif
	 )
	addPatchFilterView();


    //
    // Patterns view
    //

    if ( ! zyppPool().empty<zypp::Pattern>() || _testMode )
    {
	_patternList = new YQPkgPatternList( parent, true );
	CHECK_PTR( _patternList );
	_filters->addPage( _( "Patterns" ), _patternList );

	connect( _patternList,		SIGNAL( statusChanged()			),
		 this,			SLOT  ( autoResolveDependencies()	) );

	connect( _pkgConflictDialog,	SIGNAL( updatePackages()		),
		 _patternList,		SLOT  ( updateItemStates()		) );

	connect( this,			SIGNAL( refresh()			),
		 _patternList,		SLOT  ( updateItemStates()		) );
    }


    //
    // Selections view
    //

    if ( ! zyppPool().empty<zypp::Selection>() || _testMode )
    {

	_selList = new YQPkgSelList( parent, true );
	CHECK_PTR( _selList );
	_filters->addPage( _( "Selections" ), _selList );

	connect( _selList,		SIGNAL( statusChanged()			),
		 this,			SLOT  ( autoResolveDependencies()	) );

	connect( _pkgConflictDialog,	SIGNAL( updatePackages()		),
		 _selList,		SLOT  ( updateItemStates()		) );

	connect( this,			SIGNAL( refresh()			),
		 _selList,		 SLOT  ( updateItemStates()		) );
    }


    //
    // RPM group tags view
    //

    _rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( parent );
    CHECK_PTR( _rpmGroupTagsFilterView );
    _filters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

    connect( this,			SIGNAL( loadData() ),
	     _rpmGroupTagsFilterView,	SLOT  ( filter()   ) );


    //
    // Languages view
    //

    _langList = new YQPkgLangList( parent );
    CHECK_PTR( _langList );

    _filters->addPage( _( "Languages" ), _langList );
    _langList->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored ) ); // hor/vert

    connect( _langList,		SIGNAL( statusChanged()			),
	     this,		SLOT  ( autoResolveDependencies()	) );

    connect( this,		SIGNAL( refresh()			),
	     _langList,		SLOT  ( updateItemStates()		) );


    //
    // Inst source view
    //

    _instSrcFilterView = new YQPkgInstSrcFilterView( parent );
    CHECK_PTR( _instSrcFilterView );
    _filters->addPage( _( "Installation Sources" ), _instSrcFilterView );


    //
    // Package search view
    //

    _searchFilterView = new YQPkgSearchFilterView( parent );
    CHECK_PTR( _searchFilterView );
    _filters->addPage( _( "Search" ), _searchFilterView );


    //
    // Status change view
    //

    _statusFilterView = new YQPkgStatusFilterView( parent );
    CHECK_PTR( _statusFilterView );
    _filters->addPage( _( "Installation Summary" ), _statusFilterView );


#if 0
    // DEBUG

    _filters->addPage( _( "Keywords"   ), new QLabel( "Keywords\nfilter\n\nfor future use", 0 ) );
    _filters->addPage( _( "MIME Types" ), new QLabel( "MIME Types\nfilter\n\nfor future use" , 0 ) );
#endif

}


QWidget *
YQPackageSelector::layoutRightPane( QWidget * parent )
{
    QVBox * right_pane_vbox = new QVBox( parent );
    CHECK_PTR( right_pane_vbox );
    right_pane_vbox->setMargin( MARGIN );

    QSplitter * splitter = new QSplitter( QSplitter::Vertical, right_pane_vbox );
    CHECK_PTR( splitter );

    QVBox * splitter_vbox = new QVBox( splitter );
    CHECK_PTR( splitter_vbox );
    layoutPkgList( splitter_vbox );
    addVSpacing( splitter_vbox, MARGIN );

    layoutDetailsViews( splitter );

    layoutButtons( right_pane_vbox );

    return right_pane_vbox;
}


void
YQPackageSelector::layoutPkgList( QWidget * parent )
{
    _pkgList= new YQPkgList( parent );
    CHECK_PTR( _pkgList );

    connect( _pkgList,	SIGNAL( statusChanged()		  ),
	     this,	SLOT  ( autoResolveDependencies() ) );
}


void
YQPackageSelector::layoutDetailsViews( QWidget * parent )
{
    bool haveInstalledPkgs = YQPkgList::haveInstalledPkgs();

    QVBox * details_vbox = new QVBox( parent );
    CHECK_PTR( details_vbox );
    details_vbox->setMinimumSize( 0, 0 );

    addVSpacing( details_vbox, 8 );

    _detailsViews = new QTabWidget( details_vbox );
    CHECK_PTR( _detailsViews );
    _detailsViews->setMargin( MARGIN );

    // _detailsViews->setTabPosition( QTabWidget::Bottom );


    //
    // Description
    //

    _pkgDescriptionView = new YQPkgDescriptionView( _detailsViews );
    CHECK_PTR( _pkgDescriptionView );

    _detailsViews->addTab( _pkgDescriptionView, _( "D&escription" ) );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,			SIGNAL( selectionChanged    ( ZyppSel ) ),
	     _pkgDescriptionView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    //
    // Technical details
    //

    _pkgTechnicalDetailsView = new YQPkgTechnicalDetailsView( _detailsViews );
    CHECK_PTR( _pkgTechnicalDetailsView );

    _detailsViews->addTab( _pkgTechnicalDetailsView, _( "&Technical Data" ) );

    connect( _pkgList,			SIGNAL( selectionChanged    ( ZyppSel ) ),
	     _pkgTechnicalDetailsView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    //
    // Dependencies
    //

    _pkgDependenciesView = new YQPkgDependenciesView( _detailsViews );
    CHECK_PTR( _pkgDependenciesView );

    _detailsViews->addTab( _pkgDependenciesView, _( "Dependencies" ) );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,			SIGNAL( selectionChanged    ( ZyppSel ) ),
	     _pkgDependenciesView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );



    //
    // Versions
    //

    _pkgVersionsView = new YQPkgVersionsView( _detailsViews,
					      true );	// userCanSwitchVersions
    CHECK_PTR( _pkgVersionsView );

    _detailsViews->addTab( _pkgVersionsView, _( "&Versions" ) );

    connect( _pkgList,		SIGNAL( selectionChanged    ( ZyppSel ) ),
	     _pkgVersionsView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    //
    // File List
    //

    if ( haveInstalledPkgs )	// file list information is only available for installed pkgs
    {
	_pkgFileListView = new YQPkgFileListView( _detailsViews );
	CHECK_PTR( _pkgFileListView );

	_detailsViews->addTab( _pkgFileListView, _( "File List" ) );
	_detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

	connect( _pkgList,		SIGNAL( selectionChanged    ( ZyppSel ) ),
		 _pkgFileListView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );
    }


    //
    // Change Log
    //

    if ( haveInstalledPkgs )	// change log information is only available for installed pkgs
    {
	_pkgChangeLogView = new YQPkgChangeLogView( _detailsViews );
	CHECK_PTR( _pkgChangeLogView );

	_detailsViews->addTab( _pkgChangeLogView, _( "Change Log" ) );
	_detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

	connect( _pkgList,		SIGNAL( selectionChanged    ( ZyppSel ) ),
		 _pkgChangeLogView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );
    }
}


void
YQPackageSelector::layoutButtons( QWidget * parent )
{
    QHBox * button_box = new QHBox( parent );
    CHECK_PTR( button_box );
    button_box->setSpacing( SPACING );

    // Button: Dependency check
    // Translators: Please keep this short!
    _checkDependenciesButton = new QPushButton( _( "Chec&k" ), button_box );
    CHECK_PTR( _checkDependenciesButton );
    _checkDependenciesButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert
    _normalButtonBackground = _checkDependenciesButton->paletteBackgroundColor();

    connect( _checkDependenciesButton,	SIGNAL( clicked() ),
	     this,			SLOT  ( manualResolvePackageDependencies() ) );


    // Checkbox: Automatically check dependencies for every package status change?
    // Translators: Please keep this short!
    _autoDependenciesCheckBox = new QCheckBox( _( "A&utocheck" ), button_box );
    CHECK_PTR( _autoDependenciesCheckBox );
    _autoDependenciesCheckBox->setChecked( AUTO_CHECK_DEPENDENCIES_DEFAULT );

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
YQPackageSelector::layoutMenuBar( QWidget * parent )
{
    _menuBar = new QMenuBar( parent );
    CHECK_PTR( _menuBar );

    _fileMenu		= 0;
    _viewMenu		= 0;
    _pkgMenu		= 0;
    _patchMenu		= 0;
    _extrasMenu		= 0;
    _helpMenu		= 0;

}


void
YQPackageSelector::addMenus()
{
    //
    // File menu
    //

    _fileMenu = new QPopupMenu( _menuBar );
    CHECK_PTR( _fileMenu );
    _menuBar->insertItem( _( "&File" ), _fileMenu );

    _fileMenu->insertItem( _( "&Import..." ),	this, SLOT( pkgImport() ) );
    _fileMenu->insertItem( _( "&Export..." ),	this, SLOT( pkgExport() ) );

    _fileMenu->insertSeparator();

    _fileMenu->insertItem( _( "E&xit -- Discard Changes" ), this, SLOT( reject() ) );
    _fileMenu->insertItem( _( "&Quit -- Save Changes"	 ), this, SLOT( accept() ) );


    if ( _pkgList )
    {
	//
	// Package menu
	//

	_pkgMenu = new QPopupMenu( _menuBar );
	CHECK_PTR( _pkgMenu );
	_menuBar->insertItem( _( "&Package" ), _pkgMenu );

	_pkgList->actionSetCurrentInstall->addTo( _pkgMenu );
	_pkgList->actionSetCurrentDontInstall->addTo( _pkgMenu );
	_pkgList->actionSetCurrentKeepInstalled->addTo( _pkgMenu );
	_pkgList->actionSetCurrentDelete->addTo( _pkgMenu );
	_pkgList->actionSetCurrentUpdate->addTo( _pkgMenu );
	_pkgList->actionSetCurrentTaboo->addTo( _pkgMenu );
	_pkgList->actionSetCurrentProtected->addTo( _pkgMenu );

	_pkgMenu->insertSeparator();

	_pkgList->actionInstallSourceRpm->addTo( _pkgMenu );
	_pkgList->actionDontInstallSourceRpm->addTo( _pkgMenu );

	_pkgMenu->insertSeparator();
	QPopupMenu * submenu = _pkgList->addAllInListSubMenu( _pkgMenu );
	CHECK_PTR( submenu );

	submenu->insertSeparator();
	_pkgList->actionInstallListSourceRpms->addTo( submenu );
	_pkgList->actionDontInstallListSourceRpms->addTo( submenu );


	//
	// Submenu for all packages
	//

	submenu = new QPopupMenu( _pkgMenu );
	CHECK_PTR( submenu );

	// Translators: Unlike the "all in this list" submenu, this submenu
	// refers to all packages globally, not only to those that are
	// currently visible in the packages list.
	_pkgMenu->insertItem( _( "All Packages" ), submenu );

	submenu->insertItem( _( "Update if newer version available" ),
			     this, SLOT( globalUpdatePkg() ) );

	submenu->insertItem( _( "Update unconditionally" ),
			     this, SLOT( globalUpdatePkgForce() ) );
    }


    if ( _patchList )
    {
	//
	// Patch menu
	//

	_patchMenu = new QPopupMenu( _menuBar );
	CHECK_PTR( _patchMenu );
	_menuBar->insertItem( _( "&Patch" ), _patchMenu );

	_patchList->actionSetCurrentInstall->addTo( _patchMenu );
	_patchList->actionSetCurrentDontInstall->addTo( _patchMenu );
	_patchList->actionSetCurrentKeepInstalled->addTo( _patchMenu );
#if ENABLE_DELETING_PATCHES
	_patchList->actionSetCurrentDelete->addTo( _patchMenu );
#endif
	_patchList->actionSetCurrentUpdate->addTo( _patchMenu );
	_patchList->actionSetCurrentTaboo->addTo( _patchMenu );

	_patchMenu->insertSeparator();
	_patchList->addAllInListSubMenu( _patchMenu );
    }


    //
    // Extras menu
    //

    _extrasMenu = new QPopupMenu( _menuBar );
    CHECK_PTR( _extrasMenu );
    _menuBar->insertItem( _( "&Extras" ), _extrasMenu );

    _extrasMenu->insertItem( _( "Show &Products" 		  ), this, SLOT( showProducts()    ) );
    _extrasMenu->insertItem( _( "Show &Automatic Package Changes" ), this, SLOT( showAutoPkgList() ), CTRL + Key_A );
    _extrasMenu->insertItem( _( "&Verify System"                  ), this, SLOT( verifySystem()    ) );

    _extrasMenu->insertSeparator();

    // Translators: This is about packages ending in "-devel", so don't translate that "-devel"!
    _extrasMenu->insertItem( _( "Install All Matching -&devel Packages" ), this, SLOT( installDevelPkgs() ) );

    // Translators: This is about packages ending in "-debuginfo", so don't translate that "-debuginfo"!
    _extrasMenu->insertItem( _( "Install All Matching -de&buginfo Packages" ), this, SLOT( installDebugInfoPkgs() ) );

    _extrasMenu->insertSeparator();

    if ( _pkgConflictDialog )
	_extrasMenu->insertItem( _( "Generate Dependency Resolver &Test Case" ),
				    _pkgConflictDialog, SLOT( askCreateSolverTestCase() ) );

    if ( _actionResetIgnoredDependencyProblems )
	_actionResetIgnoredDependencyProblems->addTo( _extrasMenu );


#ifdef FIXME
    if ( _patchList )
	_patchList->actionShowRawPatchInfo->addTo( _extrasMenu );
#endif


    //
    // Help menu
    //

    _helpMenu = new QPopupMenu( _menuBar );
    CHECK_PTR( _helpMenu );
    _menuBar->insertSeparator();
    _menuBar->insertItem( _( "&Help" ), _helpMenu );

    // Note: The help functions and their texts are moved out
    // to a separate source file YQPackageSelectorHelp.cc

    // Menu entry for help overview
    _helpMenu->insertItem( _( "&Overview" ), this, SLOT( help()		), Key_F1	  );

    // Menu entry for help about used symbols ( icons )
    _helpMenu->insertItem( _( "&Symbols" ), this, SLOT( symbolHelp()	), SHIFT + Key_F1 );

    // Menu entry for keyboard help
    _helpMenu->insertItem( _( "&Keys" ), this, SLOT( keyboardHelp() )		      );
}


void
YQPackageSelector::connectFilter( QWidget * filter,
				  QWidget * pkgList,
				  bool hasUpdateSignal )
{
    if ( ! filter  )	return;
    if ( ! pkgList )	return;

    if ( _filters )
    {
	connect( _filters,	SIGNAL( currentChanged(QWidget *) ),
		 filter,	SLOT  ( filterIfVisible()	     ) );
    }

    connect( this,	SIGNAL( refresh()	  ),
	     filter,	SLOT  ( filterIfVisible() ) );

    connect( filter,	SIGNAL( filterStart()	),
	     pkgList,	SLOT  ( clear()		) );

    connect( filter,	SIGNAL( filterMatch( ZyppSel, ZyppPkg ) ),
	     pkgList,	SLOT  ( addPkgItem ( ZyppSel, ZyppPkg ) ) );

    connect( filter,	SIGNAL( filterFinished()  ),
	     pkgList,	SLOT  ( selectSomething() ) );


    if ( hasUpdateSignal )
    {
	connect( filter,		SIGNAL( updatePackages()   ),
		 pkgList,		SLOT  ( updateItemStates() ) );

	if ( _diskUsageList )
	{
	    connect( filter,		SIGNAL( updatePackages()  ),
		     _diskUsageList,	SLOT  ( updateDiskUsage() ) );
	}
    }
}


void
YQPackageSelector::makeConnections()
{
    connect( this, SIGNAL( resolvingStarted()	),
	     this, SLOT	 ( animateCheckButton() ) );

    connect( this, SIGNAL( resolvingFinished()	),
	     this, SLOT	 ( restoreCheckButton() ) );

    connectFilter( _updateProblemFilterView,	_pkgList, false );
    connectFilter( _patternList,		_pkgList );
    connectFilter( _selList,			_pkgList );
    connectFilter( _instSrcFilterView,		_pkgList, false );
    connectFilter( _rpmGroupTagsFilterView,	_pkgList, false );
    connectFilter( _langList,			_pkgList );
    connectFilter( _statusFilterView,		_pkgList, false );
    connectFilter( _searchFilterView,		_pkgList, false );

    if ( _searchFilterView && _pkgList )
    {
	connect( _searchFilterView,	SIGNAL( message( const QString & ) ),
		 _pkgList,		SLOT  ( message( const QString & ) ) );
    }

    if ( _instSrcFilterView && _pkgList )
    {
	connect( _instSrcFilterView,	SIGNAL( filterNearMatch	 ( ZyppSel, ZyppPkg ) ),
		 _pkgList,		SLOT  ( addPkgItemDimmed ( ZyppSel, ZyppPkg ) ) );
    }

    if ( _pkgList && _diskUsageList )
    {

	connect( _pkgList,		SIGNAL( statusChanged()	  ),
		 _diskUsageList,	SLOT  ( updateDiskUsage() ) );
    }

    connectPatchList();


    //
    // Connect package conflict dialog
    //

    if ( _pkgConflictDialog )
    {
	if (_pkgList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _pkgList,			SLOT  ( updateItemStates() ) );
	}

	if ( _patternList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _patternList,		SLOT  ( updateItemStates() ) );
	}

	if ( _selList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _selList,			SLOT  ( updateItemStates() ) );
	}

	if ( _diskUsageList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _diskUsageList,		SLOT  ( updateDiskUsage()  ) );
	}
    }


    //
    // Connect package versions view
    //

    if ( _pkgVersionsView && _pkgList )
    {
	connect( _pkgVersionsView,	SIGNAL( candidateChanged( ZyppObj ) ),
		 _pkgList,		SLOT  ( updateItemData()    ) );
    }


    //
    // Hotkey to enable "patches" filter view on the fly
    //

    QAccel * accel = new QAccel( this );
    CHECK_PTR( accel );
    accel->connectItem( accel->insertItem( Key_F2 ),
			this, SLOT( hotkeyInsertPatchFilterView() ) );


    //
    // Update actions just before opening menus
    //

    if ( _pkgMenu && _pkgList )
    {
	connect( _pkgMenu,	SIGNAL( aboutToShow()	),
		 _pkgList,	SLOT  ( updateActions() ) );
    }

    if ( _patchMenu && _patchList )
    {
	connect( _patchMenu,	SIGNAL( aboutToShow()	),
		 _patchList,	SLOT  ( updateActions() ) );
    }
}


void
YQPackageSelector::animateCheckButton()
{
    if ( _checkDependenciesButton )
    {
	_checkDependenciesButton->setPaletteBackgroundColor( QColor( 0xE0, 0xE0, 0xF8 ) );
	_checkDependenciesButton->repaint();
    }
}


void
YQPackageSelector::restoreCheckButton()
{
    if ( _checkDependenciesButton )
	_checkDependenciesButton->setPaletteBackgroundColor( _normalButtonBackground );
}


void
YQPackageSelector::autoResolveDependencies()
{
    if ( _autoDependenciesCheckBox && ! _autoDependenciesCheckBox->isChecked() )
	return;

    resolvePackageDependencies();
}


int
YQPackageSelector::manualResolvePackageDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	y2error( "No package conflict dialog existing" );
	return QDialog::Accepted;
    }

    YQUI::ui()->busyCursor();
    int result = _pkgConflictDialog->solveAndShowConflicts();
    YQUI::ui()->normalCursor();

#if DEPENDENCY_FEEDBACK_IF_OK

    if ( result == QDialog::Accepted )
    {
	QMessageBox::information( this, "",
				  _( "All package dependencies are OK." ),
				  QMessageBox::Ok );
    }
#endif

    return result;
}


void
YQPackageSelector::addPatchFilterView()
{
    if ( ! _patchFilterView )
    {
	_patchFilterView = new YQPkgPatchFilterView( this );
	CHECK_PTR( _patchFilterView );
	_filters->addPage( _( "Patches" ), _patchFilterView );

	_patchList = _patchFilterView->patchList();
	CHECK_PTR( _patchList );

	connectPatchList();
    }
}


void
YQPackageSelector::hotkeyInsertPatchFilterView()
{
    if ( ! _patchFilterView )
    {
	y2milestone( "Activating patches filter view" );

	addPatchFilterView();
	connectPatchList();

	_filters->showPage( _patchFilterView );
	_pkgList->clear();
	_patchList->filter();
    }
}


void
YQPackageSelector::connectPatchList()
{
    if ( _pkgList && _patchList )
    {
	connectFilter( _patchList, _pkgList );

	connect( _patchList, SIGNAL( filterMatch   ( const QString &, const QString &, FSize ) ),
		 _pkgList,   SLOT  ( addPassiveItem( const QString &, const QString &, FSize ) ) );

	connect( _patchList,		SIGNAL( statusChanged()			),
		 this,			SLOT  ( autoResolveDependencies()	) );

	if ( _pkgConflictDialog )
	{
	    connect( _pkgConflictDialog,SIGNAL( updatePackages()		),
		     _patchList,	SLOT  ( updateItemStates()		) );
	}

	connect( this,			SIGNAL( refresh()			),
		 _patchList,		SLOT  ( updateItemStates()		) );

    }
}


void
YQPackageSelector::pkgExport()
{
    QString filename = YQUI::ui()->askForSaveFileName( QString( DEFAULT_EXPORT_FILE_NAME ),	// startsWith
						       QString( "*.xml;;*" ),			// filter
						       _( "Save Package List" ) );

    if ( ! filename.isEmpty() )
    {
	zypp::syscontent::Writer writer;
	const zypp::ResPool & pool = zypp::getZYpp()->pool();

	// The ZYPP obfuscated C++ contest proudly presents:

	for_each( pool.begin(), pool.end(),
		  boost::bind( &zypp::syscontent::Writer::addIf,
			       boost::ref( writer ),
			       _1 ) );
	// Yuck. What a mess.
	//
	// Does anybody seriously believe this kind of thing is easier to read,
	// let alone use? Get real. This is an argument in favour of all C++
	// haters. And it's one that is really hard to counter.
	//
	// -sh 2006-12-13

	try
	{
	    std::ofstream exportFile( fromUTF8( filename ) );
	    exportFile.exceptions( std::ios_base::badbit | std::ios_base::failbit );
	    exportFile << writer;

	    y2milestone( "Package list exported to %s", (const char *) filename );
	}
	catch ( std::exception & exception )
	{
	    y2warning( "Error exporting package list to %s", (const char *) filename );

	    // The export might have left over a partially written file.
	    // Try to delete it. Don't care if it doesn't exist and unlink() fails.
	    (void) unlink( (const char *) filename );

	    // Post error popup
	    QMessageBox::warning( this,						// parent
				  _( "Error" ),					// caption
				  _( "Error exporting package list to %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }
}


void
YQPackageSelector::pkgImport()
{
    QString filename =	QFileDialog::getOpenFileName( DEFAULT_EXPORT_FILE_NAME,		// startsWith
						      "*.xml+;;*",			// filter
						      this,				// parent
						      0,				// name
						      _( "Load Package List" ) );	// caption

    if ( ! filename.isEmpty() )
    {
	y2milestone( "Importing package list from %s", (const char *) filename );

	try
	{
	    std::ifstream importFile( fromUTF8( filename ) );
	    zypp::syscontent::Reader reader( importFile );

	    //
	    // Put reader contents into maps
	    //

	    typedef zypp::syscontent::Reader::Entry	ZyppReaderEntry;
	    typedef std::pair<string, ZyppReaderEntry>	ImportMapPair;

	    map<string, ZyppReaderEntry> importPkg;
	    map<string, ZyppReaderEntry> importPatterns;

	    for ( zypp::syscontent::Reader::const_iterator it = reader.begin();
		  it != reader.end();
		  ++ it )
	    {
		string kind = it->kind();

		if      ( kind == "package" ) 	importPkg.insert     ( ImportMapPair( it->name(), *it ) );
		else if ( kind == "pattern" )	importPatterns.insert( ImportMapPair( it->name(), *it ) );
	    }

	    y2debug( "Found %u packages and %u patterns in %s",
		     importPkg.size(),
		     importPatterns.size(),
		     (const char *) filename );


	    //
	    // Set status of all patterns and packages according to import map
	    //

	    for ( ZyppPoolIterator it = zyppPatternsBegin();
		  it != zyppPatternsEnd();
		  ++it )
	    {
		ZyppSel selectable = *it;
		importSelectable( *it, importPatterns.find( selectable->name() ) != importPatterns.end(), "pattern" );
	    }

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		ZyppSel selectable = *it;
		importSelectable( *it, importPkg.find( selectable->name() ) != importPkg.end(), "package" );
	    }


	    //
	    // Display result
	    //

	    emit refresh();

	    if ( _statusFilterView )
	    {
		// Switch to "Installation Summary" filter view

		_filters->showPage( _statusFilterView );
		_statusFilterView->filter();
	    }

	}
	catch ( const zypp::Exception & exception )
	{
	    y2warning( "Error reading package list from %s", (const char *) filename );

	    // Post error popup
	    QMessageBox::warning( this,						// parent
				  _( "Error" ),					// caption
				  _( "Error loading package list from %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }
}


void
YQPackageSelector::importSelectable( ZyppSel		selectable,
				     bool		isWanted,
				     const char * 	kind )
{
    ZyppStatus oldStatus = selectable->status();
    ZyppStatus newStatus = oldStatus;

    if ( isWanted )
    {
	//
	// Make sure this selectable does not get installed
	//

	switch ( oldStatus )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_KeepInstalled:
	    case S_Protected:
	    case S_Update:
	    case S_AutoUpdate:
		newStatus = oldStatus;
		break;

	    case S_Del:
	    case S_AutoDel:
		newStatus = S_KeepInstalled;
		y2debug( "Keeping %s %s", kind, selectable->name().c_str() );
		break;

	    case S_NoInst:
	    case S_Taboo:

		if ( selectable->hasCandidateObj() )
		{
		    newStatus = S_Install;
		    y2debug( "Adding %s %s", kind, selectable->name().c_str() );
		}
		else
		{
		    y2debug( "Can't add %s %s: No candidate",
			     kind, selectable->name().c_str() );
		}
		break;
	}
    }
    else // ! isWanted
    {
	//
	// Make sure this selectable does not get installed
	//

	switch ( oldStatus )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_KeepInstalled:
	    case S_Protected:
	    case S_Update:
	    case S_AutoUpdate:
		newStatus = S_Del;
		y2debug( "Deleting %s %s", kind, selectable->name().c_str() );
		break;

	    case S_Del:
	    case S_AutoDel:
	    case S_NoInst:
	    case S_Taboo:
		newStatus = oldStatus;
		break;
	}
    }

    if ( oldStatus != newStatus )
	selectable->set_status( newStatus );
}


void
YQPackageSelector::globalUpdatePkg( bool force )
{
    if ( ! _pkgList )
	return;

    int count = _pkgList->globalSetPkgStatus( S_Update, force,
					      true ); // countOnly
    y2milestone( "%d pkgs found for update", count );

    if ( count >= GLOBAL_UPDATE_CONFIRMATION_THRESHOLD )
    {
	if ( QMessageBox::question( this, "",	// caption
				    // Translators: %1 is the number of affected packages
				    _( "%1 packages will be updated" ).arg( count ),
				    _( "&Continue" ), _( "C&ancel" ),
				    0,		// defaultButtonNumber (from 0)
				    1 )		// escapeButtonNumber
	     == 1 )	// "Cancel"?
	{
	    return;
	}
    }

    (void) _pkgList->globalSetPkgStatus( S_Update, force,
					 false ); // countOnly

    if ( _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->clear();
	_statusFilterView->showTransactions();
	_statusFilterView->filter();
    }
}


void
YQPackageSelector::showProducts()
{
    YQPkgProductDialog::showProductDialog();
}

void
YQPackageSelector::installDevelPkgs()
{
    installSubPkgs( "-devel" );
}


void
YQPackageSelector::installDebugInfoPkgs()
{
    installSubPkgs( "-debuginfo" );
}


void
YQPackageSelector::installSubPkgs( const QString suffix )
{
    // Find all matching packages and put them into a QMap

    QMap<QString, ZyppSel> subPkgs;

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	QString name = (*it)->name().c_str();

	if ( name.endsWith( suffix ) )
	{
	    subPkgs[ name ] = *it;

	    y2debug( "Found subpackage: %s", (const char *) name );
	}
    }


    // Now go through all packages and look if there is a corresponding subpackage in the QMap

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	QString name = (*it)->name().c_str();

	if ( subPkgs.contains( name + suffix ) )
	{
	    ZyppSel subPkg = subPkgs[ name + suffix ];
	    QString subPkgName;

	    switch ( (*it)->status() )
	    {
		case S_AutoDel:
		case S_NoInst:
		case S_Protected:
		case S_Taboo:
		case S_Del:
		    // Don't install the subpackage
		    y2milestone( "Ignoring unwanted subpackage %s", (const char *) subPkgName );
		    break;

		case S_AutoInstall:
		case S_Install:
		case S_KeepInstalled:

		    // Install the subpackage, but don't try to update it

		    if ( ! subPkg->installedObj() )
		    {
			subPkg->set_status( S_Install );
			y2milestone( "Installing subpackage %s", (const char *) subPkgName );
		    }
		    break;


		case S_Update:
		case S_AutoUpdate:

		    // Install or update the subpackage

		    if ( ! subPkg->installedObj() )
		    {
			subPkg->set_status( S_Install );
			y2milestone( "Installing subpackage %s", (const char *) subPkgName );
		    }
		    else
		    {
			subPkg->set_status( S_Update );
			y2milestone( "Updating subpackage %s", (const char *) subPkgName );
		    }
		    break;

		    // Intentionally omitting 'default' branch so the compiler can
		    // catch unhandled enum states
	    }
	}
    }


    if ( _filters && _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->filter();
    }

    YQPkgChangesDialog::showChangesDialog( _( "Added Subpackages:" ),
					   QRegExp( ".*" + suffix + "$" ),
					   _( "&OK" ),
					   QString::null,	// rejectButtonLabel
					   true );		// showIfEmpty
}


#include "YQPackageSelector.moc"
