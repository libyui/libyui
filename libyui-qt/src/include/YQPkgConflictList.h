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

#include <y2pm/PkgDep.h>
#include "QY2ListView.h"


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

public slots:

    /**
     * Activate the choices the user made.
     **/
    void activateUserChoices();


};



#endif // ifndef YQPkgConflictList_h
