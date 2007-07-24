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

  Textdomain "packages-qt"

/-*/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h>
#include <qprogressdialog.h>
#include <qdatetime.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPkgSearchFilterView.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "utf8.h"
#include "YQUI.h"

using std::list;
using std::string;


#define SPACING                 6       // between subwidgets
#define MARGIN                  4       // around the widget


YQPkgSearchFilterView::YQPkgSearchFilterView( QWidget * parent )
    : QVBox( parent )
{
    _matchCount = 0;
    setMargin( MARGIN );
    setSpacing( SPACING );

    addVStretch( this );

    // Headline
    QLabel * label = new QLabel( _( "Searc&h:" ), this );
    CHECK_PTR( label );
    label->setFont( YQUI::ui()->headingFont() );

    // Input field ( combo box ) for search text
    _searchText = new QComboBox( this );
    CHECK_PTR( _searchText );
    _searchText->setEditable( true );
    label->setBuddy( _searchText );


    // Box for search button
    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );

    addHStretch( hbox );

    // Search button
    _searchButton = new QPushButton( _( "&Search" ), hbox );
    CHECK_PTR( _searchButton );

    connect( _searchButton, SIGNAL( clicked() ),
             this,          SLOT  ( filter()  ) );

    addVStretch( this );

    //
    // Where to search
    //

    QVGroupBox * gbox = new QVGroupBox( _( "Search in" ), this );
    CHECK_PTR( gbox );


    _searchInName        = new QCheckBox( _( "&Name" 		), gbox ); CHECK_PTR( _searchInName        );
    _searchInSummary     = new QCheckBox( _( "Su&mmary" 	), gbox ); CHECK_PTR( _searchInSummary     );
    _searchInDescription = new QCheckBox( _( "Descr&iption"	), gbox ); CHECK_PTR( _searchInDescription );

    addVStretch( gbox );

    // Intentionally NOT marking RPM tags for translation
    _searchInProvides    = new QCheckBox(  "RPM \"&Provides\""   , gbox ); CHECK_PTR( _searchInProvides    );
    _searchInRequires    = new QCheckBox(  "RPM \"Re&quires\""   , gbox ); CHECK_PTR( _searchInRequires    );

    _searchInName->setChecked( true );
    _searchInSummary->setChecked( true );

    addVStretch( this );


    //
    // Search mode
    //

    label = new QLabel( _( "Search &Mode:" ), this );
    CHECK_PTR( label );

    _searchMode = new QComboBox( this );
    CHECK_PTR( _searchMode );
    _searchMode->setEditable( false );

    label->setBuddy( _searchMode );

    // Caution: combo box items must be inserted in the same order as enum SearchMode!
    _searchMode->insertItem( _( "Contains"		 ) );
    _searchMode->insertItem( _( "Begins with"		 ) );
    _searchMode->insertItem( _( "Exact Match"		 ) );
    _searchMode->insertItem( _( "Use Wild Cards" 	 ) );
    _searchMode->insertItem( _( "Use Regular Expression" ) );

    _searchMode->setCurrentItem( Contains );


    addVStretch( this );

    _caseSensitive = new QCheckBox( _( "Case Sensiti&ve" ), this );
    CHECK_PTR( _caseSensitive );

    for ( int i=0; i < 6; i++ )
	addVStretch( this );
}


YQPkgSearchFilterView::~YQPkgSearchFilterView()
{
    // NOP
}


void
YQPkgSearchFilterView::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	if ( event->state() == 0 )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		_searchButton->animateClick();
		return;
	    }
	}

    }

    QVBox::keyPressEvent( event );
}


void
YQPkgSearchFilterView::setFocus()
{
    _searchText->setFocus();
}


