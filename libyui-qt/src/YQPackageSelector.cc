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

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define CHECK_DEPENDENCIES_ON_STARTUP	1

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qdialog.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qstylefactory.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qvbox.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPackageSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDependenciesView.h"
#include "YQPkgList.h"
#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgSelList.h"
#include "YQPkgSelectionsFilterView.h"
#include "YQPkgStatusFilterView.h"
#include "YQPkgTechnicalDetailsView.h"
#include "YQPkgUpdateProblemFilterView.h"
#include "YQPkgVersionsView.h"
#include "YQPkgYouPatchFilterView.h"
#include "YQPkgYouPatchList.h"

#include "QY2ComboTabWidget.h"
#include "YQDialog.h"
#include "utf8.h"
#include "YUIQt.h"
#include "YQi18n.h"
#include "layoututils.h"



using std::max;

#define MIN_WIDTH			800
#define MIN_HEIGHT			600

#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget



YQPackageSelector::YQPackageSelector( YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt )
    : QVBox(parent)
    , YPackageSelector( opt )
    , _yuiqt(yuiqt)
{
    setWidgetRep(this);

    _autoDependenciesCheckBox	= 0;
    _detailsViews		= 0;
    _diskSpace			= 0;
    _filters			= 0;
    _leftPane			= 0;
    _pkgDescriptionView		= 0;
    _pkgDependenciesView	= 0;
    _pkgList			= 0;
    _pkgTechnicalDetailsView	= 0;
    _pkgVersionsView		= 0;
    _rpmGroupTagsFilterView	= 0;
    _searchFilterView		= 0;
    _selList			= 0;
    _selectionsFilterView	= 0;
    _statusFilterView		= 0;
    _updateProblemFilterView	= 0;
    _youPatchFilterView		= 0;
    _youPatchList		= 0;


    _youMode	= opt.youMode.value();
    _updateMode	= opt.updateMode.value();
    _testMode	= opt.testMode.value();

    if ( _testMode )	fakeData();
    if ( _youMode )	y2milestone( "YOU mode" );
    if ( _updateMode )	y2milestone( "Update mode" );


    setTextdomain( "packages-qt" );
    setFont( _yuiqt->currentFont() );
    yuiqt->blockWmClose(); // Automatically undone after UI::RunPkgSelection()
    _conflictDialog = new YQPkgConflictDialog( this );
    CHECK_PTR( _conflictDialog );

    basicLayout();
    makeConnections();
    emit loadData();

    Y2PM::packageManager().SaveState();
    Y2PM::selectionManager().SaveState();

    if ( _youMode )
    {
	Y2PM::youPatchManager().SaveState();

	if ( _filters && _youPatchFilterView && _youPatchList )
	{
	    _filters->showPage( _youPatchFilterView );
	    _youPatchList->filter();
	    _youPatchList->clearSelection();
	    _youPatchList->selectSomething();
	}
    }
    else
    {
	if ( _filters )
	{
	    if ( _updateProblemFilterView )
	    {
		_filters->showPage( _updateProblemFilterView );
		_updateProblemFilterView->filter();

	    }
	    else if ( _selectionsFilterView && _selList )
	    {
		_filters->showPage( _selectionsFilterView );
		_selList->filter();
	    }
	}

#if CHECK_DEPENDENCIES_ON_STARTUP

	if ( ! _youMode )
	{
	    // Fire up the first dependency check in the main loop.
	    // Don't do this right away - wait until all initializations are finished.
	    QTimer::singleShot( 0, this, SLOT( autoResolveDependencies() ) );
	}
#endif
    }

    y2milestone( "PackageSelector init done" );
}


void
YQPackageSelector::basicLayout()
{
#if 0
    layoutMenuBar( this );
#endif

    QSplitter *outer_splitter = new QSplitter( QSplitter::Horizontal, this );
    CHECK_PTR( outer_splitter );

    layoutLeftPane ( outer_splitter );
    layoutRightPane( outer_splitter );
}


