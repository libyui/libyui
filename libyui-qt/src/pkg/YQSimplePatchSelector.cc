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

  File:	      YQSimplePatchSelector.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <qapplication.h>
#include <qhbox.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qsplitter.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQSimplePatchSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQWizard.h"
#include "YQDialog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;

#define SHOW_DISK_USAGE		0

#define SPACING			6
#define MARGIN			6



YQSimplePatchSelector::YQSimplePatchSelector( YWidget *	parent, long modeFlags )
    : YQPackageSelectorBase( parent, modeFlags )
{
    _patchFilterView	= 0;
    _patchList		= 0;
    _diskUsageList	= 0;
    _wizard		= findWizard();

    basicLayout();
    makeConnections();

#if 0
    _patchList->fillList();
    _patchList->selectSomething();
#endif

    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();
}



YQWizard *
YQSimplePatchSelector::findWizard() const
{
    YQWizard * wizard = 0;

    YQDialog * dialog = dynamic_cast<YQDialog *> ( YDialog::currentDialog() );

    if ( dialog )
	wizard = dialog->findWizard();

    return wizard;
}



void
YQSimplePatchSelector::basicLayout()
{
    QSplitter * splitter = new QSplitter( QSplitter::Vertical, this );
    CHECK_PTR( splitter );
    splitter->setMargin( MARGIN );

    //
    // PatchFilterView
    //

    QVBox * upper_vbox = new QVBox( splitter );
    CHECK_PTR( upper_vbox );
    splitter->setResizeMode( upper_vbox, QSplitter::Stretch );

    _patchFilterView = new YQPkgPatchFilterView( upper_vbox );
    CHECK_PTR( _patchFilterView );
    
    _patchList = _patchFilterView->patchList();
    CHECK_PTR( _patchList );
    
    addVSpacing( upper_vbox, MARGIN );

    //
    // Disk Usage
    //


#if SHOW_DISK_USAGE
    QVBox * lower_vbox = new QVBox( splitter );
    CHECK_PTR( lower_vbox );
    addVSpacing( lower_vbox, MARGIN );

    _diskUsageList = new YQPkgDiskUsageList( lower_vbox );
    CHECK_PTR( _diskUsageList );
    
    splitter->setResizeMode( lower_vbox, QSplitter::FollowSizeHint );
#endif


    //
    // Buttons
    //
    
    if ( _wizard )	// No button box - add "Details..." button here
    {
	//
	// "Details" button
	//

	addVSpacing( this, SPACING );

	QHBox * hbox = new QHBox( this );
	CHECK_PTR( hbox );

	QPushButton * details_button = new QPushButton( _( "&Details..." ), hbox );
	CHECK_PTR( details_button );

	connect( details_button, SIGNAL( clicked() ),
		 this,		 SLOT  ( detailedPackageSelection() ) );

	addHStretch( hbox );
    }
    else // ! _wizard
    {
	layoutButtons( this );
    }
}


void
YQSimplePatchSelector::layoutButtons( QWidget * parent )
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
YQSimplePatchSelector::makeConnections()
{
    if ( _patchList && _diskUsageList )
    {
	connect( _patchList,	 SIGNAL( updatePackages()  ),
		 _diskUsageList, SLOT  ( updateDiskUsage() ) );
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
YQSimplePatchSelector::detailedPackageSelection()
{
    y2milestone( "\"Details..\" button clicked" );
    YQUI::ui()->sendEvent( new YMenuEvent( YCPSymbol( "details" ) ) );
}


void
YQSimplePatchSelector::debugTrace()
{
    y2warning( "debugTrace" );
}



#include "YQSimplePatchSelector.moc"
