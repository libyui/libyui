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

   File:       NCPkgPackageDetails.h

/-*/

#ifndef NCPkgPackageDetails_h
#define NCPkgPackageDetails_h 

#include "NCRichText.h"
#include "NCZypp.h"

class NCPackageSelector;


class NCPkgPackageDetails : public NCRichText {


    NCPkgPackageDetails & operator=( const NCPkgPackageDetails & );
    NCPkgPackageDetails            ( const NCPkgPackageDetails & );

private:
    NCPackageSelector *pkg;

public:

    NCPkgPackageDetails( YWidget *parent, std::string initial_text, NCPackageSelector * pkger );

    std::string createText( std::list <std::string> info, bool oneline );

    std::string createHtmlText( std::string description );
    
    std::string createRelLine( const zypp::Capabilities & info );

    std::string commonHeader( ZyppObj pkgPtr );

    void longDescription( ZyppObj pkgPtr );

    void technicalData( ZyppObj pkgPtr, ZyppSel slbPtr );

    void fileList (ZyppSel slbPtr);

    void dependencyList( ZyppObj objPtr, ZyppSel slbPtr );    

    bool patchDescription( ZyppObj objPtr, ZyppSel selectable );
};
#endif
