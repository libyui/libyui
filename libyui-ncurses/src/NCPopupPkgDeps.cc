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

   File:       NCPopupPkgDeps.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupPkgDeps.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCPkgTable.h"
#include "NCMenuButton.h"
#include "NCPushButton.h"
#include "NCPopupInfo.h"
#include "ObjectStatStrategy.h"

#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDeps::NCPopupPkgDeps
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupPkgDeps::NCPopupPkgDeps( const wpos at, PackageSelector * pkger )
    : NCPopupDeps( at, pkger )
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDeps::~NCPopupPkgDeps
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupPkgDeps::~NCPopupPkgDeps()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDeps::solveInstall
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupPkgDeps::solveInstall( PkgDep::ResultList & goodList, PkgDep::ErrorResultList & badList )
{
    return Y2PM::packageManager().solveInstall( goodList, badList );
}

string NCPopupPkgDeps::getLabelRequire1()
{
    return PkgNames::LabelRequire1().str();
}

string NCPopupPkgDeps::getHeadline()
{
    return PkgNames::PackageDeps().str();
}

void NCPopupPkgDeps::setDepsTableType()
{
    // set status strategy
    ObjectStatStrategy * strategy =  new DependencyStatStrategy();
    pkgs->setTableType( NCPkgTable::T_Dependency, strategy );
}
