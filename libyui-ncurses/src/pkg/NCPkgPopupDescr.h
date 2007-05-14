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

   File:       NCPopupPkgDescr.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupPkgDescr_h
#define NCPopupPkgDescr_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;
class NCRichText;
class PackageSelector;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupPkgDescr
//
//	DESCRIPTION :
//
class NCPopupPkgDescr : public NCPopup {

    NCPopupPkgDescr & operator=( const NCPopupPkgDescr & );
    NCPopupPkgDescr            ( const NCPopupPkgDescr & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCRichText *descrText;
    NCLabel *headline;
    
    PackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPopupPkgDescr( const wpos at, PackageSelector * pkger );
    
    virtual ~NCPopupPkgDescr();

    virtual long nicesize(YUIDimension dim);

    bool fillData( ZyppPkg pkgPtr, ZyppSel slbPtr );

    void createLayout( );

    NCursesEvent showInfoPopup( ZyppPkg pkgPtr, ZyppSel slbPtr );

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgDescr_h
