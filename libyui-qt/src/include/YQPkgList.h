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

  File:	      YQPkgList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgList_h
#define YQPkgList_h

#include <YQPkgObjList.h>
#include <zypp/Package.h>


class YQPkgListItem;


/**
 * @short Display a list of zypp::Package objects.
 **/
class YQPkgList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgList();


    // Column numbers

    int srpmStatusCol() 	const	{ return _srpmStatusCol; 	}

    /**
     * Save the pkg list to a file.
     *
     * Posts error popups if 'interactive' is 'true' ( only log entries
     * otherwise ).
     **/
    void exportList( const QString filename, bool interactive ) const;

    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QPopupMenu * addAllInListSubMenu( QPopupMenu * menu );


public slots:

    /**
     * Add a pkg to the list. Connect a filter's filterMatch() signal to this
     * slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgItem( zypp::Package::constPtr zyppPkg );
    
    /**
     * Add a pkg to the list, but display it dimmed (grey text foreground
     * rather than normal black). 
     **/
    void addPkgItemDimmed( zypp::Package::constPtr zyppPkg );

    /**
     * Add a pkg to the list
     **/
    void addPkgItem( zypp::Package::constPtr zyppPkg, bool dimmed );


    /**
     * Dispatcher slot for mouse click: Take care of source RPM status.
     * Let the parent class handle the normal status.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void pkgObjClicked( int		button,
				QListViewItem *	item,
				int		col,
				const QPoint &	pos );

    /**
     * Update the internal actions: What actions are available for 'item'?
     *
     * Reimplemented from YQPkgObjList
     **/
    virtual void updateActions( YQPkgObjListItem * item );

    /**
     * Reimplemented from QListView / QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    /**
     * Ask for a file name and save the current pkg list to file.
     **/
    void askExportList() const;


    // Direct access to some states for menu actions

    void setInstallCurrentSourceRpm()	  { setInstallCurrentSourceRpm( true  ); }
    void setDontInstallCurrentSourceRpm() { setInstallCurrentSourceRpm( false ); }

    void setInstallListSourceRpms()	  { setInstallListSourceRpms( true  ); }
    void setDontInstallListSourceRpms()	  { setInstallListSourceRpms( false ); }


    // No separate selectionChanged( zypp::Package::constPtr ) signal:
    // Use YQPkgObjList::selectionChanged( zypp::ResObject::constPtr ) instead
    // and dynamic_cast to zypp::Package::constPtr if required.
    // This saves duplicating a lot of code.


protected:

    /**
     * Create ( additional ) actions for the context menus.
     **/
    void createActions();
    
    /**
     * Create the context menu for items that are not installed.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createNotInstalledContextMenu();

    /**
     * Create the context menu for installed items.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createInstalledContextMenu();

    /**
     * Create context menu for source RPMs.
     **/
    void createSourceRpmContextMenu();

    /**
     * Sets the currently selected item's source RPM status.
     * Automatically selects the next item if 'selectNextItem' is 'true'.
     **/
    void setInstallCurrentSourceRpm( bool inst, bool selectNextItem = false );

    /**
     * Sets the source RPM status of all items in this list.
     **/
    void setInstallListSourceRpms( bool inst );


    // Data members

    int			_srpmStatusCol;
    QPopupMenu *	_sourceRpmContextMenu;


public:

    QAction *		actionInstallSourceRpm;
    QAction *		actionDontInstallSourceRpm;
    QAction *		actionInstallListSourceRpms;
    QAction *		actionDontInstallListSourceRpms;
};



class YQPkgListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgListItem( YQPkgList * pkgList, zypp::Package::constPtr zyppPkg );

    /**
     * Destructor
     **/
    virtual ~YQPkgListItem();

    /**
     * Returns the parent package list.
     **/
    YQPkgList * pkgList() { return _pkgList; }

    /**
     * Returns the original object within the package manager backend.
     **/
    zypp::Package::constPtr zyppPkg() { return _zyppPkg; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const zypp::Package::constPtr constPMPkg() const { return _zyppPkg; }

    /**
     * Returns the source RPM package status:
     * Should the source RPM be installed?
     **/
    bool installSourceRpm() const;

    /**
     * Set the source RPM status
     **/
    void setInstallSourceRpm( bool installSourceRpm );

    /**
     * Cycle the source package status to the next valid value.
     **/
    void toggleSourceRpmStatus();

    /**
     * Returns whether or not a source RPM is available for this package.
     **/
    bool hasSourceRpm() const;


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
     * Reimplemented from YQPkgObjList.
     **/
    virtual void updateData();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QString toolTip( int column );

    /**
     * Returns true if this package is to be displayed dimmed,
     * i.e. with grey text foreground rather than the normal black.
     **/
    bool isDimmed() const { return _dimmed; }

    /**
     * Set the 'dimmed' flag.
     **/
    void setDimmed( bool d = true ) { _dimmed = d; }


    // Columns

    int srpmStatusCol() const { return _pkgList->srpmStatusCol(); }


protected:

    /**
     * Set the suitable icon for the source RPM status.
     **/
    void setSourceRpmIcon();

    /**
     * Paint method. Reimplemented from @ref QListViewItem so different
     * colors can be used.
     *
     * Reimplemented from QListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );

    // Data members

    YQPkgList *		_pkgList;
    zypp::Package::constPtr	_zyppPkg;
    bool		_dimmed;
};


#endif // ifndef YQPkgList_h
