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

  File:	      YQPkgUpdateProblemFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgUpdateProblemFilterView_h
#define YQPkgUpdateProblemFilterView_h

#include <qtextbrowser.h>
#include <zypp/Package.h>


/**
 * @short Filter view for packages that made problems during update
 **/
class YQPkgUpdateProblemFilterView : public QTextBrowser
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgUpdateProblemFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgUpdateProblemFilterView();


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


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( zypp::Package::constPtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();

};



#endif // ifndef YQPkgUpdateProblemFilterView_h