QSize
YQPkgSearchFilterView::minimumSizeHint() const
{
    return QSize( 0, 0 );
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
    _matchCount = 0;

    if ( ! _searchText->currentText().isEmpty() )
    {
	// Create a progress dialog that is only displayed if the search takes
	// longer than a couple of seconds ( default: 4 ).

	QProgressDialog progress( _( "Searching..." ),			// text
				  _( "&Cancel" ),			// cancelButtonLabel
#ifdef FIXME
				  Y2PM::packageManager().size(),	// totalSteps
#else
				  1000,
#endif
				  this, 0,				// parent, name
				  true );				// modal
	progress.setCaption( "" );
	progress.setMinimumDuration( 2000 ); // millisec
	QTime timer;

	QRegExp regexp = _searchText->currentText();
	regexp.setCaseSensitive( _caseSensitive->isChecked() );
	regexp.setWildcard( _searchMode->currentItem() == UseWildcards );

	timer.start();


	int count = 0;

	for ( ZyppPoolIterator it = zyppPkgBegin();
	      it != zyppPkgEnd() && ! progress.wasCancelled();
	      ++it )
	{
	    ZyppSel selectable = *it;

	    bool match =
		check( selectable, selectable->candidateObj(), regexp ) ||
		check( selectable, selectable->installedObj(), regexp );

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.

	    if ( ! match                      &&
		 ! selectable->candidateObj() &&
		 ! selectable->installedObj()   )
		check( selectable, selectable->theObj(), regexp );


	    progress.setProgress( count++ );

	    if ( timer.elapsed() > 300 ) // milisec
	    {
		// Process events only every 300 milliseconds - this is very
		// expensive since both the progress dialog and the package
		// list change all the time, thus display updates are necessary
		// each time.

		qApp->processEvents();
		timer.restart();
	    }
	}

	if ( _matchCount == 0 )
	    emit message( _( "No Results." ) );
    }

    emit filterFinished();
}


bool
YQPkgSearchFilterView::check( ZyppSel	selectable,
			      ZyppObj 	zyppObj )
{
    QRegExp regexp = _searchText->currentText();
    regexp.setCaseSensitive( _caseSensitive->isChecked() );
    regexp.setWildcard( _searchMode->currentItem() == UseWildcards );

    return check( selectable, zyppObj, regexp );
}


bool
YQPkgSearchFilterView::check( ZyppSel		selectable,
			      ZyppObj		zyppObj,
			      const QRegExp & 	regexp )
{
    if ( ! zyppObj )
	return false;

    bool match =
	( _searchInName->isChecked()        && check( zyppObj->name(),        regexp ) ) ||
	( _searchInSummary->isChecked()     && check( zyppObj->summary(),     regexp ) ) ||
	( _searchInDescription->isChecked() && check( zyppObj->description(), regexp ) ) ||
	( _searchInProvides->isChecked()    && check( zyppObj->dep( zypp::Dep::PROVIDES ), regexp ) ) ||
	( _searchInRequires->isChecked()    && check( zyppObj->dep( zypp::Dep::REQUIRES ), regexp ) );

    if ( match )
    {
	ZyppPkg zyppPkg = tryCastToZyppPkg( zyppObj );

	if ( zyppPkg )
	{
	    _matchCount++;
	    emit filterMatch( selectable, zyppPkg );
	}
    }

    return match;
}


bool
YQPkgSearchFilterView::check( const string &	attribute,
			      const QRegExp &	regexp )
{
    QString att    	= fromUTF8( attribute );
    QString searchText	= _searchText->currentText();
    bool match		= false;

    switch ( _searchMode->currentItem() )
    {
	case Contains:
	    match = att.contains( searchText, _caseSensitive->isChecked() );
	    break;

	case BeginsWith:
	    match = att.startsWith( searchText );	// only case sensitive
	    break;

	case ExactMatch:
	    match = ( att == searchText );
	    break;

	case UseWildcards:
	case UseRegExp:
	    // Both cases differ in how the regexp is set up during initialization
	    match = att.contains( regexp );
	    break;

	    // Intentionally omitting "default" branch - let gcc watch for unhandled enums
    }

    return match;
}


bool
YQPkgSearchFilterView::check( const zypp::CapSet & capSet, const QRegExp & regexp )
{
    for ( zypp::CapSet::const_iterator it = capSet.begin();
	  it != capSet.end();
	  ++it )
    {
	if ( check( ( *it).index(), regexp ) )
	{
	    // y2debug( "Match for %s", (*it).asString().c_str() );
	    return true;
	}
    }

    return false;
}



#include "YQPkgSearchFilterView.moc"
