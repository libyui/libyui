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

  File:	      YQPkgVersionsView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgVersionsView_h
#define YQPkgVersionsView_h

#include <QScrollArea>
#include <QRadioButton>
#include <QButtonGroup>
#include <QBoxLayout>
#include <QLabel>
#include <QList>

#include "YQZypp.h"

class QTabWidget;


/**
 * @short Package version selector: Display a list of available versions from
 * all the different installation sources and let the user change the candidate
 * version for installation / update.
 **/
class YQPkgVersionsView : public QScrollArea
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * 'userCanSwitchVersions' specifies whether or not the user is allowed to
     * switch between package versions - e.g. in patch mode, he can't.
     **/
    YQPkgVersionsView( QWidget * parent, bool userCanSwitchVersions );

    /**
     * Destructor
     **/
    virtual ~YQPkgVersionsView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;


public slots:

    /**
     * Show details for the specified package.
     * Delayed ( optimized ) display if this is embedded into a QTabWidget
     * parent: In this case, wait until this page becomes visible.
     **/
    void showDetailsIfVisible( ZyppSel selectable );

    // slot clear() inherited from QListView


    /**
     * Show data for the last package.
     **/
    void reload( QWidget * newCurrent );


signals:

    /**
     * Emitted when the user changes the
     **/
    void candidateChanged( ZyppObj newCandidate );


protected slots:

    /**
     * Check for changed candidates
     **/
    void checkForChangedCandidate();


protected:

    /**
     * Show details for the specified package.
     **/
    void showDetails( ZyppSel selectable );


    // Data members

    QWidget     *	_content;
    QTabWidget	*	_parentTab;
    ZyppSel		_selectable;
    bool		_userCanSwitch;
    QButtonGroup *	_buttons;
    QList<QWidget*>     _installed;
    QVBoxLayout	*	_layout;
};


class YQPkgVersion: public QRadioButton
{
public:

    /**
     * Constructor. Creates a YQPkgVersion item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgVersion( QWidget *	parent,
		  ZyppSel	selectable,
		  ZyppObj 	zyppObj,
		  bool		enabled = true );

    /**
     * Destructor
     **/
    virtual ~YQPkgVersion();

    /**
     * Returns the original ZYPP object
     **/
    ZyppObj zyppObj() const { return _zyppObj; }

    /**
     * Returns the original ZYPP selectable
     **/
    ZyppSel selectable() const { return _selectable; }

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from QY2CheckListItem.
     **/
    virtual QString toolTip( int column );


protected:

    // Data members

    ZyppSel		_selectable;
    ZyppObj		_zyppObj;
};


#endif // ifndef YQPkgVersionsView_h
