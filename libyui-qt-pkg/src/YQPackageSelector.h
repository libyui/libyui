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

  File:	      YQPackageSelector.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPackageSelector_h
#define YQPackageSelector_h

#include <QColor>
#include <QLabel>
#include <QMenu>

#include "YQPackageSelectorBase.h"
#include "YQPkgObjList.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QListWidget;
class QProgressBar;
class QPushButton;
class QSplitter;
class QTabWidget;
class QMenu;
class QMenuBar;

class QY2ComboTabWidget;

class YQPkgChangeLogView;
class YQPkgDependenciesView;
class YQPkgDescriptionView;
class YQPkgDiskUsageList;
class YQPkgFileListView;
class YQPkgRepoFilterView;
class YQPkgLangList;
class YQPkgList;
class YQPkgPatternList;
class YQPkgRpmGroupTagsFilterView;
class YQPkgSearchFilterView;
class YQPkgSelList;
class YQPkgStatusFilterView;
class YQPkgTechnicalDetailsView;
class YQPkgUpdateProblemFilterView;
class YQPkgVersionsView;
class YQPkgPatchFilterView;
class YQPkgPatchList;

class YQPackageSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQPackageSelector( YWidget * parent, long modeFlags = 0 );


public slots:

    /**
     * Resolve package dependencies manually.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
     int manualResolvePackageDependencies();

    /**
     * Automatically resolve package dependencies if desired
     * (if the "auto check" checkbox is on).
     **/
    void autoResolveDependencies();

    /**
     * Export all current selection/package states
     **/
    void pkgExport();

    /**
     * Import selection/package states
     **/
    void pkgImport();

    /**
     * Install any -devel package for packages that are installed or marked for
     * installation
     **/
    void installDevelPkgs();

    /**
     * Install any -debuginfo package for packages that are installed or marked
     * for installation
     **/
    void installDebugInfoPkgs();

    /**
     * Install any subpackage that ends with 'suffix' for packages that are
     * installed or marked for installation
     **/
    void installSubPkgs( const QString suffix );

    /**
     * Enable or disable the package exclude rules (show or suppress -debuginfo
     * or -devel packages) according to the current menu settings and apply the
     * exclude rules.
     **/
    void pkgExcludeDebugChanged( bool on );
    void pkgExcludeDevelChanged( bool on );

    /**
     * Display (generic) online help.
     **/
    void help();

    /**
     * Display online help about symbols (package status icons).
     **/
    void symbolHelp();

    /**
     * Display online help about magic keys.
     **/
    void keyboardHelp();


signals:

    /**
     * Emitted once (!) when the dialog is about to be shown, when all widgets
     * are created and all signal/slot connections are set up - when it makes
     * sense to load data.
     **/
    void loadData();

    /**
     * Emitted when the internal data base might have changed and a refresh of
     * all displayed data might be necessary - e.g., when saved (exported) pkg
     * states are reimported.
     **/
    void refresh();


protected slots:

    /**
     * Animate the "Check" button when dependency resolving is in progress,
     * i.e. change its background color
     **/
    void animateCheckButton();

    /**
     * Restore the normal background color of the "Check" button.
     **/
    void restoreCheckButton();

    /**
     * Add the "Patches" filter view, if it is not already there.
     **/
    void addPatchFilterView();

    /**
     * Add the "Patches" filter view upon hotkey (F2).
     **/
    void hotkeyInsertPatchFilterView();

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update", if there is
     * a candidate package that is newer than the installed package.
     **/
    void globalUpdatePkg() { globalUpdatePkg( false ); }

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update", even if the
     * candidate package is not newer than the installed package.
     **/
    void globalUpdatePkgForce() { globalUpdatePkg( true ); }

    /**
     * Show all products in a popup dialog.
     **/
    void showProducts();

public:
    /**
     * returns the full path for an icon of a given size
     */
    static std::string iconPath( const std::string &name, int size );

