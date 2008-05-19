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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>

#include "YQZypp.h"
#include <zypp/ResStatus.h>
#include <zypp/ui/UserWantedPackages.h>

#include "YQPkgChangesDialog.h"
#include "YQPkgList.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "YQUI.h"

using std::set;
using std::string;


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgChangesDialog::YQPkgChangesDialog( QWidget *		parent,
					const QString & 	message,
					const QString &		acceptButtonLabel,
					const QString &		rejectButtonLabel )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Changed Packages" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Limit dialog size to available screen size
    setMaximumSize( qApp->desktop()->availableGeometry().size() );

    // Layout for the dialog ( can't simply insert a QVBox )

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    layout->setMargin(MARGIN);
    layout->setSpacing(SPACING);
    setLayout(layout);

    // HBox for icon and message

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    layout->addLayout( hbox );


    // Icon

    hbox->addSpacing(SPACING);
    QLabel * iconLabel = new QLabel( this );
    Q_CHECK_PTR( iconLabel );
    hbox->addWidget(iconLabel);
#ifdef FIXME
    iconLabel->setPixmap( QApplication::style().stylePixmap( QStyle::SP_MessageBoxInformation ) );
#endif
    iconLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert
    hbox->addSpacing(SPACING);

    // Label for the message

    QLabel * label = new QLabel( message, this );
    Q_CHECK_PTR( label );
    hbox->addWidget(label);
    label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert


    // Pkg list

    _pkgList = new YQPkgList( this );
    Q_CHECK_PTR( _pkgList );
    _pkgList->setEditable( false );

    layout->addWidget( _pkgList );


    // Button box

    hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );

    hbox->addStretch();


    // Accept button - usually "OK" or "Continue"

    QPushButton * button = new QPushButton( acceptButtonLabel, this );
    Q_CHECK_PTR( button );
    hbox->addWidget( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    hbox->addStretch();

    if ( ! rejectButtonLabel.isEmpty() )
    {
	// Reject button ( if desired ) - usually "Cancel"

	button = new QPushButton( rejectButtonLabel, this );
	Q_CHECK_PTR( button );
        hbox->addWidget(button);
	connect( button,	SIGNAL( clicked() ),
		 this,      	SLOT  ( reject()  ) );

	hbox->addStretch();
    }
}


void
YQPkgChangesDialog::filter( bool byAuto, bool byApp, bool byUser )
{
    filter( QRegExp( "" ), byAuto, byApp, byUser );
}


void
YQPkgChangesDialog::filter( const QRegExp & regexp, bool byAuto, bool byApp, bool byUser )
{
    YQUI::ui()->busyCursor();
    _pkgList->clear();

    set<string> ignoredNames;

    if ( ! byUser || ! byApp )
	ignoredNames = zypp::ui::userWantedPackageNames();

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel selectable = *it;

	if ( selectable->toModify() )
	{
	    zypp::ResStatus::TransactByValue modifiedBy = selectable->modifiedBy();

	    if ( ( ( modifiedBy == zypp::ResStatus::SOLVER     ) && byAuto ) ||
		 ( ( modifiedBy == zypp::ResStatus::APPL_LOW ||
		   modifiedBy == zypp::ResStatus::APPL_HIGH  ) && byApp ) ||
		 ( ( modifiedBy == zypp::ResStatus::USER       ) && byUser )  )
	    {
		if ( regexp.isEmpty() || regexp.indexIn( selectable->name().c_str() ) >= 0 )
		{
		    if ( ! contains( ignoredNames, selectable->name() ) )
			_pkgList->addPkgItem( selectable, tryCastToZyppPkg( selectable->theObj() ) );
		}
	    }
	}
    }

    YQUI::ui()->normalCursor();
}


bool
YQPkgChangesDialog::isEmpty() const
{
    return _pkgList->topLevelItemCount() == 0;
}


QSize
YQPkgChangesDialog::sizeHint() const
{
    return limitToScreenSize( this, QDialog::sizeHint() );
}


bool
YQPkgChangesDialog::showChangesDialog( QWidget *	parent,
				       const QString & 	message,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
				       bool		showIfListEmpty	  )
{
    YQPkgChangesDialog dialog( parent,
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
YQPkgChangesDialog::showChangesDialog( QWidget *	parent,
				       const QString & 	message,
				       const QRegExp &  regexp,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
				       bool		showIfListEmpty	  )
{
    YQPkgChangesDialog dialog( parent,
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
