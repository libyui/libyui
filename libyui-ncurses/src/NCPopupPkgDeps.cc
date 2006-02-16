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

string NCPopupPkgDeps::getLabelRequire()
{
    return PkgNames::LabelRequire();
}

string NCPopupPkgDeps::getLabelRequBy1()
{
    return PkgNames::LabelPkgRequBy1();
}

string NCPopupPkgDeps::getLabelRequBy2()
{
    return PkgNames::LabelPkgRequBy2();
}

string NCPopupPkgDeps::getLabelConflict()
{
    return PkgNames::LabelPkgConflict();
}

string NCPopupPkgDeps::getLabelContinueRequ()
{
    return PkgNames::ContinueRequ();
}

string NCPopupPkgDeps::getHeadline()
{
    return PkgNames::PackageDeps();
}
