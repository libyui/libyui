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

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QProgressDialog>
#include <QDateTime>
#include <QKeyEvent>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPkgSearchFilterView.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "utf8.h"
#include "YQApplication.h"
#include "YQUI.h"

using std::list;
using std::string;


#define SPACING                 6       // between subwidgets
#define MARGIN                  4       // around the widget


YQPkgSearchFilterView::YQPkgSearchFilterView( QWidget * parent )
    : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    _matchCount = 0;
    layout->setMargin( MARGIN );
    layout->setSpacing( SPACING );

    layout->addStretch();

    // Headline
    QLabel * label = new QLabel( _( "Searc&h:" ), this );
    Q_CHECK_PTR( label );
    layout->addWidget(label);
    label->setFont( YQUI::yqApp()->headingFont() );

    // Input field ( combo box ) for search text
    _searchText = new QComboBox( this );
    Q_CHECK_PTR( _searchText );
    layout->addWidget(_searchText);
    _searchText->setEditable( true );
    label->setBuddy( _searchText );


    // Box for search button
    QHBoxLayout * hbox = new QHBoxLayout( this );
    Q_CHECK_PTR( hbox );
    layout->addLayout(hbox);
    hbox->addStretch();

    // Search button
    _searchButton = new QPushButton( _( "&Search" ), this );
    Q_CHECK_PTR( _searchButton );
    hbox->addWidget(_searchButton);

    connect( _searchButton, SIGNAL( clicked() ),
             this,          SLOT  ( filter()  ) );

    layout->addStretch();

    //
    // Where to search
    //
    
    QGroupBox * gbox = new QGroupBox( _( "Search in" ), this );
    Q_CHECK_PTR( gbox );
    QVBoxLayout *vlayout = new QVBoxLayout;
    gbox->setLayout(vlayout);

    _searchInName        = new QCheckBox( _( "&Name" 		), gbox ); Q_CHECK_PTR( _searchInName        );
    vlayout->addWidget(_searchInName);
    _searchInSummary     = new QCheckBox( _( "Su&mmary" 	), gbox ); Q_CHECK_PTR( _searchInSummary     );
    vlayout->addWidget(_searchInSummary);
    _searchInDescription = new QCheckBox( _( "Descr&iption"	), gbox ); Q_CHECK_PTR( _searchInDescription );
    vlayout->addWidget(_searchInDescription);

    vlayout->addStretch();

    // Intentionally NOT marking RPM tags for translation
    _searchInProvides    = new QCheckBox(  "RPM \"&Provides\""   , gbox ); Q_CHECK_PTR( _searchInProvides    );
    vlayout->addWidget(_searchInProvides);
    _searchInRequires    = new QCheckBox(  "RPM \"Re&quires\""   , gbox ); Q_CHECK_PTR( _searchInRequires    );
    vlayout->addWidget(_searchInRequires);

    _searchInName->setChecked( true );
    _searchInSummary->setChecked( true );

    layout->addStretch();


    //
    // Search mode
    //

    label = new QLabel( _( "Search &Mode:" ), this );
    Q_CHECK_PTR( label );
    layout->addWidget(label);

    _searchMode = new QComboBox( this );
    Q_CHECK_PTR( _searchMode );
    layout->addWidget(_searchMode);

    _searchMode->setEditable( false );

    label->setBuddy( _searchMode );

    // Caution: combo box items must be inserted in the same order as enum SearchMode!
    _searchMode->addItem( _( "Contains"		 ) );
    _searchMode->addItem( _( "Begins with"		 ) );
    _searchMode->addItem( _( "Exact Match"		 ) );
    _searchMode->addItem( _( "Use Wild Cards" 	 ) );
    _searchMode->addItem( _( "Use Regular Expression" ) );

    _searchMode->setCurrentIndex( Contains );


    layout->addStretch();

    _caseSensitive = new QCheckBox( _( "Case Sensiti&ve" ), this );
    Q_CHECK_PTR( _caseSensitive );
    layout->addWidget(_caseSensitive);

    for ( int i=0; i < 6; i++ )
      layout->addStretch();
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
	if ( event->modifiers() == Qt::NoModifier )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		_searchButton->animateClick();
		return;
	    }
	}

    }

    QWidget::keyPressEvent( event );
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
          0,
				  1000,
				  this			// parent
				  );
	progress.setWindowTitle( "" );
	progress.setMinimumDuration( 2000 ); // millisec
	QTime timer;

	QRegExp regexp( _searchText->currentText() );
	regexp.setCaseSensitivity( _caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );
  regexp.setPatternSyntax( (_searchMode->currentIndex() == UseWildcards) ? QRegExp::Wildcard : QRegExp::RegExp);

	timer.start();


	int count = 0;

	for ( ZyppPoolIterator it = zyppPkgBegin();
	      it != zyppPkgEnd() && ! progress.wasCanceled();
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


	    progress.setValue( count++ );

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
    QRegExp regexp( _searchText->currentText() );
    regexp.setCaseSensitivity( _caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );
    regexp.setPatternSyntax( (_searchMode->currentIndex() == UseWildcards) ? QRegExp::Wildcard : QRegExp::RegExp);
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

    switch ( _searchMode->currentIndex() )
    {
	case Contains:
	    match = att.contains( searchText, _caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
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
