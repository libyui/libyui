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

/-*/


#define IGNORE_WARNING_THRESHOLD	5

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include <qpushbutton.h>
#include <qmessagebox.h>
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
    return QSize( 500, 450 );
}


void
YQPkgConflictDialog::solveAndShowConflicts()
{
    CHECK_PTR( _conflictList );

    if ( isVisible() )
    {
	_conflictList->applyResolutions();
    }

    _conflictList->clear();
    PkgDep::ResultList		goodList;
    PkgDep::ErrorResultList	badList;

    // y2milestone( "Solving..." );
    YUIQt::yuiqt()->busyCursor();

    bool success = Y2PM::packageManager().solveInstall( goodList, badList );

    YUIQt::yuiqt()->normalCursor();
    // y2milestone( "Solving done" );
    emit updatePackages();

    if ( success )
    {
	if ( isVisible() )
	    accept();
    }
    else
    {
	// y2milestone( "Dependency conflict!" );
	_conflictList->fill( badList );

	if ( _conflictList->isEmpty() ) // Maybe all conflicts are ignored
	{
	    if ( isVisible() )
		accept();
	}
	else // There are conflicts
	{
	    if ( ! isVisible() )
		exec();
	}
    }
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
