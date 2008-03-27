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
#include "NCPackageSelector.h"
#include "NCZypp.h"

class NCPkgPackageDetails : public NCRichText {


    NCPkgPackageDetails & operator=( const NCPkgPackageDetails & );
    NCPkgPackageDetails            ( const NCPkgPackageDetails & );

private:
    NCPackageSelector *pkg;

public:

    NCPkgPackageDetails( YWidget *parent, string initial_text, NCPackageSelector * pkger );

    string createText( list <string> info, bool oneline );

    string createRelLine( const zypp::Capabilities & info );

    string commonHeader( ZyppObj pkgPtr );

    void longDescription( ZyppObj pkgPtr );

    void technicalData( ZyppObj pkgPtr, ZyppSel slbPtr );

    void fileList (ZyppSel slbPtr);

    void dependencyList( ZyppObj objPtr, ZyppSel slbPtr );    

};
#endif
