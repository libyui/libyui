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
#include <y2pm/PMPackage.h>


class YQPkgListItem;


/**
 * @short Display a list of PMPackage objects.
 **/
class YQPkgList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgList( QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgList();


    // Column numbers

    int srpmStatusCol() 	const	{ return _srpmStatusCol; 	}


public slots:

    /**
     * Add a pkg to the list. Connect a filter's filterMatch() signal to this
     * slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgItem( PMPackagePtr pmPkg );


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
     * Reimplemented from QListView / QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;


    // Direct access to some states for menu actions

    void setInstallCurrentSourceRpm()	  { setInstallCurrentSourceRpm( true  ); }
    void setDontInstallCurrentSourceRpm() { setInstallCurrentSourceRpm( false ); }


    // No separate selectionChanged( PMPackagePtr ) signal:
    // Use YQPkgObjList::selectionChanged( PMObjectPtr ) instead
    // and dynamic_cast to PMPackagePtr if required.
    // This saves duplicating a lot of code.


protected:

    /**
     * Create context menu for source RPMs.
     **/
    void createSourceRpmContextMenu();

    /**
     * Sets the currently selected item's source RPM status.
     * Automatically selects the next item if 'selectNextItem' is 'true'.
     **/
    void setInstallCurrentSourceRpm( bool inst, bool selectNextItem = false );

    // Data members

    int			_srpmStatusCol;
    QAction *		_actionInstallSourceRpm;
    QAction *		_actionDontInstallSourceRpm;
    QPopupMenu *	_sourceRpmContextMenu;
};



class YQPkgListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgListItem( YQPkgList * pkgList, PMPackagePtr pmPkg );

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
    PMPackagePtr pmPkg() { return _pmPkg; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const PMPackagePtr constPMPkg() const { return _pmPkg; }

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
    PMPackagePtr	_pmPkg;
};


#endif // ifndef YQPkgList_h
