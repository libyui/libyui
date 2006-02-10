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

  File:	      YQPkgInstSrcList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgInstSrcList_h
#define YQPkgInstSrcList_h

#include "YQZypp.h"
#include <QY2ListView.h>


class YQPkgInstSrcListItem;


/**
 * @short Display a list of zypp::Selection objects.
 **/
class YQPkgInstSrcList : public QY2ListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgInstSrcList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcList();


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
     * Add an inst source to the list.
     **/
    void addInstSrc( InstSrcManager::ISrcId instSrcId );
    
    
public:

    // Column numbers

    int nameCol()		const	{ return _nameCol;		}
    int urlCol()		const	{ return _urlCol;		}

    
    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgInstSrcListItem * selection() const;


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package comes from the respective source
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package does not come from the respective source
     **/
    void filterNearMatch( ZyppSel	selectable,
			  ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the list.
     **/
    void fillList();


private:

    //
    // Data members
    //

    int		_nameCol;
    int		_urlCol;
    
};



class YQPkgInstSrcListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgInstSrcListItem( YQPkgInstSrcList *parentList, InstSrcManager::ISrcId instSrcId );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcListItem();

    /**
     * Returns the original inst source ID within the package manager backend.
     **/
    InstSrcManager::ISrcId instSrcId() { return _instSrcId; }

    /**
     * Returns the parent list
     **/
    const YQPkgInstSrcList * instSrcList() const { return _instSrcList; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const InstSrcManager::ISrcId constInstSrcId() const { return _instSrcId; }

    /**
     * Returns the package manager backend's inst source description or 0
     **/
    constInstSrcDescrPtr instSrcDescr() const
	{ return _instSrcId ? _instSrcId->descr() : 0; }


    // Columns

    int nameCol()	const	{ return _instSrcList->nameCol();	}
    int urlCol()	const 	{ return _instSrcList->urlCol(); 	}


protected:

    // Data members

    YQPkgInstSrcList *		_instSrcList;
    InstSrcManager::ISrcId	_instSrcId;
};


#endif // ifndef YQPkgInstSrcList_h
