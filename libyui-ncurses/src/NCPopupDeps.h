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

    map<PMObjectPtr, list<PMObjectPtr> > conflicts;
    map<PMObjectPtr, list<PMObjectPtr> > alternatives;
    map<PMObjectPtr, list<PMObjectPtr> > broken;
    
    NCPushButton * cancelButton;
    NCPushButton * okButton;
    NCPushButton * solveButton;

    NCPkgTable * pkgs;
    NCPkgTable * deps;

    NCMenuButton * depsMenu;

    PackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupDeps();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline );

    NCursesEvent showDependencyPopup( bool solve );

    void checkDependencies( );
    
    bool fillDepsPackageList( NCPkgTable * table, list<PMObjectPtr> badPkgs );

    void concretelyDependency( PMObjectPtr pkgPtr );

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDeps_h
