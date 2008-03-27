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

   File:       NCPkgPopupTable.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupTable_h
#define NCPkgPopupTable_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;
class NCPackageSelector;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupTable
//
//	DESCRIPTION :
//
class NCPkgPopupTable : public NCPopup {

    NCPkgPopupTable & operator=( const NCPkgPopupTable & );
    NCPkgPopupTable            ( const NCPkgPopupTable & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCPackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPopupTable( const wpos at, NCPackageSelector * pkger );
    
    virtual ~NCPkgPopupTable();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    bool fillAutoChanges( NCPkgTable * pkgTable );

    void createLayout( );

    NCursesEvent showInfoPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupTable_h
