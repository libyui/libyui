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

   File:       NCPopupPkgTable.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupPkgTable_h
#define NCPopupPkgTable_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupPkgTable
//
//	DESCRIPTION :
//
class NCPopupPkgTable : public NCPopup {

    NCPopupPkgTable & operator=( const NCPopupPkgTable & );
    NCPopupPkgTable            ( const NCPopupPkgTable & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    PackageSelector * packager;

    int hDim;
    int vDim;
    
protected:

    virtual NCursesEvent wHandleHotkey( int ch );

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupPkgTable( const wpos at, PackageSelector * pkger );
    
    virtual ~NCPopupPkgTable();

    virtual long nicesize(YUIDimension dim);

    bool fillAutoChanges( NCPkgTable * pkgTable );

    void createLayout( );

    NCursesEvent & showInfoPopup( );

    void setNiceSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgTable_h
