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

  File:	      YQPkgDependenciesView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgDependenciesView_h
#define YQPkgDependenciesView_h

#include <zypp/Package.h>
#include "YQPkgGenericDetailsView.h"

using std::string;


/**
 * @short Display technical details ( very much like 'rpm -qi' ) for a zypp::Package
 * object - the installed instance, the candidate instance or both ( in two
 * columns ) if both exist. All other available instances are ignored.
 **/
class YQPkgDependenciesView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgDependenciesView( QWidget * parent );


    /**
     * Destructor
     **/
    virtual ~YQPkgDependenciesView();


protected:

    /**
     * Show details for the specified zypp::ResObject:
     * In this case technical data, very much like "rpm -qi".
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( zypp::ResObject::Ptr zyppObj );

    /**
     * Returns a string containing a HTML table for technical details for one
     * package.
     **/
    QString simpleTable( zypp::Package::Ptr pkg );


    /**
     * Returns a string containing a HTML table for technical details for two
     * package instances: The installed instance and an alternate instance.
     * ( usually the candidate instance ).
     **/
    QString complexTable( zypp::Package::Ptr installedPkg,
			  zypp::Package::Ptr candidatePkg );

    /**
     * Make a HTML table cell from a PkgRelList.
     **/
    static QString cell( const PMSolvable::PkgRelList_type & list );

    /**
     * Returns a string containing a HTML table cell with 'contents'.
     **/
    static QString cell( const string & contents	)
	{ return YQPkgGenericDetailsView::cell( contents ); }
};


#endif // ifndef YQPkgDependenciesView_h
