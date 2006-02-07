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

#include <qpixmap.h>
#include <QY2ListView.h>
#include <zypp/ResObject.h>
#include <zypp/ui/Selectable.h>
#include <y2util/FSize.h>

class YQPkgObjListItem;
class QAction;
class QPopupMenu;
using std::string;


/**
 * @short Abstract base class to display a list of zypp::ResObjects.
 * Handles most generic stuff like setting status etc.
 **/
class YQPkgObjList : public QY2ListView
{
    Q_OBJECT

protected:
    /**
     * Constructor. Does not add any QListView columns!
     **/
    YQPkgObjList( QWidget * parent );

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
    void setCurrentStatus( zypp::ui::Status	newStatus,
			   bool			selectNextItem = false );

    /**
     * Sets the status of all ( toplevel ) list items to 'newStatus', if possible.
     * Only one single statusChanged() signal is emitted.
     *
     * 'force' overrides sensible defaults like setting only zypp::ResObjects to
     * 'update' that really come with a newer version.
     **/
    void setAllItemStatus( zypp::ui::Status newStatus, bool force = false );

    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     **/
    virtual QPopupMenu * addAllInListSubMenu( QPopupMenu * menu );

    /**
     * Returns the suitable icon for a zypp::ResObject status - the regular icon if
     * 'enabled' is 'true' or the insensitive icon if 'enabled' is 'false.
     * 'bySelection' is relevant only for auto-states: This uses the icon for
     * 'auto-by-selection" rather than the default auto-icon.
     **/
    virtual QPixmap statusIcon( zypp::ui::Status status,
				bool 		enabled     = true,
				bool		bySelection = false );

    /**
     * Returns a short ( one line ) descriptive text for a zypp::ResObject status.
     **/
    virtual QString statusText( zypp::ui::Status status ) const;


public slots:

    /**
     * Add a zypp::ResObject to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     *
     * Intentionally NOT named addItem() so the calling class cannot confuse
     * this method with overlaid methods of the same name that were simply
     * forgotten to implement!
     **/
    void addPkgObjItem( zypp::ResObject::Ptr zyppObj );

    /**
     * Add a purely passive list item that has a name and optional summary and
     * size.
     **/
    void addPassiveItem( const QString & name,
			 const QString & summary = QString::null,
			 FSize 		 size    = -1 );

    /**
     * Dispatcher slot for mouse click: cycle status depending on column.
     **/
    virtual void pkgObjClicked( int		button,
				QListViewItem * item,
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
     * Update the internal actions for the currently selected item ( if any ).
     * This only calls updateActions( YQPkgObjListItem * ) with the currently
     * selected item as argument, so there is normally no need to reimplement
     * this method, too, if the other one is reimplemented.
     **/
    virtual void updateActions();

    /**
     * Select the next item, i.e. move the selection one item further down the
     * list.
     **/
    void selectNextItem();

    /**
     * Emit a statusChanged() signal for the specified zypp::ResObject.
     **/
    void sendStatusChanged() { emit statusChanged(); }

    /**
     * Display a one-line message in the list.
     **/
    virtual void message( const QString & text );


    // Direct access to some states for menu actions

    void setCurrentInstall()	   { setCurrentStatus( Status::S_Install	     	); }
    void setCurrentDontInstall()   { setCurrentStatus( Status::S_NoInst	     	); }
    void setCurrentKeepInstalled() { setCurrentStatus( Status::S_KeepInstalled	); }
    void setCurrentDelete()	   { setCurrentStatus( Status::S_Del	     	); }
    void setCurrentUpdate()	   { setCurrentStatus( Status::S_Update	     	); }
    void setCurrentTaboo()	   { setCurrentStatus( Status::S_Taboo	     	); }
    void setCurrentProtected()	   { setCurrentStatus( Status::S_Protected		); }

    void setListInstall()	   { setAllItemStatus( Status::S_Install		); }
    void setListDontInstall()	   { setAllItemStatus( Status::S_NoInst		); }
    void setListKeepInstalled()	   { setAllItemStatus( Status::S_KeepInstalled	); }
    void setListDelete()	   { setAllItemStatus( Status::S_Del		); }
    void setListUpdate()	   { setAllItemStatus( Status::S_Update		); }
    void setListUpdateForce()	   { setAllItemStatus( Status::S_Update, true  	); }
    void setListTaboo()		   { setAllItemStatus( Status::S_Taboo		); }
    void setListProtected()	   { setAllItemStatus( Status::S_Protected     	); }


protected slots:

    /**
     * Dispatcher slot for selection change - internal only.
     **/
    virtual void selectionChangedInternal( QListViewItem * item );


signals:


    /**
     * Emitted when a zypp::ResObject is selected.
     * May be called with a null poiner if no zypp::ResObject is selected.
     **/
    void selectionChanged( zypp::ResObject::Ptr zyppObj );

    /**
     * Emitted when the status of a zypp::ResObject is changed.
     **/
    void statusChanged();


protected:

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Reimplemented from QListView / QWidget.
     */
    virtual void keyPressEvent( QKeyEvent * ev );

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
     * Note: This is intentionally not virtual!
     **/
    void createActions();

    /**
     * Create an action based on a zypp::ResObject status - automatically retrieve the
     * corresponding status icons ( both sensitive and insensitive ) and text.
     * 'key' is only a descriptive text, no true accelerator.
     **/
    QAction * createAction( zypp::ui::Status 	status,
			    const QString &	key	= QString::null,
			    bool 		enabled = false );

    /**
     * Low-level: Create an action.
     * 'key' is only a descriptive text, no true accelerator.
     **/
    QAction * createAction( const QString & 	text,
			    const QPixmap & 	icon		= QPixmap(),
			    const QPixmap & 	insensitiveIcon	= QPixmap(),
			    const QString & 	key		= QString::null,
			    bool 		enabled		= false );


    // Data members

    int		_statusCol;
    int		_nameCol;
    int		_summaryCol;
    int		_sizeCol;
    int		_versionCol;
    int		_instVersionCol;
    bool	_editable;


    QPopupMenu *	_installedContextMenu;
    QPopupMenu *	_notInstalledContextMenu;


public:

    QAction *		actionSetCurrentInstall;
    QAction *		actionSetCurrentDontInstall;
    QAction *		actionSetCurrentKeepInstalled;
    QAction *		actionSetCurrentDelete;
    QAction *		actionSetCurrentUpdate;
    QAction *		actionSetCurrentTaboo;
    QAction *		actionSetCurrentProtected;

    QAction *		actionSetListInstall;
    QAction *		actionSetListDontInstall;
    QAction *		actionSetListKeepInstalled;
    QAction *		actionSetListDelete;
    QAction *		actionSetListUpdate;
    QAction *		actionSetListUpdateForce;
    QAction *		actionSetListTaboo;
    QAction *		actionSetListProtected;
};



class YQPkgObjListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor. Creates a YQPkgObjList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgObjListItem( YQPkgObjList * pkgObjList, zypp::ResObject::Ptr zyppObj );

