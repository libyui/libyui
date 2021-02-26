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

  File:	      YQPkgLangList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/



#ifndef YQPkgLangList_h
#define YQPkgLangList_h

#include <YQPkgObjList.h>


class YQPkgLangListItem;


/**
 * @short Display a list of zypp::Selection objects.
 **/
class YQPkgLangList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgLangList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangList();


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
    void addLangItem( ZyppSel		selectable,
		      ZyppLang 	lang );

    
public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgLangListItem * selection() const;


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


protected slots:

    /**
     * Fill the language list.
     **/
    void fillList();
};



class YQPkgLangListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgLangList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgLangListItem( YQPkgLangList *	pkgSelList,
		       ZyppSel		selectable,
		       ZyppLang		lang		);

    /**
     * Destructor
     **/
    virtual ~YQPkgLangListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    ZyppLang zyppLang() const	{ return _zyppLang; }


    // Columns

    int statusCol()	const	{ return _langList->statusCol(); }


protected:

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all languages.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgLangList *	_langList;
    ZyppLang		_zyppLang;
};


#endif // ifndef YQPkgLangList_h
