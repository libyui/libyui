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
#include <ycp/YCPString.h>

#include "YPackageSelector.h"


class QCheckBox;
class QComboBox;
class QLabel;
class QListView;
class QProgressBar;
class QSplitter;
class QTabWidget;
class QPopupMenu;
class QMenuBar;

class QY2ComboTabWidget;

class YQPkgConflictDialog;
class YQPkgDependenciesView;
class YQPkgDescriptionView;
class YQPkgDiskUsageList;
class YQPkgList;
class YQPkgRpmGroupTagsFilterView;
class YQPkgSearchFilterView;
class YQPkgSelList;
class YQPkgSelectionsFilterView;
class YQPkgStatusFilterView;
class YQPkgTechnicalDetailsView;
class YQPkgUpdateProblemFilterView;
class YQPkgVersionsView;
class YQPkgYouPatchFilterView;
class YQPkgYouPatchList;

class YUIQt;


class YQPackageSelector : public QVBox, public YPackageSelector
{
    Q_OBJECT

public:

    YQPackageSelector( YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     **/
    void setEnabling(bool enabled);

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     **/
    long nicesize(YUIDimension dim);

    /**
     * Sets the new size of the widget.
     **/
    void setSize(long newwidth, long newheight);

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Set the text domain.
     **/
    void setTextdomain( const char * domain );


public slots:

    /**
     * Resolve package dependencies (unconditionally).
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
     int resolvePackageDependencies();

    /**
     * Resolve selection dependencies (unconditionally).
     * Can safely be called even if there is no selection conflict dialog.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
     int resolveSelectionDependencies();

    /**
     * Automatically resolve package dependencies if desired
     * (if the "auto check" checkbox is on).
     **/
    void autoResolveDependencies();

    /**
     * Check for disk overflow and post a warning dialog if necessary.
     * The user can choose to override this warning.
     *
     * Returns QDialog::Accepted if no warning is necessary or if the user
     * wishes to override the warning, QDialog::Rejected otherwise.
     **/
    int checkDiskUsage();

    /**
     * Close processing and abandon changes
     **/
    void reject();

    /**
     * Close processing and accept changes
     **/
    void accept();

    /**
     * Display online help
     **/
    void help();


signals:

    /**
     * Emitted once (!) when the dialog is about to be shown, when all widgets
     * are created and all signal/slot connections are set up - when it makes
     * sense to load data.
     **/
    void loadData();


protected:

    // Layout methods - create and layout widgets

    void basicLayout();

    void layoutLeftPane		( QWidget * parent );
    void layoutFilters		( QWidget * parent );
    void layoutRightPane	( QWidget * parent );
    void layoutPkgList		( QWidget * parent );
    void layoutDetailsViews	( QWidget * parent );
    void layoutButtons		( QWidget * parent );
    void layoutMenuBar		( QWidget * parent );

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
     * Provide some fake data for testing
     **/
    void fakeData();

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


    // Data members

    YUIQt *				_yuiqt;
    bool				_youMode;
    bool				_updateMode;
    bool				_testMode;

    QCheckBox *				_autoDependenciesCheckBox;
    QProgressBar *			_diskSpace;
    QTabWidget *			_detailsViews;
    QY2ComboTabWidget *			_filters;
    YQPkgConflictDialog *		_pkgConflictDialog;
    YQPkgConflictDialog *		_selConflictDialog;
    YQPkgDependenciesView *		_pkgDependenciesView;
    YQPkgDescriptionView *		_pkgDescriptionView;
    YQPkgDiskUsageList *		_diskUsageList;
    YQPkgList *				_pkgList;
    YQPkgRpmGroupTagsFilterView *	_rpmGroupTagsFilterView;
    YQPkgSearchFilterView *		_searchFilterView;
    YQPkgSelList *			_selList;
    YQPkgSelectionsFilterView *		_selectionsFilterView;
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
    QPopupMenu *			_helpMenu;
};



/**
 * Add vertical stretchable space.
 **/
void addVStretch( QWidget * parent );

/**
 * Add horizontal stretchable space.
 **/
void addHStretch( QWidget * parent );


/**
 * Add a fixed height vertical space
 **/
void addVSpacing( QWidget * parent, int height = 8 );


/**
 * Add a fixed height horizontal space
 **/
void addHSpacing( QWidget * parent, int width = 8);


#endif // YQPackageSelector_h
