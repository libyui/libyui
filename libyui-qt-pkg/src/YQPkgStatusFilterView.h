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

  File:	      YQPkgStatusFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgStatusFilterView_h
#define YQPkgStatusFilterView_h

#include "YQZypp.h"
#include <QWidget>
#include <QRegExp>
#include <QPixmap>
#include <QScrollArea>


class QComboBox;
class QCheckBox;
class QPushButton;


/**
 * @short Filter view for packages that made problems during update
 **/
class YQPkgStatusFilterView : public QScrollArea
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgStatusFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgStatusFilterView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Check if pkg matches the filter criteria.
     **/
    bool check( ZyppSel	selectable,
		ZyppObj 	pkg );


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *	  filterStart()
     *	  filterMatch() for each pkg that matches the filter
     *	  filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


    /**
     * Reset all check boxes (set them all to "off")
     **/
    void clear();
    
    /**
     * Set up the check boxes so all pending transactions are displayed.
     **/
    void showTransactions();

    /**
     * Set up the check boxes so pending manual transactions (no "auto" states)
     * are displayed.
     **/
    void showManualTransactions();
    
    /**
     * Set up the check boxes so pending automatic transactions (the "auto" states)
     * are displayed.
     **/
    void showAutoTransactions();

    /**
     * Set the check boxes for locked packages ("Taboo" and "Protected") to "on".
     **/
    void showLocks();

    /**
     * Set the check box for installed packages to "on".
     **/
    void showInstalled();

    /**
     * Set the check box for not installed packages to "on".
     **/
    void showNotInstalled();

    
signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:

    /**
     * Add a check box
     **/
    QCheckBox * addStatusCheckBox( QWidget * 		parent,
				   const QString & 	label,
				   const QPixmap &	icon,
				   bool 		initiallyChecked );

    // Data members

    QCheckBox *		_showAutoDel;
    QCheckBox *		_showAutoInstall;
    QCheckBox *		_showAutoUpdate;
    QCheckBox *		_showDel;
    QCheckBox *		_showInstall;
    QCheckBox *		_showKeepInstalled;
    QCheckBox *		_showNoInst;
    QCheckBox *		_showTaboo;
    QCheckBox *		_showProtected;
    QCheckBox *		_showUpdate;

    QPushButton *	_refreshButton;
};



#endif // ifndef YQPkgStatusFilterView_h
