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

  File:	      YQPackageSelectorBase.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <qmessagebox.h>

#include "YQZypp.h"
#include <zypp/ResPoolProxy.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2LayoutUtils.h"

#include "YQPackageSelectorBase.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgTextDialog.h"

#include "YQDialog.h"
#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


using std::max;
using std::string;

#define MIN_WIDTH			640
#define MIN_HEIGHT			480



YQPackageSelectorBase::YQPackageSelectorBase( QWidget * 		parent,
					      const YWidgetOpt & 	opt	)
    : QVBox( parent )
    , YPackageSelector( opt )
{
    setWidgetRep( this );

    _showChangesDialog		= false;
    _pkgConflictDialog		= 0;
    _diskUsageList		= 0;
    _pkgConflictDialog 		= 0;
    _youMode	 		= opt.youMode.value();

    YQUI::setTextdomain( "packages-qt" );
    setFont( YQUI::ui()->currentFont() );
    YQUI::ui()->blockWmClose(); // Automatically undone after UI::RunPkgSelection()

    _pkgConflictDialog = new YQPkgConflictDialog( this );
    CHECK_PTR( _pkgConflictDialog );

#ifdef FIXME
    Y2PM::packageManager().SaveState();

    if ( ! _youMode )
	Y2PM::selectionManager().SaveState();
#endif

    //
    // Handle WM_CLOSE like "Cancel"
    //

    connect( YQUI::ui(),	SIGNAL( wmClose() ),
	     this,		SLOT  ( reject()   ) );

    y2milestone( "PackageSelectorBase init done" );
}



int
YQPackageSelectorBase::resolvePackageDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	y2error( "No package conflict dialog existing" );
	return QDialog::Accepted;
    }


    YQUI::ui()->busyCursor();
    emit resolvingStarted();

    int result = _pkgConflictDialog->solveAndShowConflicts();

    emit resolvingFinished();
    YQUI::ui()->normalCursor();

    return result;
}


int
YQPackageSelectorBase::checkDiskUsage()
{
    if ( ! _diskUsageList )
    {
#if 1
	y2warning( "No disk usage list existing, assuming disk usage is OK" );
#endif
	return QDialog::Accepted;
    }

    if ( ! _diskUsageList->overflowWarning.inRange() )
	return QDialog::Accepted;

    QString msg =
	// Translators: RichText ( HTML-like ) format
	"<p><b>" + _( "Error: Out of disk space!" ) + "</b></p>"
	+ _( "<p>"
	     "You can choose to install anyway if you know what you are doing, "
	     "but you risk getting a corrupted system that requires manual repairs. "
	     "If you are not absolutely sure how to handle such a case, "
	     "press <b>Cancel</b> now and deselect some packages."
	     "</p>" );

    return YQPkgDiskUsageWarningDialog::diskUsageWarning( msg,
							  100, _( "C&ontinue Anyway" ), _( "&Cancel" ) );

}



void
YQPackageSelectorBase::showAutoPkgList()
{
    resolvePackageDependencies();

    // Show which packages are installed/deleted automatically
    QString msg =
	"<p><b>"
	// Dialog header
	+ _( "Automatic Changes" )
	+ "</b></p>"
	// Detailed explanation ( automatic word wrap! )
	+ "<p>"
	+ _( "In addition to your manual selections, the following packages"
	     " have been changed to resolve dependencies:" )
	+ "<p>";

    YQPkgChangesDialog::showChangesDialog( msg,
					   _( "&OK" ),
					   QString::null,	// rejectButtonLabel
					   true );		// showIfEmpty
}



void
YQPackageSelectorBase::reject()
{
#ifdef FIXME
    bool changes = Y2PM::packageManager().DiffState();

    if ( _youMode )
	changes |= Y2PM::youPatchManager().DiffState();
    else
	changes |= Y2PM::selectionManager().DiffState();

    if ( ! changes ||
	 ( QMessageBox::warning( this, "",
				 _( "Abandon all changes?" ),
				 _( "&OK" ), _( "&Cancel" ), "",
				 1, // defaultButtonNumber (from 0)
				 1 ) // escapeButtonNumber
	   == 0 )	// Proceed upon button #0 ( OK )
	 )
    {
	Y2PM::packageManager().RestoreState();

	if ( _youMode )
	    Y2PM::youPatchManager().RestoreState();
	else
	    Y2PM::selectionManager().RestoreState();

	YQUI::ui()->sendEvent( new YCancelEvent() );
    }
#endif
}


void
YQPackageSelectorBase::accept()
{
    if ( ! _youMode )
    {
	// Force final dependency resolving
	if ( resolvePackageDependencies() == QDialog::Rejected )
	    return;
    }

    if ( _showChangesDialog )
    {
	// Show which packages are installed/deleted automatically
	QString msg =
	    "<p><b>"
	    // Dialog header
	    + _( "Automatic Changes" )
	    + "</b></p>"
	    // Detailed explanation ( automatic word wrap! )
	    + "<p>"
	    + _( "In addition to your manual selections, the following packages"
		 " have been changed to resolve dependencies:" )
	    + "<p>";

	if ( YQPkgChangesDialog::showChangesDialog( msg, _( "C&ontinue" ), _( "&Cancel" ) )
	     == QDialog::Rejected )
	    return;
    }

    // Check disk usage
    if ( checkDiskUsage() == QDialog::Rejected )
	return;


#ifdef FIXME
    Y2PM::packageManager().ClearSaveState();

    if ( _youMode )
	Y2PM::youPatchManager().ClearSaveState();
    else
	Y2PM::selectionManager().ClearSaveState();
#endif

    YQUI::ui()->sendEvent( new YMenuEvent( YCPSymbol( "accept" ) ) );
}


void
YQPackageSelectorBase::notImplemented()
{
    QMessageBox::information( this, "",
			      _( "Not implemented yet. Sorry." ),
			      QMessageBox::Ok );
}


void
YQPackageSelectorBase::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	unsigned special_combo = ( Qt::ControlButton | Qt::ShiftButton | Qt::AltButton );

	if ( ( event->state() & special_combo ) == special_combo )
	{
	    if ( event->key() == Qt::Key_A )
	    {
		showAutoPkgList();
		event->accept();
		return;
	    }
	}
	else if ( event->key() == Qt::Key_F5 )	// No matter if Ctrl/Alt/Shift pressed
	{
	    YQUI::ui()->easterEgg();
	    return;
	}
    }

    QWidget::keyPressEvent( event );
}


long
YQPackageSelectorBase::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	int hintWidth = sizeHint().width();

	return max( MIN_WIDTH, hintWidth );
    }
    else
    {
	int hintHeight = sizeHint().height();

	return max( MIN_HEIGHT, hintHeight );
    }
}


void
YQPackageSelectorBase::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void
YQPackageSelectorBase::setEnabling( bool enabled )
{
    setEnabled( enabled );
}


bool
YQPackageSelectorBase::setKeyboardFocus()
{
    setFocus();

    return true;
}


void
YQPackageSelectorBase::postBetaWarning()
{
    YQPkgTextDialog::showText( this,
			       "<h1>Warning</h1>"
			       "<p>"
			       "The package manager in this beta is not fully functional"
			       "since we integrated a new package dependency resolver.  "
			       "Selecting single packages, the resolver itself and conflict resolution"
			       "should work."
			       "</p><p>"
			       "Everything else in the graphical frontend is currently not working. "
			       "We are aware of that, and we are working on it."
			       "</p><p>"
			       "<b>Please only report bugs that you find in the resolver!</b>"
			       "</p>"
			       );
}



#include "YQPackageSelectorBase.moc"
