/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA


  File:       NCPkgMenuFilter.cc
  Author:     Gabriele Mohr <gs@suse.de>

*/


#ifndef NCPkgMenuFilter_h
#define NCPkgMenuFilter_h

#include <string>

#include <yui/YReplacePoint.h>
#include <yui/YWidget.h>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCMenuButton.h>

#include "NCPkgPatchSearch.h"


class NCPackageSelector;

#include "NCZypp.h"

class NCPkgMenuFilter : public NCMenuButton
{
public:

	enum PatchFilter
	{
	    F_Needed,
	    F_Unneeded,
	    F_All,
	    F_Recommended,
	    F_Security,
	    F_Optional,
	    F_Unknown
	};

    NCPkgMenuFilter & operator=( const NCPkgMenuFilter & );
    NCPkgMenuFilter            ( const NCPkgMenuFilter & );

private:
    NCPackageSelector *pkg;

    NCPkgPatchSearch *searchPopup;

    YItemCollection items;

    YMenuItem *needed;
    YMenuItem *unneeded;
    YMenuItem *allPatches;
    YMenuItem *recommended;
    YMenuItem *security;
    YMenuItem *optional;
    YMenuItem *search;


public:

    NCPkgMenuFilter (YWidget *parent, std::string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuFilter();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
