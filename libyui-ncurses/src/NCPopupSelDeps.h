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
#include "y2pm/PkgDep.h"

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>


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
    
    // method to solve the dependencies
    bool solveInstall( PkgDep::ResultList & goodList, PkgDep::ErrorResultList & badList );

    string getLabelRequire();
    string getLabelRequBy();
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
