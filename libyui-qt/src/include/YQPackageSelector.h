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

class QY2ComboTabWidget;

class YQPkgConflictDialog;
class YQPkgDescriptionView;
class YQPkgList;
class YQPkgRpmGroupTagsFilterView;
class YQPkgSelList;
class YQPkgSelectionsFilterView;
class YQPkgTechnicalDetailsView;
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


public slots:

    /**
     * Resolve package dependencies (unconditionally).
     **/
    void resolveDependencies();

    /**
     * Automatically resolve package dependencies if desired
     * (if the "auto check" checkbox is on).
     **/
    void autoResolveDependencies();

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

    // FIXME
    void preAlphaWarning();

    
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
    void layoutDiskSpaceSummary	( QWidget * parent );
    void layoutRightPane	( QWidget * parent );
    void layoutPkgList		( QWidget * parent );
    void layoutDetailsViews	( QWidget * parent );
    void layoutButtons		( QWidget * parent );
    void layoutMenuBar		( QWidget * parent );

    void makeConnections();
    void fakeData();
    

    // Data members

    YUIQt *				_yuiqt;
    bool				_youMode;
    bool				_updateMode;
    bool				_testMode;

    QWidget *				_leftPane;
    QY2ComboTabWidget * 		_filters;
    QProgressBar *			_diskSpace;
    YQPkgList *			 	_pkgList;
    QTabWidget *			_detailsViews;
    YQPkgDescriptionView *		_pkgDescriptionView;
    YQPkgTechnicalDetailsView *		_pkgTechnicalDetailsView;
    QCheckBox *			 	_autoDependenciesCheckBox;
    YQPkgRpmGroupTagsFilterView *	_rpmGroupTagsFilterView;
    YQPkgSelectionsFilterView *		_selectionsFilterView;
    YQPkgSelList *			_selList;
    YQPkgYouPatchFilterView *		_youPatchFilterView;
    YQPkgYouPatchList *			_youPatchList;
    YQPkgConflictDialog *		_conflictDialog;
};

#endif // YQPackageSelector_h
