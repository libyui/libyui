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

  File:	      YQPkgObjList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgObjList_h
#define YQPkgObjList_h

#include <QY2ListView.h>
#include <y2pm/PMObject.h>


class YQPkgObjListItem;
class QAction;
class QPopupMenu;


/**
 * @short Abstract base class to display a list of PMObjects.
 * Handles most generic stuff like setting status etc.
 **/
class YQPkgObjList : public QY2ListView
{
    Q_OBJECT

protected:
    /**
     * Constructor. Does not add any QListView columns!
     **/
    YQPkgObjList( QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgObjList();


public:

    // Column numbers

    int statusCol()		const	{ return _statusCol;		}
    int nameCol()		const	{ return _nameCol;		}
    int summaryCol()		const	{ return _summaryCol;		}
    int sizeCol()		const	{ return _sizeCol;		}
    int versionCol()		const	{ return _versionCol;		}
    int instVersionCol()	const	{ return _instVersionCol;	}

    /**
     * Return whether or not items in this list are generally editable,
     * i.e. the user can change their status. Note that individual items can be
     * set to non-editable even if the list is generally editable.
     * Lists are editable by default.
     **/
    bool editable() const { return _editable; }

    /**
     * Set the list's editable status.
     **/
    void setEditable( bool editable = true ) { _editable = editable; }

    /**
     * Sets the currently selected item's status.
     * Automatically selects the next item if 'selectNextItem' is 'true'.
     **/
    void setCurrentStatus( PMSelectable::UI_Status	newStatus,
			   bool				selectNextItem = false );

public slots:

    /**
     * Add a PMObject to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     *
     * Intentionally NOT named addItem() so the calling class cannot confuse
     * this method with overlaid methods of the same name that were simply
     * forgotten to implement!
     **/
    void addPkgObjItem( PMObjectPtr pmObj );


    /**
     * Dispatcher slot for mouse click: cycle status depending on column.
     **/
    virtual void pkgObjClicked( int		button,
				QListViewItem *	item,
				int		col,
				const QPoint &	pos );

    /**
     * Reimplemented from QY2ListView:
     * Emit selectionChanged() signal after clearing the list.
     **/
    virtual void clear();

    /**
     * Update the internal actions: What actions are available for 'item'?
     **/
    virtual void updateActions( YQPkgObjListItem * item );

    /**
     * Select the next item, i.e. move the selection one item further down the
     * list.
     **/
    void selectNextItem();

    /**
     * Emit a statusChanged() signal for the specified PMObject.
     **/
    void sendStatusChanged( PMObjectPtr pmObj ) { emit statusChanged( pmObj ); }


    // Direct access to some states for menu actions

    void setCurrentInstall()	   { setCurrentStatus( PMSelectable::S_Install	     ); }
    void setCurrentDontInstall()   { setCurrentStatus( PMSelectable::S_NoInst	     ); }
    void setCurrentKeepInstalled() { setCurrentStatus( PMSelectable::S_KeepInstalled ); }
    void setCurrentDelete()	   { setCurrentStatus( PMSelectable::S_Del	     ); }
    void setCurrentUpdate()	   { setCurrentStatus( PMSelectable::S_Update	     ); }
    void setCurrentTaboo()	   { setCurrentStatus( PMSelectable::S_Taboo	     ); }


protected slots:

    /**
     * Dispatcher slot for selection change - internal only.
     **/
    virtual void selectionChangedInternal( QListViewItem * item );


signals:


    /**
     * Emitted when a PMObject is selected.
     * May be called with a null poiner if no PMObject is selected.
     **/
    void selectionChanged( PMObjectPtr pmObj );

    /**
     * Emitted when the status of a PMObject is changed.
     **/
    void statusChanged( PMObjectPtr pmObj );


protected:

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Reimplemented from QListView / QWidget.
     */
    void keyPressEvent( QKeyEvent * ev );

    /**
     * Returns the context menu for items that are not installed.
     * Creates the menu upon the first call.
     **/
    virtual QPopupMenu * installedContextMenu();

    /**
     * Returns the context menu for items that are installed.
     * Creates the menu upon the first call.
     **/
    virtual QPopupMenu * notInstalledContextMenu();

    /**
     * Create the context menu for items that are not installed.
     **/
    virtual void createNotInstalledContextMenu();

    /**
     * Create the context menu for installed items.
     **/
    virtual void createInstalledContextMenu();

    /**
     * Create the actions for the context menus.
     **/
    void createActions();

    /**
     * Create one action
     **/
    QAction * createAction( const QPixmap &	icon,
			    const QString &	text,
			    bool		enabled = true );


    // Data members

    int 	_statusCol;
    int 	_nameCol;
    int 	_summaryCol;
    int 	_sizeCol;
    int 	_versionCol;
    int 	_instVersionCol;
    bool	_editable;

    QAction *		_actionSetCurrentInstall;
    QAction *		_actionSetCurrentDontInstall;
    QAction *		_actionSetCurrentKeepInstalled;
    QAction *		_actionSetCurrentDelete;
    QAction *		_actionSetCurrentUpdate;
    QAction *		_actionSetCurrentTaboo;

    QAction *		_actionSetCurrentAutoInstall;
    QAction *		_actionSetCurrentAutoUpdate;
    QAction *		_actionSetCurrentAutoDelete;

    QPopupMenu * 	_installedContextMenu;
    QPopupMenu *	_notInstalledContextMenu;
};



class YQPkgObjListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor. Creates a YQPkgObjList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgObjListItem( YQPkgObjList * pkgObjList, PMObjectPtr pmObj );

