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

  File:	      YQPkgConflictDialog.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define IGNORE_WARNING_THRESHOLD	5

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>

#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qlayout.h>
#include <qhbox.h>

#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"
#include "YQPackageSelector.h"

#include "YUIQt.h"
#include "YQi18n.h"
#include "utf8.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgConflictDialog::YQPkgConflictDialog( QWidget * parent )
    : QDialog( parent )
{
    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );


    // Layout for the dialog (can't simply insert a QVbox)

    QVBoxLayout * layout = new QVBoxLayout( this, MARGIN, SPACING );
    CHECK_PTR( layout );

    // Conflict list

    _conflictList = new YQPkgConflictList( this );
    CHECK_PTR( _conflictList );
    layout->addWidget( _conflictList );
    layout->addSpacing( 8 );

    connect( _conflictList, SIGNAL( updatePackages() ),
	     this,	    SIGNAL( updatePackages() ) );


    // Button box

    QHBox * buttonBox	= new QHBox( this );
    CHECK_PTR( buttonBox );
    buttonBox->setSpacing( SPACING );
    buttonBox->setMargin ( MARGIN  );
    layout->addWidget( buttonBox );


    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK - Try Again" ), buttonBox );
    CHECK_PTR( button );
    button->setDefault( true );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( solveAndShowConflicts() ) );

    addHStretch( buttonBox );


    // "Ignore all" button

    button = new QPushButton( _( "&Ignore All" ), buttonBox );
    CHECK_PTR( button );

    connect( button,  SIGNAL( clicked() ),
	     this,    SLOT  ( ignoreAll() ) );

    addHStretch( buttonBox );


    // "Expert" menu button

    button = new QPushButton( _( "&Expert" ), buttonBox );
    CHECK_PTR( button );
    
    addHStretch( buttonBox );


    // "Expert" menu
    
    _expertMenu = new QPopupMenu( button );
    CHECK_PTR( _expertMenu );
    button->setPopup( _expertMenu );

    _expertMenu->insertItem( _( "&Save this list to file..." ),
			     _conflictList, SLOT( askSaveToFile() ) );


    // "Cancel" button

    button = new QPushButton( _( "&Cancel" ), buttonBox );
    CHECK_PTR( button );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( reject()  ) );
}


YQPkgConflictDialog::~YQPkgConflictDialog()
{
    // NOP
}


QSize
YQPkgConflictDialog::sizeHint() const
{
    return QSize( 550, 450 );
}


int
YQPkgConflictDialog::solveAndShowConflicts()
{
    CHECK_PTR( _conflictList );
    int result = QDialog::Accepted;

    if ( isVisible() )
    {
	// This is not only the starting point for all the dependency solving
	// magic, it is also used internally when clicking the "OK - Try again"
	// button. Thus, before doing anything else, check if the conflict list
	// still contains anything, and if so, apply any conflict resolutions
	// the user selected - but only if this dialog is already visible.

	_conflictList->applyResolutions();
    }


    // Initialize for next round of solving.
    _conflictList->clear();
    PkgDep::ResultList		goodList;
    PkgDep::ErrorResultList	badList;

    YUIQt::yuiqt()->busyCursor();
    y2debug( "Solving..." );

    // Solve.
    bool success = Y2PM::packageManager().solveInstall( goodList, badList );

    y2debug( "Solving done" );


    // Package states may have changed: The solver may have set packages to
    // autoInstall or autoUpdate. Make those changes known.
    emit updatePackages();

    YUIQt::yuiqt()->normalCursor();

    if ( success )	// Solving went without any complaints?
    {
	result = QDialog::Accepted;

	if ( isVisible() )
	    accept();	// Pop down the dialog.
    }
    else		// There were solving problems.
    {
	y2debug( "Dependency conflict!" );
	YUIQt::yuiqt()->busyCursor();

	// Make the bad list human readable and fill the conflictList
	// widget. During that process, filter out any conflicts the user
	// previously chose to ignore.

	_conflictList->fill( badList );
	YUIQt::yuiqt()->normalCursor();

	if ( _conflictList->isEmpty() ) // No conflicts?
	{
	    // Yes, this _can_ happen: The solver reported 'no success', yet
	    // the conflict list is empty. The badList still contains all
	    // ignored conflicts; the conflictList widget filters out the
	    // ignored ones.

	    if ( isVisible() )
		accept();	// Pop down the dialog.
	}
	else // There are conflicts
	{
	    if ( ! isVisible() )
	    {
		// Pop up the dialog and run a local event loop.
		result = exec();
	    }
	}
    }

    return result;	// QDialog::Accepted or QDialog::Rejected
}


void
YQPkgConflictDialog::ignoreAll()
{
    CHECK_PTR( _conflictList );

    if ( _conflictList->count() >= IGNORE_WARNING_THRESHOLD )
    {
	if ( QMessageBox::warning( this, "",
			       _( "\
Ignoring that many dependency problems\n\
may result in a really inconsistent system.\n\
\n\
Do this only if you know exactly what you are doing!\
" ),
			       _( "&Ignore Anyway" ), _( "&Cancel" ), "",
			       1, // defaultButtonNumber (from 0)
			       1 ) // escapeButtonNumber
	     == 1 )	// Proceed upon button #0 (OK)
	    return;
    }

    _conflictList->ignoreAll();
    solveAndShowConflicts();
}



#include "YQPkgConflictDialog.moc.cc"
