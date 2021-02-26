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

   File:       NCPkgPopupDescr.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupDescr_h
#define NCPkgPopupDescr_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;
class NCRichText;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupDescr
//
//	DESCRIPTION :
//
class NCPkgPopupDescr : public NCPopup {

    NCPkgPopupDescr & operator=( const NCPkgPopupDescr & );
    NCPkgPopupDescr            ( const NCPkgPopupDescr & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCRichText *descrText;
    NCLabel *headline;
    
    NCPackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPopupDescr( const wpos at, NCPackageSelector * pkger );
    
    virtual ~NCPkgPopupDescr();

    virtual long nicesize(YUIDimension dim);

    bool fillData( ZyppPkg pkgPtr, ZyppSel slbPtr );

    void createLayout( );

    NCursesEvent showInfoPopup( ZyppPkg pkgPtr, ZyppSel slbPtr );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDescr_h
