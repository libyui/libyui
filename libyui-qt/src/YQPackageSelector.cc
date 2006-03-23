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
#define ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE	1

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

#include "QY2FloppyMountDialog.h"
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
#include "YQPkgLangList.h"
#include "YQPkgList.h"
#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQPkgPatternList.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgSelList.h"
#include "YQPkgStatusFilterView.h"
#include "YQPkgTechnicalDetailsView.h"
#include "YQPkgTextDialog.h"
#include "YQPkgUpdateProblemFilterView.h"
#include "YQPkgVersionsView.h"

#include "QY2ComboTabWidget.h"
#include "YQDialog.h"
#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;

#define SPACING			6
#define MARGIN			4



YQPackageSelector::YQPackageSelector( QWidget * 		parent,
				      const YWidgetOpt & 	opt,
				      const YCPString & 	floppyDevice )
    : YQPackageSelectorBase( parent, opt )
    , _floppyDevice( floppyDevice->value().c_str() )
{
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

    _searchMode	 	= opt.searchMode.value();
    _testMode	 	= opt.testMode.value();
    _updateMode	 	= opt.updateMode.value();
    _summaryMode 	= opt.summaryMode.value();
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
	_filters->showPage( _instSrcFilterView );
	_instSrcFilterView->filter();
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

    y2debug( "Floppy device: %s", (const char *) _floppyDevice );
    y2milestone( "PackageSelector init done" );


#if CHECK_DEPENDENCIES_ON_STARTUP

    if ( ! _testMode )
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

    QWidget * left_pane  = layoutLeftPane ( outer_splitter );
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

    QVBox * vbox = new QVBox( splitter );
    CHECK_PTR( vbox );
    layoutFilters( vbox );
    addVSpacing( vbox, MARGIN );

    vbox = new QVBox( splitter );
    addVSpacing( vbox, MARGIN );
    _diskUsageList = new YQPkgDiskUsageList( vbox );
    CHECK_PTR( _diskUsageList );

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
#ifdef FIXME
	if ( ! Y2PM::packageManager().updateEmpty()
	     || _testMode )
	{
	    _updateProblemFilterView = new YQPkgUpdateProblemFilterView( parent );
	    CHECK_PTR( _updateProblemFilterView );
	    _filters->addPage( _( "Update Problems" ), _updateProblemFilterView );
	}
#endif
    }


    //
    // Patches view
    //

    if ( _youMode
#if ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE
	 || ! zyppPool().empty<zypp::Patch>()
#endif
	 )
	addPatchFilterView( false ); // don't make this the active filter


    //
    // Patterns view
    //

    if ( ! zyppPool().empty<zypp::Pattern>() || _testMode )
    {
	_patternList = new YQPkgPatternList( parent, true );
	CHECK_PTR( _patternList );
	_filters->addPage( _( "Patterns" ), _patternList );

	connect( _patternList, 	 	SIGNAL( statusChanged()	        	),
		 this,		 	SLOT  ( autoResolveDependencies() 	) );

	connect( _pkgConflictDialog,	SIGNAL( updatePackages()      		),
		 _patternList,		SLOT  ( updateItemStates() 		) );

	connect( this,			SIGNAL( refresh()			),
		 _patternList, 		SLOT  ( updateItemStates() 		) );
    }


    //
    // Selections view
    //

    if ( ! zyppPool().empty<zypp::Selection>() || _testMode )
    {

	_selList = new YQPkgSelList( parent, true );
	CHECK_PTR( _selList );
	_filters->addPage( _( "Selections" ), _selList );

	connect( _selList, 	 	SIGNAL( statusChanged()	        	),
		 this,		 	SLOT  ( autoResolveDependencies() 	) );

	connect( _pkgConflictDialog,	SIGNAL( updatePackages()      		),
		 _selList,		 SLOT  ( updateItemStates() 		) );

	connect( this,		 SIGNAL( refresh()				),
		 _selList, 		 SLOT  ( updateItemStates() 		) );
    }


    //
    // RPM group tags view
    //

    _rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( parent );
    CHECK_PTR( _rpmGroupTagsFilterView );
    _filters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

    connect( this,    			SIGNAL( loadData() ),
	     _rpmGroupTagsFilterView,	SLOT  ( filter()   ) );


    //
    // Languages view
    //

    _langList = new YQPkgLangList( parent );
    CHECK_PTR( _langList );
    _filters->addPage( _( "Languages" ), _langList );

    connect( _langList, 	SIGNAL( statusChanged()	               	),
	     this,		SLOT  ( autoResolveDependencies() 	) );

    connect( this,		SIGNAL( refresh()			),
	     _langList, 	SLOT  ( updateItemStates() 		) );


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

    connect( _pkgList, 	SIGNAL( statusChanged()	          ),
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
    _checkDependenciesButton->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    _normalButtonBackground = _checkDependenciesButton->paletteBackgroundColor();

    connect( _checkDependenciesButton,	SIGNAL( clicked() ),
	     this,         		SLOT  ( manualResolvePackageDependencies() ) );


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
	     this,          SLOT  ( reject()   ) );


    QPushButton * accept_button = new QPushButton( _( "&Accept" ), button_box );
    CHECK_PTR( accept_button );
    accept_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( accept_button, SIGNAL( clicked() ),
	     this,          SLOT  ( accept()   ) );

    button_box->setFixedHeight( button_box->sizeHint().height() );
}


