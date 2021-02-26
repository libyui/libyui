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

  Textdomain "qt-pkg"

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
#include <zypp/Patch.h>

#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQPkgDescriptionView.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"

typedef zypp::Patch::Contents			ZyppPatchContents;
typedef zypp::Patch::Contents::const_iterator	ZyppPatchContentsIterator;

using std::set;

#define ENABLE_TOTAL_DOWNLOAD_SIZE	0

YQPkgPatchFilterView::YQPkgPatchFilterView( QWidget * parent )
    : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    _splitter			= new QSplitter( Qt::Vertical, this );	Q_CHECK_PTR( _splitter 	);
    layout->addWidget(_splitter);

    QWidget *upper_box = new QWidget( _splitter );
    QVBoxLayout *vbox = new QVBoxLayout( upper_box );
    _patchList			= new YQPkgPatchList( upper_box );
    Q_CHECK_PTR( _patchList 	);

    vbox->addWidget( _patchList );

    QHBoxLayout * hbox 		= new QHBoxLayout(); Q_CHECK_PTR( hbox );
    vbox->addLayout(hbox);
    vbox->setContentsMargins(0,0,0,0);


    QLabel * label		= new QLabel( _( "&Show Patch Category:" ), upper_box );
    hbox->addWidget(label);

    _patchFilter		= new QComboBox( upper_box );
    Q_CHECK_PTR( _patchFilter );
    hbox->addWidget(_patchFilter);

    _patchFilter->addItem( _( "Needed Patches" ));
    _patchFilter->addItem( _( "Unneeded Patches" ));
    _patchFilter->addItem( _( "All Patches" ),				2 );
    _patchFilter->setCurrentIndex( 0 );

    label->setBuddy( _patchFilter );

    connect( _patchFilter, SIGNAL( activated( int ) ), this, SLOT( fillPatchList() ) );

    _detailsViews		= new QTabWidget( _splitter );			Q_CHECK_PTR( _detailsViews	);

    _descriptionView		= new YQPkgDescriptionView( _detailsViews );	Q_CHECK_PTR( _descriptionView	);

    _detailsViews->addTab( _descriptionView, _( "Patch Description" ) );

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
	    ZyppPatchContents patchContents( patch->contents() );

	    for ( ZyppPatchContentsIterator contents_it = patchContents.begin();
		  contents_it != patchContents.end();
		  ++contents_it )
	    {
		ZyppPkg pkg =  zypp::make<zypp::Package>(*contents_it);
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
	    totalSize += (*it)->candidateObj()->installSize();
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
