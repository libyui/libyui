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

  File:	      YQPkgStatusFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgStatusFilterView.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "utf8.h"
#include "YUIQt.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgStatusFilterView::YQPkgStatusFilterView( QWidget * parent )
    : QVBox( parent )
{
    setMargin( MARGIN );
    setSpacing( SPACING );

    addVStretch( this );

#if 0
    // Headline
    QLabel * label = new QLabel( _( "Changes Overview" ), this );
    CHECK_PTR( label );
    label->setFont( YUIQt::yuiqt()->headingFont() );
#endif


    //
    // Packages with what status to show
    //

    QVGroupBox * gbox = new QVGroupBox( _( "Show status" ), this );
    CHECK_PTR( gbox );


    _showDel		= new QCheckBox( _( "Delete"	    ), gbox ); CHECK_PTR( _showDel	     );
    _showInstall	= new QCheckBox( _( "Install"	    ), gbox ); CHECK_PTR( _showInstall	     );
    _showUpdate		= new QCheckBox( _( "Update"	    ), gbox ); CHECK_PTR( _showUpdate	     );
    _showAutoDel	= new QCheckBox( _( "Auto-delete"   ), gbox ); CHECK_PTR( _showAutoDel	     );
    _showAutoInstall	= new QCheckBox( _( "Auto-install"  ), gbox ); CHECK_PTR( _showAutoInstall   );
    _showAutoUpdate	= new QCheckBox( _( "Auto-update"   ), gbox ); CHECK_PTR( _showAutoUpdate    );
    _showTaboo		= new QCheckBox( _( "Taboo"	    ), gbox ); CHECK_PTR( _showTaboo	     );
    addVStretch( gbox );
    _showKeepInstalled	= new QCheckBox( _( "Keep"	    ), gbox ); CHECK_PTR( _showKeepInstalled );
    _showNoInst		= new QCheckBox( _( "Don't install" ), gbox ); CHECK_PTR( _showNoInst	     );

    _showDel->setChecked( true );
    _showInstall->setChecked( true );
    _showUpdate->setChecked( true );
    _showAutoDel->setChecked( true );
    _showAutoInstall->setChecked( true );
    _showAutoUpdate->setChecked( true );
    _showTaboo->setChecked( true );

    addVStretch( this );


    // Box for refresh button
    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );

    addHStretch( hbox );

    // Refresh button
    _refreshButton = new QPushButton( _( "&Refresh" ), hbox );
    CHECK_PTR( _refreshButton );
    addHStretch( hbox );

    connect( _refreshButton, SIGNAL( clicked() ),
	     this,	    SLOT  ( filter()  ) );

    addVStretch( this );


#if 0
    for ( int i=0; i < 6; i++ )
	addVStretch( this );
#endif
}


YQPkgStatusFilterView::~YQPkgStatusFilterView()
{
    // NOP
}


QSize
YQPkgStatusFilterView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


void
YQPkgStatusFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgStatusFilterView::filter()
{
    emit filterStart();

    PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();

    while ( it != Y2PM::packageManager().end() )
    {
	PMSelectablePtr selectable = *it;

	bool match =
	    check( selectable->candidateObj() ) ||
	    check( selectable->installedObj() );

	// If there is neither an installed nor a candidate package, check
	// any other instance.

	if ( ! match			  &&
	     ! selectable->candidateObj() &&
	     ! selectable->installedObj()   )
	    check( selectable->theObject() );

	++it;
    }

    emit filterFinished();
}


bool
YQPkgStatusFilterView::check( PMPackagePtr pkg )
{
    bool match;
    
    if ( ! pkg )
	return false;

    switch ( pkg->getSelectable()->status() )
    {
	case PMSelectable::S_Taboo:		match = _showTaboo->isChecked();		break;
	case PMSelectable::S_Del:		match = _showDel->isChecked();			break;
	case PMSelectable::S_Update:		match = _showUpdate->isChecked();		break;
	case PMSelectable::S_Install:		match = _showInstall->isChecked();		break;
	case PMSelectable::S_AutoDel:		match = _showAutoDel->isChecked();		break;
	case PMSelectable::S_AutoInstall:	match = _showAutoInstall->isChecked();		break;
	case PMSelectable::S_AutoUpdate:	match = _showAutoUpdate->isChecked();		break;
	case PMSelectable::S_KeepInstalled:	match = _showKeepInstalled->isChecked();	break;
	case PMSelectable::S_NoInst:		match = _showNoInst->isChecked();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    if ( match )
	emit filterMatch( pkg );

    return match;
}



#include "YQPkgStatusFilterView.moc.cc"