void
YQPackageSelector::layoutLeftPane( QWidget * parent )
{
    QVBox *left_vbox = new QVBox( parent );
    CHECK_PTR( left_vbox );
    left_vbox->setSpacing( SPACING );
    left_vbox->setMargin( MARGIN );

    layoutFilters( left_vbox );
    layoutDiskSpaceSummary( left_vbox );
    _leftPane = left_vbox;
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
	if ( ! Y2PM::packageManager().updateEmpty()
	     || _testMode )
	{
	    _updateProblemFilterView = new YQPkgUpdateProblemFilterView( parent );
	    CHECK_PTR( _updateProblemFilterView );
	    _filters->addPage( _( "Update Problems" ), _updateProblemFilterView );

	    connect( _filters,			SIGNAL( currentChanged( QWidget * ) ),
		     _updateProblemFilterView,	SLOT  ( filterIfVisible()            ) );
	}
    }


    //
    // YOU patches view
    //

    if ( _youMode )
    {
	_youPatchFilterView = new YQPkgYouPatchFilterView( parent );
	CHECK_PTR( _youPatchFilterView );
	_filters->addPage( _( "YOU Patches" ), _youPatchFilterView );

	_youPatchList = _youPatchFilterView->youPatchList();
	CHECK_PTR( _youPatchList );

	connect( _filters,	SIGNAL( currentChanged( QWidget * ) ),
		 _youPatchList,	SLOT  ( filterIfVisible()           ) );
    }


    //
    // Selections view
    //

    if ( ! _youMode )
    {
	_selectionsFilterView = new YQPkgSelectionsFilterView( parent );
	CHECK_PTR( _selectionsFilterView );
	_filters->addPage( _( "Selections" ), _selectionsFilterView );

	_selList = _selectionsFilterView->selList();
	CHECK_PTR( _selList );

	connect( _filters, 	SIGNAL( currentChanged( QWidget * ) ),
		 _selList,	SLOT  ( filterIfVisible()           ) );
    }


    //
    // RPM group tags view
    //

    // if ( ! _youMode )
    {
	_rpmGroupTagsFilterView = new YQPkgRpmGroupTagsFilterView( parent );
	CHECK_PTR( _rpmGroupTagsFilterView );
	_filters->addPage( _( "Package Groups" ), _rpmGroupTagsFilterView );

	connect( this,    			SIGNAL( loadData() ),
		 _rpmGroupTagsFilterView,	SLOT  ( filter()   ) );

	connect( _filters, 			SIGNAL( currentChanged( QWidget * ) ),
		 _rpmGroupTagsFilterView,	SLOT  ( filterIfVisible()           ) );
    }


    //
    // Package search view
    //

    // if ( ! _youMode )
    {
	_searchFilterView = new YQPkgSearchFilterView( parent );
	CHECK_PTR( _searchFilterView );
	_filters->addPage( _( "Search" ), _searchFilterView );

	connect( _filters, 		SIGNAL( currentChanged( QWidget * ) ),
		 _searchFilterView,	SLOT  ( filterIfVisible()           ) );
    }


    //
    // Status change view
    //

    // if ( ! _youMode )
    {
	_statusFilterView = new YQPkgStatusFilterView( parent );
	CHECK_PTR( _statusFilterView );
	_filters->addPage( _( "Installation Summary" ), _statusFilterView );

	connect( _filters, 		SIGNAL( currentChanged( QWidget * ) ),
		 _statusFilterView,	SLOT  ( filterIfVisible()           ) );
    }


#if 0
    // DEBUG

    _filters->addPage( _("Keywords"   ), new QLabel( "Keywords\nfilter\n\nfor future use", 0 ) );
    _filters->addPage( _("MIME Types" ), new QLabel( "MIME Types\nfilter\n\nfor future use" , 0 ) );
#endif

}



