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

// -*- c++ -*-

#ifndef YQPackageSelector_h
#define YQPackageSelector_h

#include <qvbox.h>
#include <qcolor.h>
#include <ycp/YCPString.h>

#include "YQPackageSelectorBase.h"


class QCheckBox;
class QComboBox;
class QLabel;
class QListView;
class QProgressBar;
class QPushButton;
class QSplitter;
class QTabWidget;
class QPopupMenu;
class QMenuBar;

class QY2ComboTabWidget;

class YQPkgDependenciesView;
class YQPkgDescriptionView;
class YQPkgDiskUsageList;
class YQPkgInstSrcFilterView;
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
class YQPkgYouPatchFilterView;
class YQPkgYouPatchList;


class YQPackageSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQPackageSelector( QWidget *		parent,
		       const YWidgetOpt & 	opt,
		       const YCPString &	floppyDevice );


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


protected:

    // Layout methods - create and layout widgets

    void basicLayout();

    QWidget *	layoutLeftPane		( QWidget * parent );
    QWidget *	layoutRightPane		( QWidget * parent );
    void	layoutFilters		( QWidget * parent );
    void 	layoutPkgList		( QWidget * parent );
    void 	layoutDetailsViews	( QWidget * parent );
    void 	layoutButtons		( QWidget * parent );
    void 	layoutMenuBar		( QWidget * parent );

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

    bool				_searchMode;
    bool				_testMode;
    bool				_updateMode;
    bool				_summaryMode;
    QString				_floppyDevice;

    QCheckBox *				_autoDependenciesCheckBox;
    QPushButton *			_checkDependenciesButton;
    QTabWidget *			_detailsViews;
    QY2ComboTabWidget *			_filters;
    YQPkgDependenciesView *		_pkgDependenciesView;
    YQPkgDescriptionView *		_pkgDescriptionView;
    YQPkgInstSrcFilterView *		_instSrcFilterView;
    YQPkgLangList *			_langList;
    YQPkgList *				_pkgList;
    YQPkgPatternList *			_patternList;
    YQPkgRpmGroupTagsFilterView *	_rpmGroupTagsFilterView;
    YQPkgSearchFilterView *		_searchFilterView;
    YQPkgSelList *			_selList;
    YQPkgStatusFilterView *		_statusFilterView;
    YQPkgTechnicalDetailsView *		_pkgTechnicalDetailsView;
    YQPkgUpdateProblemFilterView *	_updateProblemFilterView;
    YQPkgVersionsView *			_pkgVersionsView;
    YQPkgYouPatchFilterView *		_youPatchFilterView;
    YQPkgYouPatchList *			_youPatchList;

    QMenuBar *				_menuBar;
    QPopupMenu *			_fileMenu;
    QPopupMenu *			_viewMenu;
    QPopupMenu *			_pkgMenu;
    QPopupMenu *			_youPatchMenu;
    QPopupMenu *			_extrasMenu;
    QPopupMenu *			_helpMenu;

    QColor				_normalButtonBackground;
};



#endif // YQPackageSelector_h
