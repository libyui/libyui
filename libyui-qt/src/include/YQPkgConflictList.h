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

  File:	      YQPkgConflictList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgConflictList_h
#define YQPkgConflictList_h


#include <stdio.h>
#include <qmap.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMSelectable.h>
#include "QY2ListView.h"


class QAction;
class YQPkgConflict;
class YQPkgConflictAlternative;
class YQPkgConflictResolution;
class YQPkgConflictDialog;


/**
 * Resolution types for conflicts
 **/
typedef enum YQPkgConflictResolutionType
{
    YQPkgConflictUndo,
    YQPkgConflictIgnore,
    YQPkgConflictBruteForceDelete,
    YQPkgConflictAlternative
};


/**
 * @short Display package dependency conflicts in a tree list and let the user
 * choose how to resolve each conflict.
 **/
class YQPkgConflictList : public QY2ListView
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQPkgConflictList( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflictList();

    /**
     * Fill the list with the specified bad list.
     **/
    void fill( PkgDep::ErrorResultList & badList );

    /**
     * Check if the user choices are complete.
     **/
    bool choicesComplete();

    /**
     * Check if the conflict list is empty.
     **/
    bool isEmpty() const { return childCount() == 0; }

    /**
     * Returns the number of conflicts in the list.
     **/
    int count() const { return childCount(); }


public slots:

    /**
     * Apply the choices the user made.
     **/
    void applyResolutions();

    /**
     * Ignore all conflicts.
     **/
    void ignoreAll();

    /**
     * Ask for a file name and save the current conflict list to file.
     **/
    void askSaveToFile() const;


public:

    /**
     * Save the conflict list in its current state to a file. Retains the
     * current 'expanded' state, i.e. writes only those entries that are
     * currently open (not collapsed) in the tree.
     *
     * Posts error popups if 'interactive' is 'true' (only log entries
     * otherwise).
     **/
    void saveToFile( const QString filename, bool interactive ) const;


protected:

    /**
     * (Recursively) save one item to file.
     **/
    void saveItemToFile( FILE * file, const QListViewItem * item ) const;


signals:

    /**
     * Update package states - they may have changed.
     **/
    void updatePackages();
};



/**
 * @short Root item for each individual conflict
 **/
class YQPkgConflict: public QY2ListViewItem
{
public:

    /**
     * Constructor.
     **/
    YQPkgConflict( YQPkgConflictList *		parentList,
		   const PkgDep::ErrorResult &	errorResult );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflict() {}

    /**
     * Access the internal ErrorResult.
     **/
    PkgDep::ErrorResult & errorResult() { return _conflict; }

    /**
     * Returns if this conflict needs an alternative from a list.
     **/
    bool needAlternative() { return ! _conflict.alternatives.empty(); }

    /**
     * Returns if this package collides with other packages.
     **/
    bool hasCollisions() { return ! _conflict.conflicts_with.empty(); }

    /**
     * Returns if this package has open (unresolved) requirements.
     **/
    bool hasOpenRequirements() { return ! _conflict.unresolvable.empty(); }

    /**
     * Apply the choices the user made.
     **/
    void applyResolution();


    /**
     * Check if this conflict with header should be ignored.
     **/
    bool isIgnored();

    /**
     * Ignore this conflict - i.e. add it to the ignore list.
     **/
    void ignore();

    /**
     * Check if a conflict with header (first text line in list)
     * 'conflictHeader is ignored.
     **/
    static bool isIgnored( const QString & conflictHeader );

    /**
     * Ignore a conflict with header (first text line in list)
     * 'conflictHeader'.
     **/
    static void ignore( const QString & conflictHeader );

    /**
     * Reset all ignored conflicts as if the user had never selected any
     * conflict to ignore.
     **/
    static void resetIgnoredConflicts();
    
    /**
     * Save all ignored conflicts to (predefined) file.
     **/
    static void saveIgnoredConflicts();

    /**
     * Load ignored conflicts from (predefined) file.
     **/
    static void loadIgnoredConflicts();

