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


/**
 * @short Abstract base class for a list of PMObjects.
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


public slots:

    /**
     * Add a PMObject to the list. Connect a filter's filterMatch() signal to this
     * slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgObj( PMObjectPtr pmObj );

    
    /**
     * Dispatcher slot for mouse click: cycle status depending on column.
     **/
    virtual void pkgObjClicked( int		button,
				QListViewItem *	item,
				int		col    );

    /**
     * Reimplemented from QY2ListView:
     * Emit selectionChanged() signal after clearing the list.
     **/
    virtual void clear();

    
protected slots:

    /**
     * Dispatcher slot for selection change - internal only
     **/
    virtual void selectionChangedInternal( QListViewItem * item );


signals:
    

    /**
     * Emitted when a PMObject is selected.
     * May be called with a null poiner if no PMObject is selected.
     **/
    void selectionChanged( PMObjectPtr pmObj );


protected:

    
    // Data members

    int 	_statusCol;
    int 	_nameCol;
    int 	_summaryCol;
    int 	_sizeCol;
    int 	_versionCol;
    int 	_instVersionCol;
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
     * Returns the original object within the package manager backend
     **/
    PMObjectPtr pmObj() { return _pmObj; }

    /**
     * Returns the original object within the package manager backend
     **/
    const PMObjectPtr constPMObj() const { return _pmObj; }

    /**
     * Returns the (binary RPM) package status
     **/
    PMSelectable::UI_Status status() const;

    /**
     * Set the (binary RPM) package status
     **/
    virtual void setStatus( PMSelectable::UI_Status newStatus );

    /**
     * Set a status icon according to the package's status
     **/
    virtual void setStatusIcon();

    /**
     * Cycle the package status to the next valid value
     **/
    virtual void cycleStatus();
    
    /**
     * Set a column text via STL string
     * (QListViewItem::setText() expects a QString)
     **/
    void setText( int column, const std::string text );

    /**
     * Set a column text via PkgEdition
     * (QListViewItem::setText() expects a QString)
     **/
    void setText( int column, const PkgEdition & edition );

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

    int statusCol()		const	{ return _pkgObjList->statusCol();	}
    int nameCol()		const	{ return _pkgObjList->nameCol();	}
    int summaryCol()		const	{ return _pkgObjList->summaryCol();	}
    int sizeCol()		const	{ return _pkgObjList->sizeCol();	}
    int versionCol()		const	{ return _pkgObjList->versionCol();	}
    int instVersionCol()	const	{ return _pkgObjList->instVersionCol();	}


protected:

    // Data members

    YQPkgObjList *	_pkgObjList;
    PMObjectPtr		_pmObj;
};


#endif // ifndef YQPkgObjList_h
