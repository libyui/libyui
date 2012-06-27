/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgFilterPattern.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgFilterPattern_h
#define NCPkgFilterPattern_h

#include <iosfwd>

#include <string>
#include <set>

#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCPackageSelector.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterPattern
//
//	DESCRIPTION :
//
class NCPkgFilterPattern : public NCPkgTable {

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
    bool fillPatternList ( );

    std::string showDescription( ZyppObj objPtr );	
   
    /**
     * Shows the popup with the add ons (package categories).
     * @return NCursesEvent
     */
    void showPatternPackages( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterPattern_h