void
YQPackageSelector::layoutDiskSpaceSummary( QWidget * parent )
{
    QHGroupBox * gbox = new QHGroupBox( _( "Disk space" ), parent );
    CHECK_PTR( gbox );
    gbox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert

    _diskSpace = new QProgressBar( gbox );
    CHECK_PTR( _diskSpace );

    _diskSpace->setStyle( QStyleFactory::create( "motif" ) ); // consumes less space than platinum

#if 0
    QPushButton *details_button = new QPushButton( _( "D&etails..." ), gbox );
    CHECK_PTR( details_button );

    details_button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
#endif

    _diskSpace->setTotalSteps( 100 );
    updateDiskUsage();

    gbox->setMinimumHeight( gbox->sizeHint().height() );
}


void
YQPackageSelector::layoutRightPane( QWidget * parent )
{
    QSplitter *right_splitter = new QSplitter( QSplitter::Vertical, parent );
    CHECK_PTR( right_splitter );
    right_splitter->setMargin( MARGIN );

    layoutPkgList( right_splitter );
    layoutDetailsViews( right_splitter );
}


void
YQPackageSelector::layoutPkgList( QWidget * parent )
{
    _pkgList= new YQPkgList( parent );
    CHECK_PTR( _pkgList );

    if ( _youMode )
    {
	_pkgList->setEditable( false );
    }

    connect( _pkgList, 	SIGNAL( statusChanged()	          ),
	     this,	SLOT  ( autoResolveDependencies() ) );

    connect( _pkgList,	SIGNAL( statusChanged()   ),
	     this,	SLOT  ( updateDiskUsage() ) );
}


void
YQPackageSelector::layoutDetailsViews( QWidget * parent )
{
    QVBox *details_vbox = new QVBox( parent );
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

    connect( _pkgList,			SIGNAL( selectionChanged    ( PMObjectPtr ) ),
	     _pkgDescriptionView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );

    //
    // Technical details
    //

    _pkgTechnicalDetailsView = new YQPkgTechnicalDetailsView( _detailsViews );
    CHECK_PTR( _pkgTechnicalDetailsView );

    _detailsViews->addTab( _pkgTechnicalDetailsView, _( "&Technical Data" ) );

    connect( _pkgList,			SIGNAL( selectionChanged    ( PMObjectPtr ) ),
	     _pkgTechnicalDetailsView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );


    //
    // Dependencies
    //

    _pkgDependenciesView = new YQPkgDependenciesView( _detailsViews );
    CHECK_PTR( _pkgDependenciesView );

    _detailsViews->addTab( _pkgDependenciesView, _( "Dependencies" ) );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,			SIGNAL( selectionChanged    ( PMObjectPtr ) ),
	     _pkgDependenciesView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );


    //
    // Versions
    //

    // if ( ! _youMode )
    {
	_pkgVersionsView = new YQPkgVersionsView( _detailsViews,
						  ! _youMode );	// userCanSwitchVersions
	CHECK_PTR( _pkgVersionsView );

	_detailsViews->addTab( _pkgVersionsView, _( "&Versions" ) );

	connect( _pkgList,		SIGNAL( selectionChanged    ( PMObjectPtr ) ),
		 _pkgVersionsView,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );
    }


    layoutButtons( details_vbox );
}


void
YQPackageSelector::layoutButtons( QWidget * parent )
{
    QHBox * button_box = new QHBox( parent );
    CHECK_PTR( button_box );
    button_box->setSpacing( SPACING );

#if 0
    QPushButton * help_button = new QPushButton( _( "&Help" ), button_box );
    CHECK_PTR( help_button );
#endif

    if ( ! _youMode )
    {
	QPushButton * solve_button = new QPushButton( _( "Check &Dependencies" ), button_box );
	CHECK_PTR( solve_button );
	solve_button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert

	connect( solve_button, SIGNAL( clicked() ),
		 this,         SLOT  ( resolveDependencies() ) );


	_autoDependenciesCheckBox = new QCheckBox( _( "A&uto check" ), button_box );
	CHECK_PTR( _autoDependenciesCheckBox );
	_autoDependenciesCheckBox->setChecked( true );
    }

    addHStretch( button_box );

    QPushButton * cancel_button = new QPushButton( _( "&Cancel" ), button_box );
    CHECK_PTR( cancel_button );
    cancel_button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert

    connect( cancel_button, SIGNAL( clicked() ),
	     this,          SLOT  ( reject()   ) );


    QPushButton * accept_button = new QPushButton( _( "&Accept" ), button_box );
    CHECK_PTR( accept_button );
    accept_button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert

    connect( accept_button, SIGNAL( clicked() ),
	     this,          SLOT  ( accept()   ) );

    button_box->setMinimumHeight( button_box->sizeHint().height() );
}


