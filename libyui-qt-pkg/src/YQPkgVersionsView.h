/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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
#include <QCheckBox>
#include <QButtonGroup>
#include <QBoxLayout>
#include <QLabel>
#include <QList>

#include "YQZypp.h"


class QTabWidget;
class YQPkgMultiVersion;
class YQPkgVersion;


/**
 * @short Package version selector: Display a list of available versions from
 * all the different installation sources and let the user change the candidate
 * version for installation / update.
 **/
class YQPkgVersionsView: public QScrollArea
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQPkgVersionsView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgVersionsView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Return 'true' if 'selectable' has mixed multiversion flags,
     * 'false' if all its pool items are of the same kind
     * (all multiversion or all non-multiversion).
     **/
    static bool isMixedMultiVersion( ZyppSel selectable );

    /**
     * Return the cached value for the current selectable.
     **/
    bool isMixedMultiVersion() const { return _isMixedMultiVersion; }

    /**
     * Negotiate between multiversion and non-multiversion packages if there
     * are both kinds in that selectable. 'newSelected' is the item the user
     * chose to install.
     *
     * This returns 'true' if status setting etc. is already handled inside
     * this function, 'false' otherwise.
     **/
    bool handleMixedMultiVersion( YQPkgMultiVersion * newSelected );

    ZyppSel selectable() const { return _selectable; }


public slots:

    /**
     * Show details for the specified package.
     * Delayed ( optimized ) display if this is embedded into a QTabWidget
     * parent: In this case, wait until this page becomes visible.
     **/
    void showDetailsIfVisible( ZyppSel selectable );

    // slot clear() inherited from QListView


    /**
     * Show data for the current package.
     **/
    void reload( int newCurrent );


signals:

    /**
     * Emitted when the user changes the candidate.
     **/
    void candidateChanged( ZyppObj newCandidate );

    /**
     * Emitted when the status of any package changed.
     **/
    void statusChanged();


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

    /**
     * Add a YQPkgVersion radio button and return it.
     **/
    YQPkgVersion * addAvailable( ZyppSel selectable, ZyppObj zyppObj );

    /**
     * Ask user if he really wants to install incompatible package versions.
     * Return 'true' if he hits [Continue], 'false' if [Cancel].
     **/
    bool mixedMultiVersionPopup( bool multiversion ) const;

    /**
     * Check if any package version is marked for installation where its
     * 'multiversion' flag is set to 'multiversion'.
     **/
    bool anyMultiVersionToInstall( bool multiversion ) const;

    /**
     * Unselect all multiversion package versions.
     **/
    void unselectAllMultiVersion();

    // Data members

    QWidget	*	_content;
    QTabWidget	*	_parentTab;
    ZyppSel		_selectable;
    bool		_isMixedMultiVersion;
    QButtonGroup *	_buttons;
    QList<QWidget*>	_installed;
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
		  ZyppObj	zyppObj );

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

    ZyppSel	_selectable;
    ZyppObj	_zyppObj;
};



class YQPkgMultiVersion: public QCheckBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQPkgMultiVersion( YQPkgVersionsView * parent,
		       ZyppSel		   selectable,
		       ZyppPoolItem	   zyppPoolItem );

    /**
     * Destructor
     **/
    virtual ~YQPkgMultiVersion();

    /**
     * Returns the original ZYPP object
     **/
    ZyppPoolItem zyppPoolItem() const { return _zyppPoolItem; }

    /**
     * Returns the original ZYPP selectable
     **/
    ZyppSel selectable() const { return _selectable; }

    /**
     * Paints checkboxes with status icons instead of a checkmark
     **/
    void paintEvent(QPaintEvent *);

signals:

    /**
     * Emitted when the status of this package version is changed.
     **/
    void statusChanged();


protected slots:
    void slotIconClicked();


protected:

    /**
     * Cycle the package status to the next valid value.
     **/
    void cycleStatus();

    void setStatus( ZyppStatus newStatus );
    QPixmap statusIcon( ZyppStatus status );

    //
    // Data members
    //

    YQPkgVersionsView * _parent;
    ZyppSel		_selectable;
    ZyppPoolItem	_zyppPoolItem;
};




#endif // ifndef YQPkgVersionsView_h
