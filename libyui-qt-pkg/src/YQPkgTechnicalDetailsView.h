/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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


#ifndef YQPkgTechnicalDetailsView_h
#define YQPkgTechnicalDetailsView_h

#include "YQPkgGenericDetailsView.h"


/**
 * @short Display technical details (very much like 'rpm -qi') for a ZYPP
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
    YQPkgTechnicalDetailsView( QWidget * parent );


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
    virtual void showDetails( ZyppSel selectable );

    /**
     * Returns a string containing a HTML table for technical details for one
     * package.
     **/
    QString simpleTable( ZyppSel	selectable,
			 ZyppPkg 	pkg );


    /**
     * Returns a string containing a HTML table for technical details for two
     * package instances: The installed instance and an alternate instance.
     * ( usually the candidate instance ).
     **/
    QString complexTable( ZyppSel	selectable,
			  ZyppPkg 	installedPkg,
			  ZyppPkg 	candidatePkg );

    /**
     * Returns a string containing HTML code for a package's authors list.
     **/
    QString authorsListCell( ZyppPkg pkg ) const;

    /**
     * Format an RPM group. Retrieves the translated ( ! ) version.
     **/
    QString formatRpmGroup( ZyppPkg pkg ) const;
};


#endif // ifndef YQPkgTechnicalDetailsView_h
