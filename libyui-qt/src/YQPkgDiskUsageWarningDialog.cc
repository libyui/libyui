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

  File:	      YQPkgDiskUsageWarningDialog.cc

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

#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPackageSelector.h"
#include "Y2QtComponent.h"
#include "YQi18n.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgDiskUsageWarningDialog::YQPkgDiskUsageWarningDialog( QWidget *		parent,
							  const QString & 	message,
							  int			thresholdPercent,
							  const QString &	acceptButtonLabel,
							  const QString &	rejectButtonLabel )
    : QDialog( parent )
{
    // Dialog title
    setCaption( _( "Disk Space Warning" ) );

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
    iconLabel->setPixmap( QApplication::style().stylePixmap( QStyle::SP_MessageBoxWarning ) );
    iconLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert
    addHSpacing( hbox );

    // Label for the message

    QLabel * label = new QLabel( message, hbox );
    CHECK_PTR( label );
    label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert


    // Disk usage list

    YQPkgDiskUsageList * duList = new YQPkgDiskUsageList( this, thresholdPercent );
    CHECK_PTR( duList );

    layout->addWidget( duList );


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

    // If there is only one button, it's safe to make that one ( the accept
    // button ) the default. If there are two, better be safe than sorry and
    // make the reject button the default.

    button->setDefault( true );
}


bool
YQPkgDiskUsageWarningDialog::diskUsageWarning( const QString & 	message,
					       int		thresholdPercent,
					       const QString &	acceptButtonLabel,
					       const QString &	rejectButtonLabel )
{
    YQPkgDiskUsageWarningDialog dialog( 0,
					message,
					thresholdPercent,
					acceptButtonLabel,
					rejectButtonLabel );
    Y2QtComponent::ui()->normalCursor();
    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}




#include "YQPkgDiskUsageWarningDialog.moc"
