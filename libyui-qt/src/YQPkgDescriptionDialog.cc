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

  File:	      YQPkgDescriptionDialog.cc

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
#include <qsplitter.h>
#include <qstyle.h>
#include <qvaluelist.h>

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include "YQPkgDescriptionDialog.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgList.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "YUIQt.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgDescriptionDialog::YQPkgDescriptionDialog( QWidget * parent, const QString & pkgName )
    : QDialog( parent )
{
    // Dialog title
    setCaption( _( "Package Description" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout( this, MARGIN, SPACING );
    CHECK_PTR( layout );


    // VBox for splitter

    QSplitter * splitter = new QSplitter( QSplitter::Vertical, this );
    CHECK_PTR( splitter );
    layout->addWidget( splitter );
    splitter->setMargin( MARGIN );


    // Pkg list

    _pkgList = new YQPkgList( splitter );
    CHECK_PTR( _pkgList );
    _pkgList->resize( _pkgList->width(), 80 );

    
    // Description view

    _pkgDescription = new YQPkgDescriptionView( splitter );
    CHECK_PTR( _pkgDescription );
    _pkgDescription->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,		SIGNAL( selectionChanged    ( PMObjectPtr ) ),
	     _pkgDescription,	SLOT  ( showDetailsIfVisible( PMObjectPtr ) ) );


    // Button box (to center the single button)

    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addWidget( hbox );

    addHStretch( hbox );


    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK" ), hbox );
    CHECK_PTR( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    addHStretch( hbox );


    filter( pkgName );
}


void
YQPkgDescriptionDialog::filter( const QString & qPkgName )
{
    std::string pkgName( (const char *) qPkgName );
    YUIQt::ui()->busyCursor();
    _pkgList->clear();


    // Search for pkgs with that name

    PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();

    while ( it != Y2PM::packageManager().end() )
    {
	PMSelectablePtr selectable = *it;
	PMObjectPtr pmObj = selectable->theObject();

	if ( pmObj && pmObj->name() == pkgName )
	    _pkgList->addPkgItem( pmObj );

	++it;
    }


    // Display description of the first pkg with that name

    YQPkgObjListItem * firstItem = dynamic_cast<YQPkgObjListItem *> ( _pkgList->firstChild() );

    if ( firstItem )
	_pkgDescription->showDetailsIfVisible( firstItem->pmObj() );
    else
	_pkgDescription->clear();

    YUIQt::ui()->normalCursor();
}


bool
YQPkgDescriptionDialog::isEmpty() const
{
    return _pkgList->firstChild() == 0;
}


QSize
YQPkgDescriptionDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


void
YQPkgDescriptionDialog::showDescriptionDialog( const QString & pkgName )
{
    YQPkgDescriptionDialog dialog( 0, pkgName );
    dialog.exec();
}




#include "YQPkgDescriptionDialog.moc.cc"