void
YQPackageSelector::layoutMenuBar( QWidget * parent )
{
    _menuBar = new QMenuBar( parent );
    CHECK_PTR( _menuBar );

    _fileMenu		= 0;
    _viewMenu		= 0;
    _pkgMenu 		= 0;
    _patchMenu 	= 0;
    _extrasMenu		= 0;
    _helpMenu 		= 0;

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

#ifdef FIXME
    _fileMenu->insertItem( _( "&Import..." ),	this, SLOT( pkgImport() ) );
    _fileMenu->insertItem( _( "&Export..." ),	this, SLOT( pkgExport() ) );

    _fileMenu->insertSeparator();
#endif

    _fileMenu->insertItem( _( "E&xit -- Discard Changes" ), this, SLOT( reject() ) );
    _fileMenu->insertItem( _( "&Quit -- Save Changes"    ), this, SLOT( accept() ) );


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

    _extrasMenu->insertItem( _( "Show &Automatic Package Changes" ), this, SLOT( showAutoPkgList() ), CTRL + Key_A );

#ifdef FIXME
    if ( _patchList )
	_patchList->actionShowRawPatchInfo->addTo( _extrasMenu );
#endif

    // Translators: This is about packages ending in "-devel", so don't translate that "-devel"!
    _extrasMenu->insertItem( _( "Install All Matching -&devel Packages" ), this, SLOT( installDevelPkgs() ) );

    // Translators: This is about packages ending in "-debuginfo", so don't translate that "-debuginfo"!
    _extrasMenu->insertItem( _( "Install All Matching -de&buginfo Packages" ), this, SLOT( installDebugInfoPkgs() ) );


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
    _helpMenu->insertItem( _( "&Overview" ), this, SLOT( help() 	), Key_F1         );

    // Menu entry for help about used symbols ( icons )
    _helpMenu->insertItem( _( "&Symbols" ), this, SLOT( symbolHelp() 	), SHIFT + Key_F1 );

    // Menu entry for keyboard help
    _helpMenu->insertItem( _( "&Keys" ), this, SLOT( keyboardHelp() )                 );
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
		 filter,	SLOT  ( filterIfVisible()            ) );
    }

    connect( this,	SIGNAL( refresh()	  ),
	     filter,	SLOT  ( filterIfVisible() ) );

    connect( filter,	SIGNAL( filterStart() 	),
	     pkgList, 	SLOT  ( clear() 	) );

    connect( filter,	SIGNAL( filterMatch( ZyppSel, ZyppPkg ) ),
	     pkgList, 	SLOT  ( addPkgItem ( ZyppSel, ZyppPkg ) ) );

    connect( filter, 	SIGNAL( filterFinished()  ),
	     pkgList, 	SLOT  ( selectSomething() ) );


    if ( hasUpdateSignal )
    {
	connect( filter, 		SIGNAL( updatePackages()   ),
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
    connect( this, SIGNAL( resolvingStarted()   ),
	     this, SLOT  ( animateCheckButton() ) );

    connect( this, SIGNAL( resolvingFinished()  ),
	     this, SLOT  ( restoreCheckButton() ) );

    connectFilter( _updateProblemFilterView,	_pkgList, false );
    connectFilter( _patternList, 		_pkgList );
    connectFilter( _selList, 			_pkgList );
    connectFilter( _instSrcFilterView,		_pkgList, false );
    connectFilter( _rpmGroupTagsFilterView, 	_pkgList, false );
    connectFilter( _langList, 			_pkgList );
    connectFilter( _statusFilterView, 		_pkgList, false );
    connectFilter( _searchFilterView, 		_pkgList, false );
    connectFilter( _patchList, 			_pkgList );

    if ( _searchFilterView && _pkgList )
    {
	connect( _searchFilterView, 	SIGNAL( message( const QString & ) ),
		 _pkgList,		SLOT  ( message( const QString & ) ) );
    }

    if ( _instSrcFilterView && _pkgList )
    {
	connect( _instSrcFilterView,	SIGNAL( filterNearMatch	 ( ZyppSel, ZyppPkg ) ),
		 _pkgList,		SLOT  ( addPkgItemDimmed ( ZyppSel, ZyppPkg ) ) );
    }

    if ( _pkgList && _diskUsageList )
    {

	connect( _pkgList,		SIGNAL( statusChanged()   ),
		 _diskUsageList,	SLOT  ( updateDiskUsage() ) );
    }

    if ( _pkgList && _patchList )
    {
	connect( _patchList, SIGNAL( filterMatch   ( const QString &, const QString &, FSize ) ),
		 _pkgList,   SLOT  ( addPassiveItem( const QString &, const QString &, FSize ) ) );
    }


    //
    // Connect package conflict dialog
    //

    if ( _pkgConflictDialog )
    {
	if (_pkgList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _pkgList, 			SLOT  ( updateItemStates() ) );
	}

	if ( _patternList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _patternList, 		SLOT  ( updateItemStates() ) );
	}

	if ( _selList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _selList, 			SLOT  ( updateItemStates() ) );
	}

	if ( _diskUsageList )
	{
	    connect( _pkgConflictDialog, 	SIGNAL( updatePackages()   ),
		     _diskUsageList,	 	SLOT  ( updateDiskUsage()  ) );
	}
    }


    //
    // Connect package versions view
    //

    if ( _pkgVersionsView && _pkgList )
    {
	connect( _pkgVersionsView, 	SIGNAL( candidateChanged( ZyppObj ) ),
		 _pkgList,		SLOT  ( updateItemData()    ) );
    }


    //
    // Hotkey to enable "patches" filter view on the fly
    //

    QAccel * accel = new QAccel( this );
    CHECK_PTR( accel );
    accel->connectItem( accel->insertItem( Key_F2 ),
			this, SLOT( addPatchFilterViewAndActivate() ) );


    //
    // Update actions just before opening menus
    //

    if ( _pkgMenu && _pkgList )
    {
	connect( _pkgMenu, 	SIGNAL( aboutToShow()   ),
		 _pkgList, 	SLOT  ( updateActions() ) );
    }

    if ( _patchMenu && _patchList )
    {
	connect( _patchMenu,	SIGNAL( aboutToShow()   ),
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
YQPackageSelector::addPatchFilterView( bool autoActivate )
{
    if ( ! _patchFilterView )
    {
	_patchFilterView = new YQPkgPatchFilterView( this );
	CHECK_PTR( _patchFilterView );
	_filters->addPage( _( "Patches" ), _patchFilterView );

	_patchList = _patchFilterView->patchList();
	CHECK_PTR( _patchList );

	if ( _pkgList && _patchList )
	{
	    connectFilter( _patchList, _pkgList );

	    connect( _patchList, SIGNAL( filterMatch   ( const QString &, const QString &, FSize ) ),
		     _pkgList,   SLOT  ( addPassiveItem( const QString &, const QString &, FSize ) ) );
	}
    }

    if ( autoActivate )
    {
	y2milestone( "Activating patches filter view" );
	_filters->showPage( _patchFilterView );
	_pkgList->clear();
	_patchList->filter();
    }
}


void
YQPackageSelector::pkgExport()
{
    QY2FloppyMountDialog fileDialog( this,				// parent
				      "user.sel",			// startWith
				      "*.sel;;*",			// filter
				      _( "Save Package List" ),		// caption
				      _floppyDevice,			// floppyDevice
				      "/media/floppy",			// floppyMountPoint
				      false );				// startWithFloppy

    QString filename = fileDialog.askForSaveFileName();

#if 0
    QString filename = YQUI::ui()->askForSaveFileName( QString( "user.sel" ),	// startsWith
							   QString( "*.sel;;*" ),	// filter
							   _( "Save Package List" ) );
#endif


    if ( ! filename.isEmpty() )
    {
	y2milestone( "Exporting package list to %s", (const char *) filename );
#ifdef FIXME
	zypp::PackageImEx exporter;
	exporter.getPMState();

	if ( exporter.doExport( Pathname( (const char *) filename ) ) )
	{
	    // Success

	    autoResolveDependencies();
	}
	else	// Error
#endif
	{
	    y2warning( "Error writing package list to %s", (const char *) filename );

	    // zypp::PackageImEx::doExport() might have left over a partially written file.
	    // Try to delete that. Don't care if it doesn't exist and unlink() fails.
	    ( void ) unlink( (const char *) filename );

	    // Post error popup
	    QMessageBox::warning( this,						// parent
				  _( "Error" ),					// caption
				  _( "Error writing package list to %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}

	fileDialog.unmount( true ); // manual call only necessary if verbose mode desired
    }
}


void
YQPackageSelector::pkgImport()
{
    QY2FloppyMountDialog fileDialog( this,				// parent
				      "user.sel",			// startWith
				      "*.sel;;*",			// filter
				      _( "Load Package List" ),		// caption
				      _floppyDevice,			// floppyDevice
				      "/media/floppy",			// floppyMountPoint
				      false );				// startWithFloppy

    QString filename = fileDialog.askForExistingFile();

#if 0
    QString filename = 	QFileDialog::getOpenFileName( "user.sel",			// startsWith
						      "*.sel;;*",			// filter
						      this,				// parent
						      0,				// name
						      _( "Load Package List" ) );	// caption
#endif

    if ( ! filename.isEmpty() )
    {
	y2milestone( "Importing package list from %s", (const char *) filename );
#ifdef FIXME
	zypp::PackageImEx importer;
	importer.getPMState();

	if ( importer.doImport( Pathname( (const char *) filename ) ) )
	{
	    // Success

	    importer.setPMState();
	    emit refresh();
	}
	else // Error
#endif
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

 	fileDialog.unmount( true ); // manual call only necessary if verbose mode desired
   }
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
