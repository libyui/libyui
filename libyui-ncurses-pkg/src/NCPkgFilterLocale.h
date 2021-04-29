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


  File:       NCPkgFilterLocale.h
  Author:     Bubli <kmachalkova@suse.cz>

*/


#ifndef NCPkgFilterLocale_h
#define NCPkgFilterLocale_h

#include <vector>
#include <zypp/sat/LocaleSupport.h>
#include <yui/ncurses/NCTable.h>

#include "NCPackageSelector.h"
#include "NCZypp.h"


class NCPkgLocaleTag : public YTableCell
{

private:

    zypp::sat::LocaleSupport locale;

public:

    NCPkgLocaleTag ( zypp::sat::LocaleSupport locale, std::string status );

    ~NCPkgLocaleTag() {};

    zypp::sat::LocaleSupport getLocale() const	{ return locale; }

};

class NCPkgLocaleTable : public NCTable
{
private:

    NCPkgLocaleTable & operator=( const NCPkgLocaleTable & );
    NCPkgLocaleTable            ( const NCPkgLocaleTable & );

    NCPackageSelector *packager;
public:

    NCPkgLocaleTable  ( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg);

    virtual ~NCPkgLocaleTable() {};

    void fillHeader();
    void addLine ( zypp::sat::LocaleSupport, const std::vector <std::string> & cols, std::string status );
    void fillLocaleList();
    void showLocalePackages();

    NCPkgLocaleTag* getTag (const int & index );
    zypp::sat::LocaleSupport getLocale( int index );
    std::string status( zypp::Locale lang );
    void cycleStatus();
    NCursesEvent wHandleInput( wint_t ch );
};

#endif
