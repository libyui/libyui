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

  File:	      YQPkgSelList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgSelList_h
#define YQPkgSelList_h

#include <qlistview.h>
#include <qpixmap.h>
#include <y2pm/PMPackage.h>


class YQPkgSel;	// shorter name than "YQPkgSelListItem"
class YUIQt;


class YQPkgSelList : public QListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelList( YUIQt *yuiqt, QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelList();


    // Column numbers

    int statusCol()	const	{ return _statusCol;	}
    int summaryCol()	const	{ return _summaryCol;	}


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


    /**
     * Add a selection to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgSel( PMSelectionPtr sel );

    /**
     * Dispatcher slot for mouse click: cycle status depending on column
     **/
    void slotPkgSelClicked( int		button,
			    YQPkgSel *	sel,
			    int		col );

    /**
     * Select a list entry (if there is any).
     * Usually this will be the first list entry, but don't rely on that - this
     * might change without notice. Emits signal selectionChanged().
     **/
    void selectSomething();

    /**
     * Update the status display of all list entries.
     * This is an expensive operation.
     **/
    void updateAllItemStates();
    

    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgSel * selection() const;


signals:

    /**
     * Emitted when it's time to update displayed package information,
     * e.g., package states
     **/
    void updatePackages();

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();
    /**
     * Emitted for mouse clicks on a selection.
     **/
    void pkgSelClicked		( int		button,
				  YQPkgSel *	sel,
				  int		col );

    /**
     * Emitted for mouse double clicks on a selection.
     **/
    void pkgSelDoubleClicked	( int		button,
				  YQPkgSel *	sel,
				  int		col );


    /**
     * Emitted when a package selection is selected in the list.
     * May be called with a null poiner if no package is selected.
     **/
    void selectionChanged	( PMSelectionPtr sel );


protected slots:

    /**
     * Fill the selection list.
     **/
    void fillList();


protected:

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMousePressEvent( QMouseEvent * e );

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMouseReleaseEvent( QMouseEvent* );

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMouseDoubleClickEvent( QMouseEvent* );


    // Data members

    YUIQt *		yuiqt;
    QListViewItem *	mousePressedItem;
    int			mousePressedCol;
    int			mousePressedButton;


private:

    int _statusCol;
    int _summaryCol;
    int _sizeCol;
    int _versionCol;
    int _srpmStatusCol;

    QPixmap _iconOff;
    QPixmap _iconOn;
    QPixmap _iconAuto;
    QPixmap _iconDelete;
    QPixmap _iconUpdate;
    QPixmap _iconTaboo;
};



class YQPkgSel: public QListViewItem
{
public:

    /**
     * Constructor. Creates a YQPkgSelList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgSel( YQPkgSelList * pkgSelList, PMSelectionPtr sel );

    /**
     * Destructor
     **/
    virtual ~YQPkgSel();

    /**
     * Returns the parent package list
     **/
    YQPkgSelList * pkgSelList() { return _pkgSelList; }

    /**
     * Returns the original object within the package manager backend
     **/
    PMSelectionPtr pkgSel() { return _pkgSel; }

    /**
     * Returns the original object within the package manager backend
     **/
    const PMSelectionPtr constPkgSel() const { return _pkgSel; }

    /**
     * Returns the (binary RPM) package status
     **/
    PMSelectable::UI_Status status() const;

    /**
     * Set the (binary RPM) package status
     **/
    void setStatus( PMSelectable::UI_Status newStatus );

    /**
     * Set a status icon according to the package's status
     **/
    void setStatusIcon();

    /**
     * Cycle the package status to the next valid value
     **/
    void cycleStatus();

    /**
     * Return wheter or not this selection is already installed, i.e. if it can
     * be updated or deleted.
     **/
    bool isInstalled() const { return _isInstalled; }

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

    // Columns

    int statusCol()	const	{ return _pkgSelList->statusCol();	}
    int summaryCol()	const	{ return _pkgSelList->summaryCol();	}


protected:

    // Data members

    YQPkgSelList	*	_pkgSelList;
    PMSelectionPtr		_pkgSel;

    bool			_isInstalled;
};


#endif // ifndef YQPkgSelList_h