    /**
     * Destructor
     **/
    virtual ~YQPkgObjListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    zypp::ResObject::Ptr zyppObj() { return _zyppObj; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const zypp::ResObject::Ptr constZyppObj() const { return _zyppObj; }

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
    zypp::ui::Status status() const;

    /**
     * Returns 'true' if this selectable's status is set by a selection
     * (rather than by the user or by the dependency solver).
     **/
    bool bySelection() const;

    /**
     * Set the ( binary RPM ) package status
     **/
    virtual void setStatus( zypp::ui::Status newStatus );

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
     * ( QListViewItem::setText() expects a QString! )
     **/
    void setText( int column, const string text );

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
    void showNotifyTexts( zypp::ui::Status status );

    /**
     * Display this item's license agreement (if there is any) that corresponds to
     * the specified status (S_Install, S_Update) in a pop-up window.
     *
     * Returns 'true' if the user agreed to that license , 'false' otherwise.
     * The item's status may have changed to S_Taboo, S_Proteced or S_Del if
     * the user disagreed with the license.
     **/
    bool showLicenseAgreement( zypp::ui::Status status );

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
     * Calculate a numerical value to compare versions, based on version relations:
     * - Installed newer than candidate ( red )
     * - Candidate newer than installed ( blue ) - worthwhile updating
     * - Installed
     * - Not installed, but candidate available
     **/
    int versionPoints() const;

    /**
     * Update this item's data completely.
     * Triggered by QY2ListView::updateAllItemData().
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void updateData();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual QString toolTip( int column );


    // Columns

    int statusCol()		const	{ return _pkgObjList->statusCol();	}
    int nameCol()		const	{ return _pkgObjList->nameCol();	}
    int summaryCol()		const	{ return _pkgObjList->summaryCol();	}
    int sizeCol()		const	{ return _pkgObjList->sizeCol();	}
    int versionCol()		const	{ return _pkgObjList->versionCol();	}
    int instVersionCol()	const	{ return _pkgObjList->instVersionCol(); }


protected:

    /**
     * Initialize internal data and set fields accordingly.
     **/
    void init();


    // Data members

    YQPkgObjList *		_pkgObjList;
    zypp::ResObject::Ptr	_zyppObj;
    bool			_editable;
    bool			_candidateIsNewer;
    bool			_installedIsNewer;
};



#endif // ifndef YQPkgObjList_h
