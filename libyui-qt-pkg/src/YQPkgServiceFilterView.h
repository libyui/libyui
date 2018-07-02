/**************************************************************************
Copyright (C) 2018 SUSE LLC
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

*/


#ifndef YQPkgServiceFilterView_h
#define YQPkgServiceFilterView_h

#include "YQPkgSecondaryFilterView.h"

class QWidget;
class YQPkgServiceList;

class YQPkgServiceFilterView : public YQPkgSecondaryFilterView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgServiceFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgServiceFilterView();

    /**
     * Is any enabled libzypp service present?
     */
    static bool any_service();

protected:

    virtual void primaryFilter();
    virtual void primaryFilterIfVisible();

    // Data members
    // list of services, owned by the parent widget
    YQPkgServiceList *		_serviceList;
};

#endif // ifndef YQPkgServiceFilterView_h