    /**
     * Destructor
     **/
    virtual ~YQPkgObjListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    PMObjectPtr pmObj() { return _pmObj; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const PMObjectPtr constPMObj() const { return _pmObj; }

    /**
     * Return whether or not this items is editable, i.e. the user can change
     * its status. This requires the corresponding list to be editable, too.
     * Items are editable by default.
     **/
    bool editable() const { return _editable; }

    /**
     * Set this item's editable status.
     **/
    void setEditable( bool editable = true ) { _editable = editable; }

    /**
     * Returns the (binary RPM) package status
     **/
    PMSelectable::UI_Status status() const;

    /**
     * Set the (binary RPM) package status
     **/
    virtual void setStatus( PMSelectable::UI_Status newStatus );

    /**
     * Update this item's status.
     * Triggered by QY2ListView::updateAllItemStates().
     * Overwritten from QY2ListViewItem.
     **/
    virtual void updateStatus();

    /**
     * Set a status icon according to the package's status.
     **/
    virtual void setStatusIcon();

    /**
     * Cycle the package status to the next valid value.
     **/
    virtual void cycleStatus();

    /**
     * Check if the candidate is newer than the installed version.
     **/
    bool candidateIsNewer() const { return _candidateIsNewer; }

    /**
     * Check if the installed version is newer than the candidate.
     **/
    bool installedIsNewer() const { return _installedIsNewer; }

    /**
     * Set a column text via STL string.
     * (QListViewItem::setText() expects a QString!)
     **/
    void setText( int column, const std::string text );

    /**
     * Re-declare ordinary setText() method so the compiler doesn't get
     * confused which one to use.
     **/
    void setText( int column, const QString & text )
	{ QListViewItem::setText( column, text ); }

    /**
     * Set a column text via PkgEdition.
     **/
    void setText( int column, const PkgEdition & edition );

    /**
     * Display this item's notify text (if there is any) that corresponds to
     * the specified status (S_Install, S_Del) in a pop-up window.
     **/
    void showNotifyTexts( PMSelectable::UI_Status status );

    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    /**
     * Update this item's data completely.
     * Triggered by QY2ListView::updateAllItemData().
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void updateData();


    // Columns

    int statusCol()		const	{ return _pkgObjList->statusCol();	}
    int nameCol()		const	{ return _pkgObjList->nameCol();	}
    int summaryCol()		const	{ return _pkgObjList->summaryCol();	}
    int sizeCol()		const	{ return _pkgObjList->sizeCol();	}
    int versionCol()		const	{ return _pkgObjList->versionCol();	}
    int instVersionCol()	const	{ return _pkgObjList->instVersionCol();	}


protected:

    /**
     * Initialize internal data and set fields accordingly.
     **/
    void init();


    // Data members

    YQPkgObjList *	_pkgObjList;
    PMObjectPtr		_pmObj;
    bool		_editable;
    bool		_candidateIsNewer;
    bool		_installedIsNewer;
};



#endif // ifndef YQPkgObjList_h
