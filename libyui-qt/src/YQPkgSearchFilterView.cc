/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       YQPkgSearchFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgSearchFilterView.h"
#include "YQi18n.h"
#include "utf8.h"
#include "YUIQt.h"


#define SPACING                 6       // between subwidgets
#define MARGIN                  4       // around the widget


YQPkgSearchFilterView::YQPkgSearchFilterView( QWidget * parent )
    : QVBox( parent )
{
    setMargin( MARGIN );
    setSpacing( SPACING );

    addVStretch( this );

    // Headline
    QLabel * label = new QLabel( _( "Searc&h:" ), this );
    CHECK_PTR( label );
    label->setFont( YUIQt::yuiqt()->headingFont() );

    // Input field (combo box) for search text
    _searchText = new QComboBox( this );
    CHECK_PTR( _searchText );
    _searchText->setEditable( true );
    label->setBuddy( _searchText );


    // Box for search button
    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );

    addHStretch( hbox );

    // Search button
    QPushButton * searchButton = new QPushButton( _( "&Search" ), hbox );
    CHECK_PTR( searchButton );

    connect( searchButton, SIGNAL( clicked() ),
             this,         SLOT  ( filter()  ) );

    addVStretch( this );

    //
    // Where to search
    //

    QVGroupBox * gbox = new QVGroupBox( _( "Search in" ), this );
    CHECK_PTR( gbox );


    _searchInName        = new QCheckBox( _( "&Name"        ), gbox ); CHECK_PTR( _searchInName        );
    _searchInSummary     = new QCheckBox( _( "S&ummary"     ), gbox ); CHECK_PTR( _searchInSummary     );
    _searchInDescription = new QCheckBox( _( "Descri&ption" ), gbox ); CHECK_PTR( _searchInDescription );

    _searchInName->setChecked( true );
    _searchInSummary->setChecked( true );

    addVStretch( this );


    //
    // Search mode
    //

    QVButtonGroup * bgroup = new QVButtonGroup( _( "Search Mode" ), this );
    CHECK_PTR( bgroup );
    bgroup->setRadioButtonExclusive( true );

    _contains     = new QRadioButton( _( "C&ontains"               ), bgroup ); CHECK_PTR( _contains     );
    _beginsWith   = new QRadioButton( _( "&Begins with"            ), bgroup ); CHECK_PTR( _beginsWith   );
    _exactMatch   = new QRadioButton( _( "E&xact match"            ), bgroup ); CHECK_PTR( _exactMatch   );
    _useWildcards = new QRadioButton( _( "Use &wildcards"          ), bgroup ); CHECK_PTR( _useWildcards );
    _useRegexp    = new QRadioButton( _( "Use &regular expression" ), bgroup ); CHECK_PTR( _useRegexp    );

    _contains->setChecked( true );

    addVStretch( this );
    _caseSensitive = new QCheckBox( _( "Case sensiti&ve" ), this ); CHECK_PTR( _caseSensitive );

    for ( int i=0; i < 6; i++ )
	addVStretch( this );
}


YQPkgSearchFilterView::~YQPkgSearchFilterView()
{
    // NOP
}

void
YQPkgSearchFilterView::filterIfVisible()
{
    if ( isVisible() )
        filter();
}


void
YQPkgSearchFilterView::filter()
{
    emit filterStart();

    if ( ! _searchText->currentText().isEmpty() )
    {
	QRegExp pattern = _searchText->currentText();
	
	
	
	PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();

	while ( it != Y2PM::packageManager().end() )
	{
	    PMSelectablePtr selectable = *it;

	    bool match =
		check( selectable->installedObj() ) ||
		check( selectable->candidateObj() );

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.

	    if ( ! match                      &&
		 ! selectable->installedObj() &&
		 ! selectable->candidateObj()     )
		check( selectable->theObject() );

	    ++it;
	}
    }

    emit filterFinished();
}


bool
YQPkgSearchFilterView::check( PMPackagePtr pkg )
{
    if ( ! pkg )
	return false;

    emit filterMatch( pkg );

    return true;
}


void
YQPkgSearchFilterView::addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert
}


void
YQPkgSearchFilterView::addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum) ); // hor/vert
}



#include "YQPkgSearchFilterView.moc.cc"
