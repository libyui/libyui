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

  File:	      YQPackageSelectorBase.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPackageSelectorBase_h
#define YQPackageSelectorBase_h

#include <QEvent>
#include <QFrame>
#include "YPackageSelector.h"
#include "YQZypp.h"


class QY2ComboTabWidget;
class QAction;

class YQPkgConflictDialog;
class YQPkgDiskUsageList;


/**
 * Abstract base class for package selectors.
 **/
class YQPackageSelectorBase : public QFrame, public YPackageSelector
{
    Q_OBJECT

protected:

    /**
     * Constructor
     *
     * Will initialize package and selection managers and create conflict
     * dialogs.
     **/
    YQPackageSelectorBase( YWidget * parent, long modeFlags = 0 );

    /**
     * Destructor
     **/
    virtual ~YQPackageSelectorBase();


public:
    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabling( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Event filter to handle WM_CLOSE like "Cancel"
     *
     * Reimplemented from QObject.
     **/
    virtual bool eventFilter( QObject * obj, QEvent * event );

    
public slots:

    /**
     * Resolve dependencies (unconditionally) for all resolvables.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int resolveDependencies();

    /**
     * Verifies dependencies of the currently installed system.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int verifySystem();

    /**
     * Check for disk overflow and post a warning dialog if necessary.
     * The user can choose to override this warning.
     *
     * Returns QDialog::Accepted if no warning is necessary or if the user
     * wishes to override the warning, QDialog::Rejected otherwise.
     **/
    int checkDiskUsage();

    /**
     * Display a list of automatically selected packages
     * (excluding packages contained in any selections that are to be installed)
     **/
    void showAutoPkgList();

    /**
     * Close processing and abandon changes
     **/
    void reject();

    /**
     * Close processing and accept changes
     **/
    void accept();

    /**
     * Close processing and request start of the repository manager 
     **/
    void repoManager();

    /**
     * Inform user about a feature that is not implemented yet.
     * This should NEVER show up in the final version.
     **/
    void notImplemented();


signals:

    /**
     * Emitted when package resolving is started.
     * This can be used for some kind of "busy display".
     **/
    void resolvingStarted();

    /**
     * Emitted when package resolving is finished.
     **/
    void resolvingFinished();


protected slots:

    /**
     * Reset all previously ignored dependency problems.
     **/
    void resetIgnoredDependencyProblems();


protected:

    /**
     * Show all license agreements the user has not confirmed yet
     * (for all packages that will be installed, and in YOU mode also for
     * patches).
     *
     * Returns 'true' if all licenses were confirmed, 'false' if one or more
     * licenses were not confirmed (in which case some packages might be set to
     * S_TABOO, which might require another resolver run).
     **/
    bool showPendingLicenseAgreements();

    /**
     * Show all license agreements in a resolvable range. To be used with
     * zyppPkgBegin() and zyppPkgEnd() or with zyppPatchesBegin() and
     * zyppPatchesEnd().
     **/
    bool showPendingLicenseAgreements( ZyppPoolIterator begin,
				       ZyppPoolIterator end );

    /**
     * Event handler for keyboard input - for debugging and testing.
     *
     * Reimplemented from QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    bool			_showChangesDialog;
    YQPkgConflictDialog *	_pkgConflictDialog;
    YQPkgDiskUsageList *	_diskUsageList;
    QAction *			_actionResetIgnoredDependencyProblems;
};



#endif // YQPackageSelectorBase_h
