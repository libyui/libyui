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

#include <qlistview.h>
#include <y2pm/PMPackage.h>
#include <vector>


class YQPkg;	// shorter name than "YQPkgListItem"
class YUIQt;

enum YQPkgStatus
{
    // Keep this order, it's used for sorting package lists!
    // Dangerous / noteworthy states are sorted first.

    YQPkgTaboo,		// Never install this
    YQPkgDel,		// Will be deleted
    YQPkgUpdate,	// Will be updated
    YQPkgInstall,	// Will be installed upon explicit user request
    YQPkgAuto,		// Will be automatically installed
    YQPkgKeepInstalled,	// Is installed - keep this version
    YQPkgNoInst		// Is not installed and will not be installed
};


class YQPkgList : public QListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgList( YUIQt *yuiqt, QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgList();


    // Column numbers

    int statusCol()	const	{ return _statusCol;	}
    int nameCol()	const	{ return _nameCol;	}
    int summaryCol()	const	{ return _summaryCol;	}
    int sizeCol()	const	{ return _sizeCol;	}
    int versionCol()	const	{ return _versionCol;	}
    int srpmStatusCol() const	{ return _srpmStatusCol; }


public slots:

    /**
     * Add a pkg to the list. Connect a filter's filterMatch() signal to this
     * slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkg( PMPackagePtr pkg );

    /**
     * Dispatcher slot for mouse click: cycle status depending on column
     **/
    void slotPkgClicked( int		button,
			 YQPkg *	pkg,
			 int		col );
    
    /**
     * Reimplemented from QListView / QWidget:
     * Reserve as much horizontal space as possible
     **/
    virtual QSize sizeHint() const;


    /**
     * Reimplemented from QListView:
     * Adjust header sizes after clearing contents.
     **/
    virtual void clear();
    
signals:

    /**
     * Emitted for mouse clicks on a package
     **/
    void pkgClicked		( int		button,
				  YQPkg *	pkg,
				  int		col );

    /**
     * Emitted for mouse double clicks on a package
     **/
    void pkgDoubleClicked	( int		button,
				  YQPkg *	pkg,
				  int		col );


    /**
     * Emitted when a package is selected.
     * May be called with a null poiner if no package is selected.
     **/
    void selectionChanged	( PMPackagePtr	pkg );


protected slots:

    /**
     * Dispatcher slot for selection change - internal only
     **/
    void selectionChangedInternal( QListViewItem * sel );

    /**
     * Internal: Handle manual column resize.
     * Save the user's preferred sizes so they don't get overwritten each time
     * the list is cleared and filled with new contents.
     **/
    void columnWidthChanged( int col, int oldSize, int newSize );

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

    /**
     * Save the current column widths.
     **/
    void saveColumnWidths();

    /**
     * Restore the column widths to what was saved previously with
     * saveColumnWidths().   
     **/
    void restoreColumnWidths();


    // Data members

    YUIQt *		yuiqt;
    QListViewItem *	mousePressedItem;
    int			mousePressedCol;
    int			mousePressedButton;


private:

    int _statusCol;
    int _nameCol;
    int _summaryCol;
    int _sizeCol;
    int _versionCol;
    int _srpmStatusCol;

    std::vector<int> _savedColumnWidth;
};



class YQPkg: public QListViewItem
{
public:

    /**
     * Constructor. Creates a YQPkgList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkg( YQPkgList * pkgList, PMPackagePtr pkg );

    /**
     * Destructor
     **/
    virtual ~YQPkg();

    /**
     * Returns the parent package list
     **/
    YQPkgList * pkgList() { return _pkgList; }

    /**
     * Returns the original object within the package manager backend
     **/
    PMPackagePtr pkg() { return _pkg; }

    /**
     * Returns the original object within the package manager backend
     **/
    const PMPackagePtr constPkg() const { return _pkg; }


    /**
     * Returns the (binary RPM) package status
     **/
    YQPkgStatus status() const { return _status; }

    /**
     * Returns the source RPM package status
     **/
    YQPkgStatus srpmStatus() const { return _srpmStatus; }


    /**
     * Set the (binary RPM) package status
     **/
    void setStatus( YQPkgStatus newStatus );

    /**
     * Set the source RPM status
     **/
    void setSrpmStatus( YQPkgStatus newSrpmStatus );

    /**
     * Cycle the package status to the next valid value
     **/
    void cycleStatus();

    /**
     * Cycle the source package status to the next valid value
     **/
    void cycleSrpmStatus();

    /**
     * Set a column text via STL string
     * (QListViewItem::setText() expects a QString)
     **/
    void setText( int column, const std::string text );

    /**
     * Return wheter or not this pkg is already installed, i.e. if it can be
     * updated or deleted.
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

    int statusCol()	const	{ return _pkgList->statusCol();		}
    int nameCol()	const	{ return _pkgList->nameCol();		}
    int summaryCol()	const	{ return _pkgList->summaryCol();	}
    int sizeCol()	const	{ return _pkgList->sizeCol();		}
    int versionCol()	const	{ return _pkgList->versionCol();	}
    int srpmStatusCol() const	{ return _pkgList->srpmStatusCol();	}


protected:


    // Data members

    YQPkgList	*	_pkgList;
    PMPackagePtr	_pkg;
    bool		_isInstalled;


    // FIXME
    // Preliminary - those are PMObject attributes!
    YQPkgStatus		_status;
    YQPkgStatus		_srpmStatus;
    bool		_haveSrpm;
};


#endif // ifndef YQPkgList_h
