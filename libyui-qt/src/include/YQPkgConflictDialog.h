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


signals:

    /**
     * Update package states - they may have changed.
     **/
    void updatePackages();


protected:

    PMManager *		_selectableManager;
    YQPkgConflictList *	_conflictList;
    QPopupMenu *	_expertMenu;
};



#endif // ifndef YQPkgConflictDialog_h