void
YQPackageSelector::layoutMenuBar( QWidget * parent )
{
    QMenuBar *menu_bar = new QMenuBar( parent );
    CHECK_PTR( menu_bar );

    QPopupMenu * menu = new QPopupMenu( parent );
    CHECK_PTR( menu );
    menu_bar->insertItem( _( "&File" ), menu );

    menu->insertItem( _( "&Close" ), this, SLOT( close() ), ALT+Key_F4 );



#if 0
    menu = new QPopupMenu( parent );
    CHECK_PTR( menu );
    menu_bar->insertSeparator();
    menu_bar->insertItem( _( "&Help" ), menu );

    menu->insertItem( _( "&Overview" ), this, SLOT( help() ), Key_F1 );
#endif
}


void
YQPackageSelector::connectFilter( QWidget * filter,
				  QWidget * pkgList,
				  bool hasUpdateSignal )
{
    if ( ! filter  )	return;
    if ( ! pkgList )	return;

    connect( filter,	SIGNAL( filterStart() 	),
	     pkgList, 	SLOT  ( clear() 	) );

    connect( filter,	SIGNAL( filterMatch( PMPackagePtr ) ),
	     pkgList, 	SLOT  ( addPkgItem ( PMPackagePtr ) ) );

    connect( filter, 	SIGNAL( filterFinished()  ),
	     pkgList, 	SLOT  ( selectSomething() ) );


    if ( hasUpdateSignal )
    {
	connect( filter, 	SIGNAL( updatePackages()           ),
		 pkgList,	SLOT  ( updateToplevelItemStates() ) );

	connect( filter,	SIGNAL( updatePackages()	   ),
		 this,		SLOT  ( updateDiskUsage()	   ) );
    }
}


void
YQPackageSelector::makeConnections()
{
    connectFilter( _updateProblemFilterView,	_pkgList, false );
    connectFilter( _youPatchList, 		_pkgList );
    connectFilter( _selList, 			_pkgList );
    connectFilter( _rpmGroupTagsFilterView, 	_pkgList, false );
    connectFilter( _statusFilterView, 		_pkgList, false );
    connectFilter( _searchFilterView, 		_pkgList, false );

    if ( _searchFilterView && _pkgList )
    {
	connect( _searchFilterView, 	SIGNAL( message( const QString & ) ),
		 _pkgList,		SLOT  ( message( const QString & ) ) );
    }

    //
    // Connect conflict dialog
    //

    if ( _conflictDialog && _pkgList )
    {
	connect( _conflictDialog,	SIGNAL( updatePackages()      ),
		 _pkgList, 		SLOT  ( updateToplevelItemStates() ) );
    }


    if ( _pkgVersionsView && _pkgList )
    {
	connect( _pkgVersionsView, 	SIGNAL( candidateChanged( PMObjectPtr ) ),
		 _pkgList,		SLOT  ( updateToplevelItemData() ) );
    }
}


void
YQPackageSelector::autoResolveDependencies()
{
    if ( _autoDependenciesCheckBox && ! _autoDependenciesCheckBox->isChecked() )
	return;

    resolveDependencies();
}


int
YQPackageSelector::resolveDependencies()
{
    if ( ! _conflictDialog )
    {
	y2error( "No conflict dialog existing" );
	return QDialog::Accepted;
    }

    return _conflictDialog->solveAndShowConflicts();
}


