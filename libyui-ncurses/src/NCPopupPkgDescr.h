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
    NCPushButton * cancelButton;
    NCRichText *descrText;
    PackageSelector * packager;

    int hDim;
    int vDim;
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupPkgDescr( const wpos at, PackageSelector * pkger );
    
    virtual ~NCPopupPkgDescr();

    virtual long nicesize(YUIDimension dim);

    bool fillData( PMPackagePtr & pkgPtr );

    void createLayout( );

    NCursesEvent showInfoPopup( PMPackagePtr & pkgPtr );

    void setNiceSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgDescr_h