protected:

    // Layout methods - create and layout widgets

    void basicLayout();

    QWidget *	layoutLeftPane		( QWidget *parent );
    QWidget *	layoutRightPane		( QWidget *parent );
    void	layoutFilters		( QWidget *parent );
    void 	layoutPkgList		( QWidget *parent );
    void 	layoutDetailsViews	( QWidget *parent );
    void 	layoutButtons		( QWidget *parent );
    void 	layoutMenuBar		( QWidget *parent );

    /**
     * Establish Qt signal / slot connections.
     *
     * This really needs to be a separate method to make sure all affected
     * wigets are created at this point.
     **/
    void makeConnections();

    /**
     * Add pulldown menus to the menu bar.
     *
     * This really needs to be a separate method to make sure all affected
     * wigets are created at this point.
     **/
    void addMenus();

    /**
     * Connect a filter view that provides the usual signals with a package
     * list. By convention, filter views provide the following signals:
     *	  filterStart()
     *	  filterMatch()
     *	  filterFinished()
     *	  updatePackages()  (optional)
     **/
    void connectFilter( QWidget *	filter,
			QWidget *	pkgList,
			bool		hasUpdateSignal = true );

    /**
     * Connect the patch list. Caution: Possible bootstrap problem!
     **/
    void connectPatchList();

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update" and switch to
     * the "Installation Summary" view afterwards.
     *
     * 'force' indicates if this should also be done if the the candidate
     * package is not newer than the installed package.
     **/
    void globalUpdatePkg( bool force );

    /**
     * Import one selectable: Set its status according to 'isWanted'
     * based on its old status.
     * 'kind' is 'package' or 'pattern' (used only for debug logging).
     **/
    void importSelectable( ZyppSel	selectable,
			   bool		isWanted,
			   const char * kind );



    /**
     * Return HTML code describing a symbol (an icon).
     **/
    QString symHelp( const QString & imgFileName,
		     const QString & summary,
		     const QString & explanation	);


    /**
     * Return HTML code describing a key.
     **/
    QString keyHelp( const QString & key,
		     const QString & summary,
		     const QString & explanation	);

    /**
     * Basic HTML formatting: Embed text into <p> ... </p>
     **/
    static QString para( const QString & text );

    /**
     * Basic HTML formatting: Embed text into <li> ... </li>
     **/
    static QString listItem( const QString & text );


    // Data members

    QCheckBox *				_autoDependenciesCheckBox;
    QPushButton *			_checkDependenciesButton;
    QTabWidget *			_detailsViews;
    QY2ComboTabWidget *			_filters;
    YQPkgChangeLogView *		_pkgChangeLogView;
    YQPkgDependenciesView *		_pkgDependenciesView;
    YQPkgDescriptionView *		_pkgDescriptionView;
    YQPkgFileListView *			_pkgFileListView;
    YQPkgRepoFilterView *		_repoFilterView;
    YQPkgLangList *			_langList;
    YQPkgList *				_pkgList;
    YQPkgPatternList *			_patternList;
    YQPkgRpmGroupTagsFilterView *	_rpmGroupTagsFilterView;
    YQPkgSearchFilterView *		_searchFilterView;
    YQPkgStatusFilterView *		_statusFilterView;
    YQPkgTechnicalDetailsView *		_pkgTechnicalDetailsView;
    YQPkgUpdateProblemFilterView *	_updateProblemFilterView;
    YQPkgVersionsView *			_pkgVersionsView;
    YQPkgPatchFilterView *		_patchFilterView;
    YQPkgPatchList *			_patchList;

    QMenuBar *				_menuBar;
    QMenu *			_fileMenu;
    QMenu *			_viewMenu;
    QMenu *			_pkgMenu;
    QMenu *			_patchMenu;
    QMenu *			_repositoryMenu;
    QMenu *			_extrasMenu;
    QMenu *			_helpMenu;

    QAction *_showDevelAction;
    QAction *_showDebugAction;

    YQPkgObjList::ExcludeRule *		_excludeDevelPkgs;
    YQPkgObjList::ExcludeRule *		_excludeDebugInfoPkgs;

    QColor				_normalButtonBackground;
};



#endif // YQPackageSelector_h
