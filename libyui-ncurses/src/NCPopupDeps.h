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
#ifndef NCPopupDeps_h
#define NCPopupDeps_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "y2pm/PkgDep.h"

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>

class NCPkgTable;
class NCPushButton;
class NCMenuButton;
class PackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupDeps
//
//	DESCRIPTION :
//
class NCPopupDeps : public NCPopup {

    NCPopupDeps & operator=( const NCPopupDeps & );
    NCPopupDeps            ( const NCPopupDeps & );

private:

    // the dependencies
    vector<PkgDep::ErrorResult> dependencies;	// index corresponds to line in package list
    
    NCPushButton * cancelButton;
    NCPushButton * okButton;
    NCPushButton * solveButton;		

    NCPkgTable * pkgs;			// the list of packages with unresolved deps
    NCPkgTable * deps;			// the conflict/alternative packages

    NCMenuButton * depsMenu;		// the menu button

    NCLabel * errorLabel1; 		// the error message
    NCLabel * errorLabel2;
    
    PackageSelector * packager;		// connection to the package selector

    string getDependencyKind(  PkgDep::ErrorResult error );
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupDeps();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline );

    NCursesEvent showDependencyPopup( );

    void showDependencies( );
    
    bool fillDepsPackageList( NCPkgTable * table );

    void evaluateErrorResult(  PkgDep::ErrorResultList errorlist );
    
    bool concretelyDependency( int index );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDeps_h
