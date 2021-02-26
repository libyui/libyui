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


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qpainter.h>

#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"
#include "YQDialog.h"

#include "QY2LayoutUtils.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "utf8.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


// The busy dialog ("Checking Dependencies") will only be shown if solving
// (on average) takes longer than this many seconds. The first one will be
// shown in any case.

#define SUPPRESS_BUSY_DIALOG_SECONDS	1.5


YQPkgConflictDialog::YQPkgConflictDialog( QWidget * parent )
    : QDialog( parent )
{
    _solveCount		= 0;
    _totalSolveTime	= 0.0;


    // Set the dialog title.
    //
    // "Dependency conflict" is already used as the conflict list header just
    // some pixels below that, so don't use this twice. This dialog title may
    // or may not be visible, depending on whether or not there is a window
    // manager running (and configured to show any dialog titles).

    setCaption( _( "Warning" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );


    // Layout for the dialog (can't simply insert a QVbox)

    QVBoxLayout * layout = new QVBoxLayout( this, MARGIN, SPACING );
    CHECK_PTR( layout );

    // Conflict list

    _conflictList = new YQPkgConflictList( this );
    CHECK_PTR( _conflictList );
    layout->addWidget( _conflictList );
    layout->addSpacing(8);

    connect( _conflictList, SIGNAL( updatePackages() ),
	     this,	    SIGNAL( updatePackages() ) );


    // Button box

    QHBox * buttonBox	= new QHBox( this );
    CHECK_PTR( buttonBox );
    buttonBox->setSpacing( SPACING );
    buttonBox->setMargin ( MARGIN  );
    layout->addWidget( buttonBox );


    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK -- Try Again" ), buttonBox );
    CHECK_PTR( button );
    button->setDefault( true );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( solveAndShowConflicts() ) );

    addHStretch( buttonBox );


    // "Expert" menu button

    button = new QPushButton( _( "&Expert" ), buttonBox );
    CHECK_PTR( button );

    addHStretch( buttonBox );


    // "Expert" menu

    _expertMenu = new QPopupMenu( button );
    CHECK_PTR( _expertMenu );
    button->setPopup( _expertMenu );

    _expertMenu->insertItem( _( "&Save This List to a File..." ),
			     _conflictList, SLOT( askSaveToFile() ) );


    // "Cancel" button

    button = new QPushButton( _( "&Cancel" ), buttonBox );
    CHECK_PTR( button );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( reject()  ) );


    // Busy popup

    _busyPopup = new QLabel( "   " + _( "Checking Dependencies..." ) + "   ", parent, 0,
			     WType_Dialog );
    CHECK_PTR( _busyPopup );
    _busyPopup->setCaption( "" );
    _busyPopup->resize( _busyPopup->sizeHint() );
    YQDialog::center( _busyPopup, parent );


    // Here comes a real nasty hack.
    //
    // The busy popup is needed to indicate that the application is (you
    // guessed right) busy. But as long as it is busy, it doesn't process X
    // events, either, and I didn't manage to convince Qt to please paint this
    // popup before the solver's calculations (which take quite a while) start
    // - all combinations of show(), repaint(), XSync(), XFlush(),
    // processEvents() etc. failed.
    //
    // So, let's do it the hard way: Give this popup a background pixmap into
    // which we render the text to display. The X server draws background
    // pixmaps immediately, so we don't have to wait until the X server, the
    // window manager and this application are finished negotiating all their
    // various events.

    // Create a pixmap. Make it large enough so it isn't replicated (i.e. the
    // text is displayed several times) if some window manager chooses not to
    // honor the size hints (KDM for example uses double the height we
    // request).

    QSize size = _busyPopup->sizeHint();
    QPixmap pixmap( 3 * size.width(), 3 * size.height() );

    // Clear the pixmap with the widget's normal background color.
    pixmap.fill( _busyPopup->paletteBackgroundColor() );

    // Render the text - aligned top and left because otherwise it will of
    // course be centered inside the pixmap which is usually much larger than
    // the popup, thus the text would be cut off.
    QPainter painter( &pixmap );
    painter.drawText( pixmap.rect(), AlignLeft | AlignTop, _busyPopup->text() );
    painter.end();

    _busyPopup->setPaletteBackgroundPixmap( pixmap );

    // If the application manages to render the true contents of the label we
    // just misused so badly, the real label will interfere with the background
    // pixmap with (maybe) a few pixels offset (bug #25647). Fast or
    // multiprocessor machines tend to have this problem.
    // So let's get rid of the label text and solely rely on the background
    // pixmap.
    _busyPopup->setText( "" );

    // Make sure the newly emptied text doesn't cause the busy dialog to be
    // resized to nil (or a window manager dependent minimum size).
    _busyPopup->setFixedSize( _busyPopup->size() );
}


YQPkgConflictDialog::~YQPkgConflictDialog()
{
    // NOP
}


QSize
YQPkgConflictDialog::sizeHint() const
{
    QSize size( 550, 450 );
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


int
YQPkgConflictDialog::solveAndShowConflicts()
{
    int result = QDialog::Accepted;
    CHECK_PTR( _conflictList );

    YQUI::ui()->busyCursor();

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

    if ( _solveCount++ == 0 || averageSolveTime() > SUPPRESS_BUSY_DIALOG_SECONDS )
    {
	YQDialog::center( _busyPopup, parentWidget() );
	_busyPopup->show();

	// No _busyPopup->repaint() - that doesn't help anyway: Qt doesn't do
	// any actual painting until the window is mapped. We just rely on the
	// background pixmap we provided in the constructor.

        // Make sure show() gets processed - usually, a window manager catches
        // the show() (XMap) events, positions and maybe resizes the window and
        // only then sends off an event that makes the window appear. This
        // event needs to be processed.
	qApp->processEvents();
    }

    y2debug( "Solving..." );
    QTime solveTime;
    solveTime.start();

    // Solve.

    zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();

    bool success = resolver->resolvePool();

    _totalSolveTime += solveTime.elapsed() / 1000.0;

    y2debug( "Solving done in %f s - average: %f s",
	     solveTime.elapsed() / 1000.0, averageSolveTime() );

    if ( _busyPopup->isVisible() )
	_busyPopup->hide();


    // Package states may have changed: The solver may have set packages to
    // autoInstall or autoUpdate. Make those changes known.
    emit updatePackages();

    YQUI::ui()->normalCursor();

    if ( success )	// Solving went without any complaints?
    {
	result = QDialog::Accepted;

	if ( isVisible() )
	    accept();	// Pop down the dialog.
    }
    else		// There were solving problems.
    {
	y2debug( "Dependency conflict!" );
	YQUI::ui()->busyCursor();

	_conflictList->fill( resolver->problems() );
	YQUI::ui()->normalCursor();

	if ( ! isVisible() )
	{
	    // Pop up the dialog and run a local event loop.
	    result = exec();
	}
    }

    return result;	// QDialog::Accepted or QDialog::Rejected
}


void
YQPkgConflictDialog::resetIgnoredDependencyProblems()
{
    zypp::getZYpp()->resolver()->undo();
}


double
YQPkgConflictDialog::averageSolveTime() const
{
    if ( _solveCount < 1 )
	return 0.0;

    return _totalSolveTime / _solveCount;
}




#include "YQPkgConflictDialog.moc"
