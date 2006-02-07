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

// -*- c++ -*-


#ifndef YQPkgLangList_h
#define YQPkgLangList_h

#include <YQPkgObjList.h>
#include <y2pm/PMLanguage.h>


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
    void addLangItem( zypp::ui::Selectable::Ptr	selectable,
		      PMLanguagePtr 		lang );

    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

    
public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgLangListItem * selection() const;


signals:

    /**
     * Emitted when it's time to update displayed package information,
     * e.g., package states.
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
    void filterMatch( zypp::ui::Selectable::Ptr	selectable,
		      zypp::Package::constPtr	pkg );

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
    YQPkgLangListItem( YQPkgLangList *			pkgSelList,
		       zypp::ui::Selectable::Ptr	selectable,
		       PMLanguagePtr			lang );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    PMLanguagePtr pmLang() const { return _pmLang; }

    /**
     * Set the selection status.
     *
     * Reimplemented from YQPkgObjListItem:
     * Activate selections and emit updatePackages signal for each
     * status change.
     **/
    virtual void setStatus( zypp::ui::Status newStatus );


    // Columns

    int statusCol()	const	{ return _langList->statusCol();	}


protected:

    // Data members

    YQPkgLangList	*	_langList;
    PMLanguagePtr		_pmLang;
};


#endif // ifndef YQPkgLangList_h
