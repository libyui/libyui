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

   File:       NCPopupDeps.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupPkgDeps_h
#define NCPopupPkgDeps_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopupDeps.h"
#include "NCLabel.h"
//#include "y2pm/PkgDep.h"

#include "YQZypp.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupPkgDeps
//
//	DESCRIPTION :
//
class NCPopupPkgDeps : public NCPopupDeps {

private:

    NCPopupPkgDeps & operator=( const NCPopupPkgDeps & );
    NCPopupPkgDeps            ( const NCPopupPkgDeps & );

    string getLabelRequire();
    string getLabelRequBy1();
    string getLabelRequBy2();
    string getLabelConflict();
    string getLabelContinueRequ();
    
    string getHeadline();
    
protected:

    void setDepsTableType();
    
public:

    NCPopupPkgDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupPkgDeps();

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgDeps_h
