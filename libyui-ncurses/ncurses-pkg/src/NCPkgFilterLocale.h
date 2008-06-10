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

   File:       NCPkgPopupRepo.h

   Author:     Bubli <kmachalkova@suse.cz> 

/-*/

#ifndef NCPkgFilterLocale_h
#define NCPkgFilterLocale_h

#include <vector>
#include "NCTable.h"
#include "NCPackageSelector.h"
#include "NCZypp.h"
#include <zypp/sat/LocaleSupport.h>

class NCPkgLocaleTag : public YTableCell
{

private:

    zypp::sat::LocaleSupport locale;

public:
   
    NCPkgLocaleTag ( zypp::sat::LocaleSupport locale, string status );

    ~NCPkgLocaleTag() {  };

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
    void addLine ( zypp::sat::LocaleSupport, const vector <string> & cols, string status );
    void fillLocaleList();
    void showLocalePackages();

    NCPkgLocaleTag* getTag (const int & index );
    zypp::sat::LocaleSupport getLocale( int index );
    string status( zypp::Locale lang );
    void toggleStatus();
    NCursesEvent wHandleInput( wint_t ch );
};

#endif
