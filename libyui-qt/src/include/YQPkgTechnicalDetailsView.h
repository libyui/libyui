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

#include "YQPkgGenericDetailsView.h"
#include <string>


class YQPkgTechnicalDetailsView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgTechnicalDetailsView( QWidget *parent );


    /**
     * Destructor
     **/
    virtual ~YQPkgTechnicalDetailsView();


protected:
 
    /**
     * Show details for the specified package:
     * In this case technical data, very much like "rpm -qi".
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showPkgDetails( PMPackagePtr pkg );

    /**
     * Returns a string containing a HTML table for technical details for one
     * package.
     **/
    QString simpleTable( PMPackagePtr pkg );


    /**
     * Returns a string containing a HTML table for technical details for two
     * package instances: The installed instance and an alternate instance.
     * (usually the candidate instance).
     **/
    QString complexTable( PMPackagePtr installedPkg,
			  PMPackagePtr candidatePkg );

    /**
     * Returns a string containing a HTML table with 'contents'.
     **/
    QString table( const QString & contents ) const;

    /**
     * Returns a string containing a HTML table row with 'contents'.
     **/
    QString row( const QString & contents ) const;

    /**
     * Returns a string containing a HTML table cell with 'contents'.
     **/
    QString cell( QString contents		) const;
    QString cell( int contents 			) const;
    QString cell( const std::string & contents	) const;
    QString cell( const Date & date		) const;

    /**
     * Returns a string containing a HTML table cell with 'contents'
     * for table headers.
     **/
    QString hcell( QString contents ) const;

    /**
     * Returns a string containing HTML code for a package's authors list.
     **/
    QString authorsListCell( PMPackagePtr pkg ) const;

    /**
     * Format a date.
     **/
    QString formatDate( const Date & date ) const;

    /**
     * Format an RPM group. Retrieves the translated (!) version.
     **/
    QString formatRpmGroup( PMPackagePtr pkg ) const;

};


#endif // ifndef YQPkgTechnicalDetailsView_h
