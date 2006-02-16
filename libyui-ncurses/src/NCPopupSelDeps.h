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

   File:       NCPopupSelDeps.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupSelDeps_h
#define NCPopupSelDeps_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopupDeps.h"
#include "NCLabel.h"
//#include "y2pm/PkgDep.h"

#include "YQZypp.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupSelDeps
//
//	DESCRIPTION :
//
class NCPopupSelDeps : public NCPopupDeps {

private:

    NCPopupSelDeps & operator=( const NCPopupSelDeps & );
    NCPopupSelDeps            ( const NCPopupSelDeps & );
    
    string getLabelRequire();
    string getLabelRequBy1();
    string getLabelRequBy2();    
    string getLabelConflict();
    string getLabelContinueRequ();
    
    string getHeadline();    

protected:

    void setDepsTableType();
    
public:
       
    NCPopupSelDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupSelDeps();
 
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgDeps_h
