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
#include "QY2ListView.h"
#include <zypp/Source.h>
#include <zypp/Product.h>


class YQPkgInstSrcListItem;

typedef zypp::Source_Ref	ZyppSrc;
typedef zypp::Product::Ptr	ZyppProduct;


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
    void addInstSrc( ZyppSrc src );
    
    
public:

    // Column numbers

    int nameCol()	const	{ return _nameCol;	}
    int urlCol()	const	{ return _urlCol;	}

    
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

    int	_nameCol;
    int	_urlCol;
    
};



class YQPkgInstSrcListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgInstSrcListItem( YQPkgInstSrcList *parentList, ZyppSrc src );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcListItem();

    /**
     * Returns the ZYPP source this item corresponds to
     **/
    ZyppSrc zyppSrc() const { return _zyppSrc; }

    /**
     * Returns the parent list
     **/
    const YQPkgInstSrcList * instSrcList() const { return _instSrcList; }

    /**
     * Returns the product on this source if it has one single product
     * or 0 if there are no or multiple products.
     **/
    ZyppProduct YQPkgInstSrcListItem::singleProduct();


    // Columns

    int nameCol()	const	{ return _instSrcList->nameCol();	}
    int urlCol()	const 	{ return _instSrcList->urlCol(); 	}

protected:

    // Data members

    YQPkgInstSrcList *		_instSrcList;
    ZyppSrc			_zyppSrc;
};


#endif // ifndef YQPkgInstSrcList_h