    /**
     * Returns 'true' if there are any ignored conflicts, 'false' otherwise.
     **/
    static bool haveIgnoredConflicts();

    /**
     * Returns a pointer to the one and only internal QAction that resets
     * ignored dependency conflicts. If none exists yet, it will be created.
     *
     * 'dialog' is the conflict dialog this action is connected to.
     **/
    static QAction * actionResetIgnoredConflicts( YQPkgConflictDialog * dialog = 0 );

    /**
     * Update the enabled / disabled state of all internal QActions (that are
     * created yet - this method will not create any if they don't exist yet).
     **/
    static void updateActions();


protected:

    /**
     * Format the text heading line for this item.
     **/
    void formatHeading();

    /**
     * Dump all relevant lists from the internal ErrorResult into the conflict
     * list.
     **/
    void dumpLists();

    /**
     * Dump a list of package relations into the conflict list:
     * Create a new list entry for each list entry.
     * If 'header' is non-null, create a bracketing list item with text
     * 'header' and insert the list items below that new item.
     * Splits into a sublist at (about) 'splitThreshold' if this is > 1.
     * Does nothing if the list is empty.
     **/
    void dumpList( QListViewItem * 		parent,
		   PkgDep::RelInfoList & 	list,
		   int				splitThreshold = -1,
		   const QString & 		header	   = QString::null );

    /**
     * Add suggestions how to resolve this conflict.
     **/
    void addResolutionSuggestions();

    /**
     * Add resolution suggestion: Undo what caused this conflict
     * (i.e. don't remove, don't install, ...).
     **/
    void addUndoResolution( QY2CheckListItem * parent );

    /**
     * Add a list of alternatives if there are any.
     **/
    void addAlternativesList( QY2CheckListItem * parent );

    /**
     * Add brute force resolution suggestion: Delete all dependent packages.
     **/
    void addDeleteResolution( QY2CheckListItem * parent );

    /**
     * Add resolution suggestion: Ignore conflict, risk inconsistent system
     **/
    void addIgnoreResolution( QY2CheckListItem * parent );

    /**
     * Dump the 'remove_to_solve_conflict' list into the conflict list.
     **/
    void dumpDeleteList( QListViewItem * parent );

    /**
     * Remove everything in the remove list.
     **/
    void bruteForceDelete();

    /**
     * Paint method. Reimplemented from @ref QListViewItem a different
     * font can be used.
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );


    // Data members

    PMObjectPtr			_pmObj;
    bool			_isPkg;
    QString			_shortName;	// Only pkg name (no version)
    QString			_fullName;	// Name + edition
    PMSelectable::UI_Status	_status;
    PMSelectable::UI_Status	_undo_status;
    bool			_collision;

    QListViewItem *		_resolutionsHeader;
    PkgDep::ErrorResult		_conflict;
    YQPkgConflictList *		_parentList;


    /**
     * Conflicts that are to be ignored.
     **/
    static QMap<QString, bool> _ignore;

    static QAction *		_actionResetIgnoredConflicts;
};



class YQPkgConflictResolution: public QY2CheckListItem
{
public:

    /**
     * Constructor for generic resolutions (not alternatives)
     **/
    YQPkgConflictResolution( QY2CheckListItem *			parent,
			     const QString & 			text,
			     YQPkgConflictResolutionType	type );

    /**
     * Constructor for alternatives
     **/
    YQPkgConflictResolution( QY2CheckListItem *	parent,
			     PMObjectPtr	pmObj );

    /**
     * Returns the corresponding PMObject.
     **/
    PMObjectPtr pmObj() const { return _pmObj; }

    /**
     * Returns the type of this resolution.
     **/
    YQPkgConflictResolutionType type() const { return _type; }


    /**
     * Returns the resolution type as string.
     **/
    const char * typeString() const;

protected:

    // Data members

    YQPkgConflictResolutionType	_type;
    PMObjectPtr			_pmObj;
};


#endif // ifndef YQPkgConflictList_h
