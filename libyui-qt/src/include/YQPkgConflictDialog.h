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

  File:	      YQPkgConflictDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgConflictDialog_h
#define YQPkgConflictDialog_h

#include <qdialog.h>

class YQPkgConflictList;
class PMManager;
class QPushButton;
class QPopupMenu;
class QLabel;


/**
 * @short Dialog that takes care of dependency checking and resolving
 * conflicts.
 **/
class YQPkgConflictDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor. Does not pop up a dialog yet.
     * Use solveAndShowConflicts().
     * 
     * 'selectableManager' is one of &Y2PM::packageManager() or
     * &Y2PM::selectionManager(). 
     **/
    YQPkgConflictDialog( PMManager * 	selectableManager,
			 QWidget *	parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflictDialog();

    /**
     * Reimplemented from QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    /**
     * Returns the selectable manager used by this dialog to resolve
     * dependencies. 
     **/
    PMManager * selectableManager() const { return _selectableManager; }

    /**
     * Returns the average time in seconds used for solving or 0 if solving
     * hasn't taken place yet.  
     **/
    double averageSolveTime() const;

    /**
     * Returns the total amount of time in seconds used for solving.
     **/
    double totalSolveTime() const { return _totalSolveTime; }
    
    /**
     * Returns the times solving has taken place (with this dialog).
     **/ 
    int solveCount() const { return _solveCount; }
    

public slots:

    /**
     * Run the package dependency solver for the current package set and open
     * the conflict dialog if there are any conflicts.
     * Returns only when the conflict dialog is closed.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int solveAndShowConflicts();

    /**
     * Ignore all conflicts.
     **/
    void ignoreAll();

    /**
     * Reset all ignored conflicts as if the user had never selected any
     * conflict to ignore.
     **/
    void resetIgnoredConflicts();


signals:

    /**
     * Update package states - they may have changed.
     **/
    void updatePackages();


protected:

    PMManager *		_selectableManager;
    YQPkgConflictList *	_conflictList;
    QPopupMenu *	_expertMenu;
    QLabel *		_busyPopup;
    
    double		_totalSolveTime;
    int			_solveCount;
};



#endif // ifndef YQPkgConflictDialog_h
