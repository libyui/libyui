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

  File:	      YQPkgSelectionsFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgSelectionsFilterView_h
#define YQPkgSelectionsFilterView_h

#include <qvbox.h>
#include <y2pm/PMSelection.h>


class YUIQt;
class YQPkgSelList;


class YQPkgSelectionsFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelectionsFilterView( YUIQt *yuiqt, QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelectionsFilterView();


    /**
     * Returns this view's selections list.
     **/
    YQPkgSelList * selList() const { return _selList; }

#if 0
// public slots:

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

    
// signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();
#endif
    

protected:

    
    // Data members

    YUIQt		* yuiqt;
    YQPkgSelList	* _selList;
};



#endif // ifndef YQPkgSelectionsFilterView_h
