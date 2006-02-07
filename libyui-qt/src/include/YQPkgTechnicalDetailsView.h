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

  File:	      YQPkgTechnicalDetailsView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgTechnicalDetailsView_h
#define YQPkgTechnicalDetailsView_h

#include <zypp/Package.h>

#include "YQPkgGenericDetailsView.h"


/**
 * @short Display technical details ( very much like 'rpm -qi' ) for a zypp::Package
 * object - the installed instance, the candidate instance or both ( in two
 * columns ) if both exist. All other available instances are ignored.
 **/
class YQPkgTechnicalDetailsView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgTechnicalDetailsView( QWidget * parent, bool youMode = false );


    /**
     * Destructor
     **/
    virtual ~YQPkgTechnicalDetailsView();


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
     * Returns a string containing HTML code for a package's authors list.
     **/
    QString authorsListCell( zypp::Package::Ptr pkg ) const;

    /**
     * Format an RPM group. Retrieves the translated ( ! ) version.
     **/
    QString formatRpmGroup( zypp::Package::Ptr pkg ) const;


    // Data members

    bool	_youMode;
};


#endif // ifndef YQPkgTechnicalDetailsView_h
