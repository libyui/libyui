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

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QTabWidget>
#include <QDateTime>
#include <QFrame>
#include <QVBoxLayout>

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
    : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    _splitter			= new QSplitter( Qt::Vertical, this );	Q_CHECK_PTR( _splitter 	);
    layout->addWidget(_splitter);

    QWidget *upper_box = new QWidget( _splitter );
    QVBoxLayout *vbox = new QVBoxLayout( upper_box );
    _patchList			= new YQPkgPatchList( upper_box );
    Q_CHECK_PTR( _patchList 	);

    vbox->addWidget( _patchList );
    //addVSpacing( vbox, 4 );

    QHBoxLayout * hbox 		= new QHBoxLayout(); Q_CHECK_PTR( hbox );
    vbox->addLayout(hbox);
    hbox->setSpacing( SPACING );

    QLabel * label		= new QLabel( _( "&Show Patch Category:" ), upper_box );
    hbox->addWidget(label);

    _patchFilter		= new QComboBox( upper_box );
    Q_CHECK_PTR( _patchFilter );
    hbox->addWidget(_patchFilter);

    _patchFilter->addItem( _( "Installable Patches" ));
    _patchFilter->addItem( _( "Installable and Installed Patches" ));
    _patchFilter->addItem( _( "All Patches" ),				2 );
    _patchFilter->setCurrentIndex( 0 );
    _patchFilter->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    label->setBuddy( _patchFilter );

    connect( _patchFilter, SIGNAL( activated( int ) ), this, SLOT( fillPatchList() ) );
    //addVSpacing( vbox, 4 );

    QWidget *down_box = new QWidget( _splitter );
    vbox			= new QVBoxLayout( down_box );			Q_CHECK_PTR( vbox		);
    //addVSpacing( vbox, 8 );

    _detailsViews		= new QTabWidget( down_box );			Q_CHECK_PTR( _detailsViews	);
    vbox->addWidget(_detailsViews);

    _descriptionView		= new YQPkgDescriptionView( _detailsViews );	Q_CHECK_PTR( _descriptionView	);

    _descriptionView->setMinimumSize( 0, 0 );
    _detailsViews->addTab( _descriptionView, _( "Patch Description" ) );


#if ENABLE_TOTAL_DOWNLOAD_SIZE
    //
    // HBox for total download size
    //

    hbox = new QHBoxLayout(); Q_CHECK_PTR( hbox );
    vbox->addLayout(hbox);
    //addHStretch( hbox );

    hbox->addWidget(new QLabel( _( "Estimated Download Size:" ) + " ", this ));
    _totalDownloadSize		= new QLabel( FSize(0).asString().c_str(), this );
    hbox->addWidget(_totalDownloadSize);
    Q_CHECK_PTR( _totalDownloadSize );


    // Give the total download size a 3D look

    _totalDownloadSize->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    _totalDownloadSize->setLineWidth(1);
    _totalDownloadSize->setMidLineWidth(2);
#endif


    connect( _patchList,	SIGNAL( currentItemChanged    ( ZyppSel ) ),
	     _descriptionView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    connect( _patchList,	SIGNAL( statusChanged() 		),
	     this,		SLOT  ( updateTotalDownloadSize() 	) );

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

    yuiDebug() << "Calculated total download size in "
	       << calcTime.elapsed() << " millisec"
	       << endl;
}


void
YQPkgPatchFilterView::fillPatchList()
{
    switch ( _patchFilter->currentIndex() )
    {
	case 0:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
	case 1:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantAndInstalledPatches );	break;
	case 2:		_patchList->setFilterCriteria( YQPkgPatchList::AllPatches		   );	break;
	default:	_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
    }

    _patchList->fillList();
    _patchList->selectSomething();
}


#include "YQPkgPatchFilterView.moc"