void
YQPackageSelector::updateDiskUsage()
{
    YUIQt::yuiqt()->busyCursor();

    const PkgDuMaster & du = Y2PM::packageManager().updateDu();
    _diskSpace->setProgress( du.pkg_u_percent() );

    if ( du.pkg_u_percent() < 1 )
    {
	// Workaround for a common problem: Maybe just the target partitions
	// are not correctly initialized, so let's at least give a hint in the
	// log file.

	FSize used = du.pkg_used();
	y2milestone( "Used disk space: %s", used.asString().c_str() );

	if ( _testMode )
	    _diskSpace->setProgress( 42 );
    }

    YUIQt::yuiqt()->normalCursor();
}


void
YQPackageSelector::fakeData()
{
    y2warning( "*** Using fake data ***" );

    if ( _youMode )
    {
	Url url( "dir:///8.1-patches" );
	Y2PM::youPatchManager().instYou().retrievePatchInfo( url, false );
	Y2PM::youPatchManager().instYou().selectPatches( PMYouPatch::kind_recommended |
							 PMYouPatch::kind_security     );
	y2milestone( "Fake YOU patches initialized" );
    }
    else
    {
	InstSrcManager & MGR( Y2PM::instSrcManager() );

	Url url( "dir:///8.1" );
	InstSrcManager::ISrcIdList nids;
	PMError err = MGR.scanMedia( nids, url );

	if ( nids.size() )
	{
	    err = MGR.enableSource( *nids.begin() );
	}

	y2milestone( "Fake installation sources initialized" );
    }
}


void
YQPackageSelector::reject()
{
    if ( QMessageBox::warning( this, "",
			       _( "Abandon all changes?" ),
			       _( "&OK" ), _( "&Cancel" ), "",
			       1, // defaultButtonNumber (from 0)
			       1 ) // escapeButtonNumber
	 == 0 )	// Proceed upon button #0 (OK)
    {
	Y2PM::packageManager().RestoreState();
	Y2PM::selectionManager().RestoreState();

	if ( _youMode )
	    Y2PM::youPatchManager().RestoreState();

	_yuiqt->setMenuSelection( YCPSymbol("cancel", true) );
	_yuiqt->returnNow( YUIInterpreter::ET_MENU, this );
    }
}


void
YQPackageSelector::accept()
{
    if ( resolveDependencies() != QDialog::Rejected )
    {
	Y2PM::packageManager().ClearSaveState();
	Y2PM::selectionManager().ClearSaveState();

	if ( _youMode )
	    Y2PM::youPatchManager().ClearSaveState();

	_yuiqt->setMenuSelection( YCPSymbol("accept", true) );
	_yuiqt->returnNow( YUIInterpreter::ET_MENU, this );
    }
}


void
YQPackageSelector::help()
{
    QMessageBox::information( this, _( "Help" ),
			      "No online help available yet.\nSorry.",
			      QMessageBox::Ok );
}


long
YQPackageSelector::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ)
    {
	int hintWidth = sizeHint().width();

	return max( MIN_WIDTH, hintWidth );
    }
    else
    {
	int hintHeight = sizeHint().height();

	return max( MIN_HEIGHT, hintHeight );
    }
}


void
YQPackageSelector::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void
YQPackageSelector::setEnabling( bool enabled )
{
    setEnabled( enabled );
}


bool
YQPackageSelector::setKeyboardFocus()
{
    setFocus();

    return true;
}


void
YQPackageSelector::setTextdomain( const char * domain )
{
    bindtextdomain( domain, LOCALEDIR );
    bind_textdomain_codeset( domain, "utf8" );
    textdomain( domain );

    // Make change known.
    {
	extern int _nl_msg_cat_cntr;
	++_nl_msg_cat_cntr;
    }
}


void addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert
}


void addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum) ); // hor/vert
}


void addVSpacing( QWidget * parent, int height )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedHeight( height );
}


void addHSpacing( QWidget * parent, int width )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedWidth( width );
}




#include "YQPackageSelector.moc.cc"
