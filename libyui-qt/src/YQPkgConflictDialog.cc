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


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include <qpushbutton.h>
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
    addHStretch( buttonBox );


    // OK button

    _okButton = new QPushButton( _( "&OK - Try Again" ), buttonBox );
    CHECK_PTR( _okButton );
    _okButton->setDefault( true );

    connect( _okButton, SIGNAL( clicked() ),
	     this,      SLOT  ( solveAndShowConflicts() ) );

    addHStretch( buttonBox );

    // Cancel button

    _cancelButton = new QPushButton( _( "&Cancel" ), buttonBox );
    CHECK_PTR( _cancelButton );

    connect( _cancelButton, SIGNAL( clicked() ),
	     this,          SLOT  ( reject()  ) );
    
    addHStretch( buttonBox );
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



#include "YQPkgConflictDialog.moc.cc"
