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

    // the dependencies (index corresponds to line in package list)
    vector<std::pair<PkgDep::ErrorResult, std::string> > dependencies;

    map<std::string, bool> ignoreDependencies;
    
    NCPushButton * cancelButton;
    NCPushButton * solveButton;		
    NCPushButton * ignoreButton;
    NCPushButton * ignoreAllButton;
    
    NCPkgTable * deps;			// the conflict/alternative packages

    NCLabel * head;			// the headline
    
    NCLabel * errorLabel1; 		// the error message
    NCLabel * errorLabel2;

    PackageSelector * packager;		// connection to the package selector
    
    // method to solve the dependencies
    virtual bool solveInstall( PkgDep::ResultList & goodList, PkgDep::ErrorResultList & badList ) = 0;

    // methods to create different text for package/selection popup
    virtual string getHeadline() = 0;
    virtual string getLabelRequire1() = 0;
    virtual string getLabelRequBy1() = 0;
    virtual string getLabelRequBy2() = 0; 
    virtual string getLabelConflict2() = 0;
    virtual string getLabelContinueRequ() = 0;
    // set the tabel type
    virtual void setDepsTableType() = 0;
    
    void createLayout();
    

    bool addDepsLine( NCPkgTable * table, const PkgDep::ErrorResult & error, string kind );

    string getReferersList( const PkgDep::ErrorResult & error );
    
protected:

    NCPkgTable * pkgs;			// the list of packages with unresolved deps
    
    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:

    NCPopupDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupDeps();

    virtual long nicesize(YUIDimension dim);

    NCursesEvent showDependencyPopup( );

    bool showDependencies( );
    
    bool evaluateErrorResult( NCPkgTable * table,
			     const PkgDep::ErrorResultList & errorlist );

    bool concretelyDependency( int index );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDeps_h
