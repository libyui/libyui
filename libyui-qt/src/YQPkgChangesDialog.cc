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

#include "YQZypp.h"
#include <zypp/ResPoolProxy.h>
#include "YQPkgChangesDialog.h"
#include "YQPkgList.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "YQUI.h"


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

    // Limit dialog size to available screen size
    setMaximumSize( qApp->desktop()->availableGeometry().size() );

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

    filter();
}


void
YQPkgChangesDialog::filter( bool byAuto, bool byApp, bool byUser )
{
    YQUI::ui()->busyCursor();
    _pkgList->clear();

#ifdef FIXME
    PMManager::SelectableVec::const_iterator it = Y2PM::packageManager().begin();

    while ( it != Y2PM::packageManager().end() )
    {
	Selectable::Ptr selectable = *it;

	if ( selectable->to_modify() )
	{
	    if ( selectable->by_auto() && byAuto ||
		 selectable->by_appl() && byApp  ||
		 selectable->by_user() && byUser   )
	    {
		_pkgList->addPkgItem( selectable, selectable->theObj() );
	    }
	}

	++it;
    }
#endif

    YQUI::ui()->normalCursor();
}


void
YQPkgChangesDialog::filter( const QRegExp & regexp, bool byAuto, bool byApp, bool byUser )
{
    YQUI::ui()->busyCursor();
    _pkgList->clear();

#ifdef FIXME
    PMManager::SelectableVec::const_iterator it = Y2PM::packageManager().begin();

    while ( it != Y2PM::packageManager().end() )
    {
	Selectable::Ptr selectable = *it;

	if ( selectable->to_modify() )
	{
	    if ( selectable->by_auto() && byAuto ||
		 selectable->by_appl() && byApp  ||
		 selectable->by_user() && byUser   )
	    {
		if ( regexp.match( selectable->name().asString().c_str() ) >= 0 )
		    _pkgList->addPkgItem( selectable, selectable->theObj() );
	    }
	}

	++it;
    }
#endif

    YQUI::ui()->normalCursor();
}


bool
YQPkgChangesDialog::isEmpty() const
{
    return _pkgList->firstChild() == 0;
}


QSize
YQPkgChangesDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
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


bool
YQPkgChangesDialog::showChangesDialog( const QString & 	message,
				       const QRegExp &  regexp,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
				       bool		showIfListEmpty	  )
{
    YQPkgChangesDialog dialog( 0,
			       message,
			       acceptButtonLabel,
			       rejectButtonLabel );
    dialog.filter( regexp, true, true, true );

    if ( dialog.isEmpty() && ! showIfListEmpty )
	return true;

    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}




#include "YQPkgChangesDialog.moc"
