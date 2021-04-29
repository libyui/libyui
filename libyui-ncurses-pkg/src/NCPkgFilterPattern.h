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


  File:       NCPkgFilterPattern.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgFilterPattern_h
#define NCPkgFilterPattern_h

#include <iosfwd>
#include <string>
#include <set>

#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>

#include "NCPackageSelector.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterPattern
//
//	DESCRIPTION :
//
class NCPkgFilterPattern : public NCPkgTable
{
    NCPkgFilterPattern & operator=( const NCPkgFilterPattern & );
    NCPkgFilterPattern            ( const NCPkgFilterPattern & );

private:

    NCPackageSelector * packager;		// connection to the PackageSelector,

protected:

    std::string getCurrentLine();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgFilterPattern( YWidget *parent, YTableHeader *header, NCPackageSelector * pkg );
    virtual ~NCPkgFilterPattern();

    void createLayout( YWidget *parent );

    /**
     * Fills the std::list with the available selections (and the status info)
     * @return bool
     */
    bool fillPatternList();

    std::string showDescription( ZyppObj objPtr );

    /**
     * Shows the popup with the add ons (package categories).
     * @return NCursesEvent
     */
    void showPatternPackages();

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterPattern_h
