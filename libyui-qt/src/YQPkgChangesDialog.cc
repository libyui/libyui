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

  File:	      YQPkgChangesDialog.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qapplication.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstyle.h>

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include "YQPkgChangesDialog.h"
#include "YQPkgList.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "YUIQt.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgChangesDialog::YQPkgChangesDialog( QWidget *		parent,
					const QString & 	message,
					const QString &		acceptButtonLabel,
					const QString &		rejectButtonLabel )
    : QDialog( parent )
{
    // Dialog title
    setCaption( _( "Changed Packages" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog ( can't simply insert a QVBox )

    QVBoxLayout * layout = new QVBoxLayout( this, MARGIN, SPACING );
    CHECK_PTR( layout );


    // HBox for icon and message

    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );
    layout->addWidget( hbox );


    // Icon

    addHSpacing( hbox );
    QLabel * iconLabel = new QLabel( hbox );
    CHECK_PTR( iconLabel );
    iconLabel->setPixmap( QApplication::style().stylePixmap( QStyle::SP_MessageBoxInformation ) );
    iconLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert
    addHSpacing( hbox );

    // Label for the message

    QLabel * label = new QLabel( message, hbox );
    CHECK_PTR( label );
    label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert


    // Pkg list

    _pkgList = new YQPkgList( this );
    CHECK_PTR( _pkgList );
    _pkgList->setEditable( false );

    layout->addWidget( _pkgList );


    // Button box

    hbox = new QHBox( this );
    CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addWidget( hbox );

    addHStretch( hbox );


    // Accept button - usually "OK" or "Continue"

    QPushButton * button = new QPushButton( acceptButtonLabel, hbox );
    CHECK_PTR( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    addHStretch( hbox );


    if ( ! rejectButtonLabel.isEmpty() )
    {
	// Reject button ( if desired ) - usually "Cancel"

	button = new QPushButton( rejectButtonLabel, hbox );
	CHECK_PTR( button );

	connect( button,	SIGNAL( clicked() ),
		 this,      	SLOT  ( reject()  ) );

	addHStretch( hbox );
    }

#if 0
    // If there is only one button, it's safe to make that one ( the accept
    // button ) the default. If there are two, better be safe than sorry and
    // make the reject button the default.

    button->setDefault( true );
#endif

    filter();
}


void
YQPkgChangesDialog::filter( bool byAuto, bool byApp, bool byUser )
{
    YUIQt::ui()->busyCursor();
    _pkgList->clear();

    PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();

    while ( it != Y2PM::packageManager().end() )
    {
	PMSelectablePtr selectable = *it;

	if ( selectable->to_modify() )
	{
	    if ( selectable->by_auto() && byAuto ||
		 selectable->by_appl() && byApp  ||
		 selectable->by_user() && byUser   )
	    {
		_pkgList->addPkgItem( selectable->theObject() );
	    }
	}

	++it;
    }
    
    YUIQt::ui()->normalCursor();
}


bool
YQPkgChangesDialog::isEmpty() const
{
    return _pkgList->firstChild() == 0;
}


bool
YQPkgChangesDialog::showChangesDialog( const QString & 	message,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
				       bool		showIfListEmpty	  )
{
    YQPkgChangesDialog dialog( 0,
			       message,
			       acceptButtonLabel,
			       rejectButtonLabel );
    dialog.filter();

    if ( dialog.isEmpty() && ! showIfListEmpty )
	return true;

    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}




#include "YQPkgChangesDialog.moc.cc"
