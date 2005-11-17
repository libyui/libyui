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

  File:	      YQPkgInstSrcFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgInstSrcFilterView_h
#define YQPkgInstSrcFilterView_h

#include <qvbox.h>


class YQPkgInstSrcList;
class QY2ComboTabWidget;
class YQPkgRpmGroupTagsFilterView;
class YQPkgSearchFilterView;


class YQPkgInstSrcFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgInstSrcFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcFilterView();

    
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
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package does not come from the respective source
     **/
    void filterNearMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();

    
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

    
protected slots:

    /**
     * Propagate a filter match from the primary filter
     * and appy any selected secondary filter(s) to it
     **/
    void primaryFilterMatch( PMPackagePtr pkg );
    
    /**
     * Propagate a filter near match from the primary filter
     * and appy any selected secondary filter(s) to it
     **/
    void primaryFilterNearMatch( PMPackagePtr pkg );


protected:

    /**
     * Widget layout for the secondary filters
     **/
    QWidget * layoutSecondaryFilters( QWidget * parent );

    /**
     * Check if pkg matches the the currently selected secondary filter
     **/
    bool secondaryFilterMatch( PMPackagePtr pkg );


    // Data members

    YQPkgInstSrcList *		_instSrcList;
    QY2ComboTabWidget *		_secondaryFilters;
    QWidget *			    _allPackages;
    YQPkgRpmGroupTagsFilterView *   _rpmGroupTagsFilterView;
    YQPkgSearchFilterView *	    _searchFilterView;
};



#endif // ifndef YQPkgInstSrcFilterView_h
