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


  File:       NCPkgPackageDetails.h

*/


#ifndef NCPkgPackageDetails_h
#define NCPkgPackageDetails_h

#include <yui/ncurses/NCRichText.h>

#include "NCZypp.h"


class NCPackageSelector;


class NCPkgPackageDetails : public NCRichText
{
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
