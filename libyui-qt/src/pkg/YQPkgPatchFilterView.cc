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

  File:	      YQPkgPatchFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qdatetime.h>

#include <FSize.h>
#include <zypp/ui/PatchContents.h>

#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgSelList.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"

typedef zypp::ui::PatchContents			ZyppPatchContents;
typedef zypp::ui::PatchContents::const_iterator	ZyppPatchContentsIterator;

using std::set;

#define ENABLE_TOTAL_DOWNLOAD_SIZE	0

#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgPatchFilterView::YQPkgPatchFilterView( QWidget * parent )
    : QVBox( parent )
{
    QVBox * vbox;
    _haveUpdateStackPatches	= YQPkgPatchList::haveUpdateStackPatches();

    _splitter			= new QSplitter( QSplitter::Vertical, this );	CHECK_PTR( _splitter 	);
    vbox			= new QVBox( _splitter );			CHECK_PTR( vbox		);
    _patchList			= new YQPkgPatchList( vbox );			CHECK_PTR( _patchList 	);

    addVSpacing( vbox, 4 );

    QHBox * hbox 		= new QHBox( vbox ); CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );

    QLabel * label		= new QLabel( _( "&Show Patch Category:" ), hbox );

    _patchFilter		= new QComboBox( hbox );
    CHECK_PTR( _patchFilter );

    int index = 0;
    
    if ( _haveUpdateStackPatches )
	// Translators: This is about patches that patch the update engine (libzypp, YaST) itself
	_patchFilter->insertItem( _( "Update Stack Patches" ),			index++ );
	
    _patchFilter->insertItem( _( "Installable Patches" ),			index++ );
    _patchFilter->insertItem( _( "Installable and Installed Patches" 	),	index++ );
    _patchFilter->insertItem( _( "All Patches" ),				index++ );
    _patchFilter->setCurrentItem( 0 );
    _patchFilter->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    label->setBuddy( _patchFilter );

    connect( _patchFilter, SIGNAL( activated( int ) ), this, SLOT( fillPatchList() ) );
    addVSpacing( vbox, 4 );

    vbox			= new QVBox( _splitter );			CHECK_PTR( vbox			);
    addVSpacing( vbox, 8 );

    _detailsViews		= new QTabWidget( vbox );			CHECK_PTR( _detailsViews	);
    _detailsViews->setMargin( MARGIN );

    _descriptionView		= new YQPkgDescriptionView( _detailsViews );	CHECK_PTR( _descriptionView	);

    _descriptionView->setMinimumSize( 0, 0 );
    _detailsViews->addTab( _descriptionView, _( "Patch Description" ) );

    
#if ENABLE_TOTAL_DOWNLOAD_SIZE
    //
    // HBox for total download size
    //

    addVSpacing( vbox, 4 );
    hbox = new QHBox( vbox ); CHECK_PTR( hbox );
    addHStretch( hbox );

    new QLabel( _( "Estimated Download Size:" ) + " ", hbox );
    _totalDownloadSize		= new QLabel( FSize(0).asString().c_str(), hbox );
    CHECK_PTR( _totalDownloadSize );


    // Give the total download size a 3D look

    _totalDownloadSize->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    _totalDownloadSize->setLineWidth(1);
    _totalDownloadSize->setMidLineWidth(2);
#endif


    connect( _patchList,	SIGNAL( selectionChanged    ( ZyppSel ) ),
	     _descriptionView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    connect( _patchList,	SIGNAL( statusChanged() 		),
	     this,		SLOT  ( updateTotalDownloadSize() 	) );

    fillPatchList();
    updateTotalDownloadSize();
}


YQPkgPatchFilterView::~YQPkgPatchFilterView()
{
    // NOP
}


void
YQPkgPatchFilterView::updateTotalDownloadSize()
{
    set<ZyppSel> selectablesToInstall;
    QTime calcTime;
    calcTime.start();

    for ( ZyppPoolIterator patches_it = zyppPatchesBegin();
	  patches_it != zyppPatchesEnd();
	  ++patches_it )
    {
	ZyppPatch patch = tryCastToZyppPatch( (*patches_it)->theObj() );

	if ( patch )
	{
	    ZyppPatchContents patchContents( patch );

	    for ( ZyppPatchContentsIterator contents_it = patchContents.begin();
		  contents_it != patchContents.end();
		  ++contents_it )
	    {
		ZyppPkg pkg = tryCastToZyppPkg( *contents_it );
		ZyppSel sel;

		if ( pkg )
		    sel = _selMapper.findZyppSel( pkg );

		if ( sel )
		{
		    switch ( sel->status() )
		    {
			case S_Install:
			case S_AutoInstall:
			case S_Update:
			case S_AutoUpdate:
			    // Insert the patch contents selectables into a set,
			    // don't immediately sum up their sizes: The same
			    // package could be in more than one patch, but of
			    // course it will be downloaded only once.

			    selectablesToInstall.insert( sel );
			    break;

			case S_Del:
			case S_AutoDel:
			case S_NoInst:
			case S_KeepInstalled:
			case S_Taboo:
			case S_Protected:
			    break;

			    // intentionally omitting 'default' branch so the compiler can
			    // catch unhandled enum states
		    }

		}
	    }
	}
    }


    FSize totalSize = 0;

    for ( set<ZyppSel>::iterator it = selectablesToInstall.begin();
	  it != selectablesToInstall.end();
	  ++it )
    {
	if ( (*it)->candidateObj() )
	    totalSize += (*it)->candidateObj()->size();
    }

#if ENABLE_TOTAL_DOWNLOAD_SIZE
    _totalDownloadSize->setText( totalSize.asString().c_str() );
#endif

    y2debug( "Calculated total download size in %d millisec", calcTime.elapsed() );
}


void
YQPkgPatchFilterView::fillPatchList()
{
    int currentIndex = _patchFilter->currentItem();

    if ( _haveUpdateStackPatches )
	currentIndex--;
    
    switch ( currentIndex )
    {
	case -1:	_patchList->setFilterCriteria( YQPkgPatchList::UpdateStackPatches          );	break;
	case 0:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
	case 1:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantAndInstalledPatches );	break;
	case 2:		_patchList->setFilterCriteria( YQPkgPatchList::AllPatches		   );	break;
	default:	_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
    }

    _patchList->fillList();
    _patchList->selectSomething();
}


#include "YQPkgPatchFilterView.moc"
