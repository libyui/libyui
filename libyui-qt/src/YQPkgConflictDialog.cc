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

#include <qapplication.h>
#include <qcursor.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qhbox.h>

#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"

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


    // Button box
    
    QHBox * buttonBox	= new QHBox( this );
    CHECK_PTR( buttonBox );
    buttonBox->setSpacing( SPACING );
    buttonBox->setMargin ( MARGIN  );
    layout->addWidget( buttonBox );


    // OK button
    
    _okButton = new QPushButton( _( "&OK - Try Again" ), buttonBox );
    CHECK_PTR( _okButton );
    _okButton->setDefault( true );
    
    connect( _okButton, SIGNAL( clicked() ),
	     this,      SLOT  ( solveAndShowConflicts() ) );

    
    // Strechtable space between the buttons
    
    QWidget * stretch = new QWidget( buttonBox );
    CHECK_PTR( stretch );
    stretch->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,	// hor
					 QSizePolicy::Minimum ) );	// vert

    // Cancel button
    
    _cancelButton = new QPushButton( _( "&Cancel - Ignore All" ), buttonBox );
    CHECK_PTR( _cancelButton );
    
    connect( _cancelButton, SIGNAL( clicked() ),
	     this,          SLOT  ( reject()  ) );
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
	_conflictList->activateUserChoices();
    }

    _conflictList->clear();
    PkgDep::ResultList		goodList;
    PkgDep::ErrorResultList	badList;

    y2milestone( "Solving..." );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    
    bool success = Y2PM::packageManager().solveInstall( goodList, badList );
    
    QApplication::restoreOverrideCursor();
    y2milestone( "Solving done" );

    if ( success )
    {
	if ( isVisible() )
	    accept();
    }
    else
    {
	y2milestone( "Dependency conflict!" );
	_conflictList->fill( badList );
	
	if ( ! isVisible() )
	{
	    exec();
	}
    }
}



#include "YQPkgConflictDialog.moc.cc"
