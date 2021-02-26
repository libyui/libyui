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

  Textdomain "qt-pkg"

/-*/

#include <QMessageBox>
#include <QKeyEvent>

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include "YQApplication.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"

using std::max;
using std::string;


YQPackageSelectorBase::YQPackageSelectorBase( YWidget * parent,
					      long	modeFlags )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YPackageSelector( parent, modeFlags )
{
    setWidgetRep( this );

    _showChangesDialog		= false;
    _pkgConflictDialog		= 0;
    _diskUsageList		= 0;
    _pkgConflictDialog 		= 0;

    YQUI::setTextdomain( "qt-pkg" );
    setFont( YQUI::yqApp()->currentFont() );

    _pkgConflictDialog = new YQPkgConflictDialog( this );
    Q_CHECK_PTR( _pkgConflictDialog );

    QString label = _( "Reset &Ignored Dependency Conflicts" );
    _actionResetIgnoredDependencyProblems = new QAction( label, this);
    _actionResetIgnoredDependencyProblems->setShortcut((QKeySequence) 0);
    //_actionResetIgnoredDependencyProblems->setMenuRole(QAction::TextHeuristicRole);
    Q_CHECK_PTR( _actionResetIgnoredDependencyProblems );

    connect( _actionResetIgnoredDependencyProblems, SIGNAL( activated() ),
	     this,				    SLOT  ( resetIgnoredDependencyProblems() ) );

    zyppPool().saveState<zypp::Package  >();
    zyppPool().saveState<zypp::Pattern  >();
    zyppPool().saveState<zypp::Patch    >();


    // Install event handler to handle WM_CLOSE like "Cancel"
    topLevelWidget()->installEventFilter( this );

    yuiMilestone() << "PackageSelectorBase init done" << endl;
}


YQPackageSelectorBase::~YQPackageSelectorBase()
{
    yuiMilestone() << "Destroying PackageSelector" << endl;
}


int
YQPackageSelectorBase::resolveDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	yuiError() << "No package conflict dialog existing" << endl;
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
	yuiError() << "No package conflict dialog existing" << endl;
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
    resolveDependencies();

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

    YQPkgChangesDialog::showChangesDialog( this,
					   msg,
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
	zyppPool().diffState<zypp::Patch    >();

    if ( changes )
    {
	if ( zyppPool().diffState<zypp::Package>() )
	    yuiMilestone() << "diffState() reports changed packages" << endl;

	if ( zyppPool().diffState<zypp::Pattern>() )
	    yuiMilestone() << "diffState() reports changed patterns" << endl;

	if ( zyppPool().diffState<zypp::Patch>() )
	    yuiMilestone() << "diffState() reports changed patches" << endl;
    }

    if ( ! changes ||
	 ( QMessageBox::warning( this, "",
				 _( "Abandon all changes?" ),
				 _( "&Abandon" ), _( "&Cancel" ), "",
				 1, // defaultButtonNumber (from 0)
				 1 ) // escapeButtonNumber
	   == 0 )	// Proceed upon button #0 ( OK )
	 )
    {
	zyppPool().restoreState<zypp::Package  >();
	zyppPool().restoreState<zypp::Pattern  >();
	zyppPool().restoreState<zypp::Patch    >();

	yuiMilestone() << "Closing PackageSelector with \"Cancel\"" << endl;
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
	if ( resolveDependencies() == QDialog::Rejected )
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

	if ( YQPkgChangesDialog::showChangesDialog( this, msg, _( "C&ontinue" ), _( "&Cancel" ) )
	     == QDialog::Rejected )
	    return;
    }

    // Check disk usage
    if ( checkDiskUsage() == QDialog::Rejected )
	return;

    yuiMilestone() << "Closing PackageSelector with \"Accept\"" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "accept" ) );
}

void
YQPackageSelectorBase::repoManager()
{
    yuiMilestone() << "Closing PackageSelector with \"RepoManager\"" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "repo_mgr" ) );
}

bool
YQPackageSelectorBase::showPendingLicenseAgreements()
{
    yuiMilestone() << "Showing all pending license agreements" << endl;

    bool allConfirmed = true;

    if ( onlineUpdateMode() )
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
			yuiMilestone() << "Resolvable " << sel->name() << " has a license agreement" << endl;

			if( ! sel->hasLicenceConfirmed() )
			{
			    yuiDebug() << "Showing license agreement for resolvable " << sel->name() << endl;
			    allConfirmed = YQPkgObjListItem::showLicenseAgreement( sel ) && allConfirmed;
			}
			else
			{
			    yuiMilestone() << "Resolvable " << sel->name()
					   << "'s  license is already confirmed" << endl;
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
	Qt::KeyboardModifiers special_combo = ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier );

	if ( ( event->modifiers() & special_combo ) == special_combo )
	{
	    if ( event->key() == Qt::Key_A )
	    {
		showAutoPkgList();
		event->accept();
		return;
	    }
	}
    }

    QWidget::keyPressEvent( event );
}


bool YQPackageSelectorBase::eventFilter( QObject * obj, QEvent * event )
{
    if ( event->type() == QEvent::Close )
    {
	// Handle WM_CLOSE like "Cancel"
	reject();

	return true;	// Stop processing this event
    }

    return false;	// Don't stop processing this event
}


int YQPackageSelectorBase::preferredWidth()
{
    return max( 640, sizeHint().width() );
}


int YQPackageSelectorBase::preferredHeight()
{
    return max( 480, sizeHint().height() );
}


void
YQPackageSelectorBase::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void
YQPackageSelectorBase::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


bool
YQPackageSelectorBase::setKeyboardFocus()
{
    setFocus();

    return true;
}


#include "YQPackageSelectorBase.moc"
