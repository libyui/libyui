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

  File:	      YQPkgPatchList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgPatchList_h
#define YQPkgPatchList_h

#include <string>
#include "YQPkgObjList.h"
#include "YQPkgSelMapper.h"

#define ENABLE_DELETING_PATCHES	1

using std::string;

class YQPkgPatchListItem;


enum YQPkgPatchCategory	// This is also the sort order
{
    YQPkgYaSTPatch,
    YQPkgSecurityPatch,
    YQPkgRecommendedPatch,
    YQPkgOptionalPatch,
    YQPkgDocumentPatch,

    YQPkgUnknownPatchCategory = 9999
};



/**
 * @short Display a list of zypp::Patch objects.
 **/
class YQPkgPatchList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgPatchList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchList();


    enum FilterCriteria
    {
	UpdateStackPatches,		// zypp or YaST2
	RelevantPatches,		// needed + broken
	RelevantAndInstalledPatches,	// needed + broken + installed
	AllPatches			// all
    };


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
     * Add a patch to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPatchItem( ZyppSel   selectable,
		       ZyppPatch zyppPatch );

    /**
     * Fill the patch list according to filterCriteria().
     **/
    void fillList();

    /**
     * Display a one-line message in the list.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void message( const QString & text );


public:

    /**
     * Return 'true' if there is any patch for the update stack in the pool
     * that is not installed yet or that could be updated.
     **/
    static bool haveUpdateStackPatches();

    /**
     * Set the filter criteria for fillList().
     **/
    void setFilterCriteria( FilterCriteria filterCriteria );

    /**
     * Returns the current filter criteria.
     **/
    FilterCriteria filterCriteria() const { return _filterCriteria; }

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgPatchListItem * selection() const;

    /**
     * Returns the column for the patch category
     **/
    int categoryCol() const { return _categoryCol; }

    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QPopupMenu * addAllInListSubMenu( QPopupMenu * menu );

    /**
     * Delayed initialization after the dialog is fully created.
     *
     * Reimplemented from QWidget.
     **/
    virtual void polish();


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
     * Emitted during filtering for non-pkg items:
     * pre-script, post-script, files
     **/
    void filterMatch( const QString & 	name,
		      const QString & 	summary,
		      FSize 		size );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:

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
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Reimplemented from YQPkgObjList / QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    int			_categoryCol;
    YQPkgSelMapper	_selMapper;
    FilterCriteria	_filterCriteria;
};



class YQPkgPatchListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgPatchList item that corresponds to
     * zyppPatch.
     **/
    YQPkgPatchListItem( YQPkgPatchList * 	patchList,
			   ZyppSel		selectable,
			   ZyppPatch 		zyppPatch );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchListItem();

    /**
     * Returns the original zyppPatch object.
     **/
    ZyppPatch zyppPatch() const { return _zyppPatch; }

    /**
     * Maps a string patch category to the corresponding enum.
     **/
    static YQPkgPatchCategory patchCategory( QString category );

    /**
     * Converts a patch category to a user-readable (translated) string.
     **/
    static QString asString( YQPkgPatchCategory category );

    /**
     * Returns the category of this patch (security, recommended, ...).
     **/
    YQPkgPatchCategory patchCategory() const { return _patchCategory; }

    /**
     * Cycle the package status to the next valid value.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void cycleStatus();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QString toolTip( int column );

    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem.
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    // Columns

    int statusCol()	const	{ return _patchList->statusCol();	}
    int summaryCol()	const	{ return _patchList->summaryCol();	}
    int categoryCol()	const	{ return _patchList->categoryCol();	}


protected:

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all patches.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgPatchList *	_patchList;
    ZyppPatch		_zyppPatch;
    YQPkgPatchCategory	_patchCategory;
};


#endif // ifndef YQPkgPatchList_h
