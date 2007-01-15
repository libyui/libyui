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

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qaction.h>
#include "QY2LayoutUtils.h"

#include "YQPackageSelectorBase.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgTextDialog.h"
#include "YQPkgObjList.h"

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

    QString label = _( "Reset &Ignored Dependency Conflicts" );
    _actionResetIgnoredDependencyProblems = new QAction( label,			// text
							 label,			// menu text
							 (QKeySequence) 0,	// accel
							 this ); 		// parent
    CHECK_PTR( _actionResetIgnoredDependencyProblems );

    connect( _actionResetIgnoredDependencyProblems, SIGNAL( activated() ),
	     this,				    SLOT  ( resetIgnoredDependencyProblems() ) );

    zyppPool().saveState<zypp::Package  >();
    zyppPool().saveState<zypp::Pattern  >();
    zyppPool().saveState<zypp::Selection>();
    zyppPool().saveState<zypp::Language >();
    zyppPool().saveState<zypp::Patch    >();


    //
    // Handle WM_CLOSE like "Cancel"
    //

    connect( YQUI::ui(),	SIGNAL( wmClose() ),
	     this,		SLOT  ( reject()   ) );

    y2milestone( "PackageSelectorBase init done" );
}


YQPackageSelectorBase::~YQPackageSelectorBase()
{
    y2milestone( "Destroying PackageSelector" );
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
YQPackageSelectorBase::verifySystem()
{
    if ( ! _pkgConflictDialog )
    {
	y2error( "No package conflict dialog existing" );
	return QDialog::Accepted;
    }


    YQUI::ui()->busyCursor();
    int result = _pkgConflictDialog->verifySystem();
    YQUI::ui()->normalCursor();
    
    if ( result == QDialog::Accepted )
    {
	QMessageBox::information( this, "",
				  _( "System dependencies verify OK." ),
				  QMessageBox::Ok );
    }

    return result;
}


int
YQPackageSelectorBase::checkDiskUsage()
{
    if ( ! _diskUsageList )
    {
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
    bool changes =
	zyppPool().diffState<zypp::Package  >()	||
	zyppPool().diffState<zypp::Pattern  >()	||
	zyppPool().diffState<zypp::Selection>() ||
	zyppPool().diffState<zypp::Language >() ||
	zyppPool().diffState<zypp::Patch    >();

    if ( changes )
    {
	if ( zyppPool().diffState<zypp::Package>() )
	    y2milestone( "diffState() reports changed packages" );

	if ( zyppPool().diffState<zypp::Pattern>() )
	    y2milestone( "diffState() reports changed patterns" );

	if ( zyppPool().diffState<zypp::Selection>() )
	    y2milestone( "diffState() reports changed selections" );

	if ( zyppPool().diffState<zypp::Language>() )
	    y2milestone( "diffState() reports changed languages" );

	if ( zyppPool().diffState<zypp::Patch>() )
	    y2milestone( "diffState() reports changed patches" );
    }

    if ( ! changes ||
	 ( QMessageBox::warning( this, "",
				 _( "Abandon all changes?" ),
				 _( "&OK" ), _( "&Cancel" ), "",
				 1, // defaultButtonNumber (from 0)
				 1 ) // escapeButtonNumber
	   == 0 )	// Proceed upon button #0 ( OK )
	 )
    {
	zyppPool().restoreState<zypp::Package  >();
	zyppPool().restoreState<zypp::Pattern  >();
	zyppPool().restoreState<zypp::Selection>();
	zyppPool().restoreState<zypp::Language >();
	zyppPool().restoreState<zypp::Patch    >();

	y2milestone( "Closing PackageSelector with \"Cancel\"" );
	YQUI::ui()->sendEvent( new YCancelEvent() );
    }
}


void
YQPackageSelectorBase::accept()
{
    bool confirmedAllLicenses;

    do
    {
	// Force final dependency resolving
	if ( resolvePackageDependencies() == QDialog::Rejected )
	    return;

	confirmedAllLicenses = showPendingLicenseAgreements();

    } while ( ! confirmedAllLicenses ); // Some packages will be set to S_TABOO - need another solver run

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

    y2milestone( "Closing PackageSelector with \"Accept\"" );
    YQUI::ui()->sendEvent( new YMenuEvent( YCPSymbol( "accept" ) ) );
}


bool
YQPackageSelectorBase::showPendingLicenseAgreements()
{
    y2milestone( "Showing all pending license agreements" );

    bool allConfirmed = true;

    if ( _youMode )
	allConfirmed = showPendingLicenseAgreements( zyppPatchesBegin(), zyppPatchesEnd() );

    allConfirmed = showPendingLicenseAgreements( zyppPkgBegin(), zyppPkgEnd() ) && allConfirmed;

    return allConfirmed;
}


bool
YQPackageSelectorBase::showPendingLicenseAgreements( ZyppPoolIterator begin, ZyppPoolIterator end )
{
    bool allConfirmed = true;
    
    for ( ZyppPoolIterator it = begin; it != end; ++it )
    {
	ZyppSel sel = (*it);

	switch ( sel->status() )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:

		if ( sel->candidateObj() )
		{
		    string licenseText = sel->candidateObj()->licenseToConfirm();

		    if ( ! licenseText.empty() )
		    {
			y2milestone( "Resolvable %s has a license agreement", sel->name().c_str() );

			if( ! sel->hasLicenceConfirmed() )
			{
			    y2debug( "Showing license agreement for resolvable %s", sel->name().c_str() );
			    allConfirmed = YQPkgObjListItem::showLicenseAgreement( sel ) && allConfirmed;
			}
			else
			{
			    y2milestone( "Resolvable %s's  license is already confirmed", sel->name().c_str() );
			}
		    }
		}
		break;

	    default:
		break;
	}
    }

    return allConfirmed;
}


void
YQPackageSelectorBase::notImplemented()
{
    QMessageBox::information( this, "",
			      _( "Not implemented yet. Sorry." ),
			      QMessageBox::Ok );
}


void
YQPackageSelectorBase::resetIgnoredDependencyProblems()
{
    YQPkgConflictDialog::resetIgnoredDependencyProblems();
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


#include "YQPackageSelectorBase.moc"
