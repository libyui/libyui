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



#ifndef YQPkgConflictList_h
#define YQPkgConflictList_h


#include <stdio.h>
#include <qmap.h>
#include "QY2ListView.h"

#include <zypp/Resolver.h>
#include <zypp/ResolverProblem.h>
#include <zypp/ProblemSolution.h>


class QAction;
class YQPkgConflict;
class YQPkgConflictResolution;
class YQPkgConflictDialog;


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
     * Fill the list with the specified problems.
     **/
    void fill( zypp::ResolverProblemList problemList );

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

    /**
     * Dump a multi-line text to a QListView as a sequence of separate items.
     * If 'longText' has considerably more lines than 'splitThreshold', fold
     * all lines from no. 'splitThreshold' on into a closed list item
     * "More...".
     * If 'header' is not empty, it will be added as the parent of the lines.
     **/
    static void dumpList( QListViewItem * 	parent,
			  const QString &	longText,
			  const QString & 	header = QString::null,
			  int			splitThreshold = 5 );

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
		   zypp::ResolverProblem_Ptr	problem );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflict() {}

    /**
     * Returns the corresponding ResolverProblem.
     **/
    zypp::ResolverProblem_Ptr problem() const { return _problem; }

    /**
     * Returns the resolution the user selected
     * or 0 if he didn't select one
     **/
    zypp::ProblemSolution_Ptr userSelectedResolution();


protected:

    /**
     * Format the text heading line for this item.
     **/
    void formatHeading();

    /**
     * Add suggestions how to resolve this conflict.
     **/
    void addSolutions();

    /**
     * Paint method. Reimplemented from @ref QListViewItem so a different
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

    zypp::ResolverProblem_Ptr	_problem;
    QY2CheckListItem *		_resolutionsHeader;
};



class YQPkgConflictResolution: public QY2CheckListItem
{
public:

    /**
     * Constructor
     **/
    YQPkgConflictResolution( QY2CheckListItem *		parent,
			     zypp::ProblemSolution_Ptr	_solution );

    /**
     * Return the corresponding ProblemSolution.
     **/
    zypp::ProblemSolution_Ptr solution() const { return _solution; }


protected:

    // Data members

    zypp::ProblemSolution_Ptr	_solution;
};


#endif // ifndef YQPkgConflictList_h
