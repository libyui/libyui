/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

   File:       NCPkgMenuDeps.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuDeps.h"
#include "NCPackageSelector.h"

#define CHECK_BOX "[ ] "
#define NO_CHECK_BOX "    "

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuDeps::NCPkgMenuDeps (YWidget *parent, std::string label, NCPackageSelector *pkger )
	: NCMenuButton( parent, label)
	, pkg (pkger)
{
    createLayout();
}

NCPkgMenuDeps::~NCPkgMenuDeps()
{

}

void NCPkgMenuDeps::setSelected( YMenuItem *item, bool selected)
{
    std::string oldLabel = item->label();
    char sel = 'x';
    
    if ( item == cleanDepsOnRemove || item == allowVendorChange )
        sel = '+';
            
    std::string newLabel = oldLabel.replace(1,1,1, selected ? sel : ' ');

    item->setLabel( newLabel);
}

void NCPkgMenuDeps::createLayout()
{

    autoCheckDeps = new YMenuItem( CHECK_BOX + _( "&Automatic Dependency Check" ) );
    items.push_back( autoCheckDeps );
    setSelected( autoCheckDeps, pkg->AutoCheck() );

    checkNow = new YMenuItem( NO_CHECK_BOX + _( "&Check Dependencies Now" ) );
    items.push_back( checkNow );

    verifySystemOpt = new YMenuItem( CHECK_BOX + _( "&System Verification Mode" ) );
    items.push_back( verifySystemOpt );
    setSelected( verifySystemOpt, pkg->VerifySystem() );

    verifySystem = new YMenuItem( NO_CHECK_BOX + _( "&Verify System Now" ) );
    items.push_back( verifySystem );

    installRecommendedOpt = new YMenuItem( CHECK_BOX + _( "Install &Recommended Packages" ) );
    items.push_back( installRecommendedOpt );
    setSelected( installRecommendedOpt, pkg->InstallRecommended() );

    installRecommendedNow = new YMenuItem( NO_CHECK_BOX + _( "&Install Recommended Packages for Already Installed Packages Now" ) );
    items.push_back( installRecommendedNow );

    cleanDepsOnRemove = new YMenuItem( CHECK_BOX + _( "Cleanup when &Deleting Packages (Temporary Change)" ));
    items.push_back ( cleanDepsOnRemove );
    setSelected( cleanDepsOnRemove, pkg->isCleanDepsOnRemove() );

    allowVendorChange = new YMenuItem( CHECK_BOX + _( "Allow &Vendor Change (Temporary Change)" ) );
    items.push_back ( allowVendorChange );
    setSelected( allowVendorChange, pkg->isAllowVendorChange() );

    testCase = new YMenuItem( NO_CHECK_BOX + _( "&Generate Dependency Solver Testcase" ) );
    items.push_back( testCase );

    addItems( items );
}

bool NCPkgMenuDeps::handleEvent( const NCursesEvent & event)
{
    if (!event.selection)
	return false;

    if (event.selection == checkNow )
	return checkDependencies();
    else if (event.selection == autoCheckDeps)
	return setAutoCheck();
    else if (event.selection == verifySystem)
	return verify();
    else if (event.selection == verifySystemOpt )
	return setVerifySystem();
    else if (event.selection == cleanDepsOnRemove )
	return setCleanDepsOnRemove();
    else if (event.selection == installRecommendedOpt )
	return setInstallRecommended();
    else if (event.selection == installRecommendedNow )
	return doInstallRecommended();
    else if (event.selection == allowVendorChange )
	return setAllowVendorChange();
    else if (event.selection == testCase)
	return generateTestcase();
    return true;
}

static
void popupInfo(const wsze size, const std::string & text)
{
    wpos pos((NCurses::lines() - size.H) / 2, (NCurses::cols() - size.W) / 2);
    NCPopupInfo * info = new NCPopupInfo( pos,
                                          "", text,
					  NCPkgStrings::OKLabel() );
    info->setPreferredSize(size.W, size.H);
    info->showInfoPopup();
    YDialog::deleteTopmostDialog();
}

bool NCPkgMenuDeps::checkDependencies()
{
    bool ok = false;

    if ( pkg->DepsPopup() )
    {
        yuiMilestone() << "Checking dependencies" << endl;
        pkg->checkNow (  &ok );
    }

    if ( ok )
    {
        popupInfo(wsze(5, 35), _( "All package dependencies are OK." ));
    }

    // update the package list and the disk space info
    pkg->updatePackageList();
    pkg->showDiskSpace();

    return true;
}

bool NCPkgMenuDeps::doInstallRecommended()
{
    bool ok = false;

    yuiMilestone() << "Adding recommended packages" << endl;

    pkg->saveState();
    pkg->doInstallRecommended (  &ok );

    //display the popup with automatic changes
    NCPkgPopupTable * autoChangePopup =
        new NCPkgPopupTable( wpos( 3, 8 ), pkg,
                             // headline of a popup with packages
                             _("Automatic Changes"),
                             // part 1 of a text explaining the list of packages which follow
                             _("Being recommended by already installed packages, the following"),
                             // part 2 of the text
                             _("packages have been automatically selected for installation:")
                             );

    NCursesEvent input = autoChangePopup->showInfoPopup();
    if ( input == NCursesEvent::cancel )
    {
        // user clicked on Cancel
        pkg->restoreState();
    }
    YDialog::deleteTopmostDialog();	// delete NCPopupInfo dialog

    // update the package list and the disk space info
    pkg->updatePackageList();
    pkg->showDiskSpace();

    return true;
}


bool NCPkgMenuDeps::generateTestcase()
{
    std::string testCaseDir = "/var/log/YaST2/solverTestcase";

    yuiMilestone() << "Generating solver test case START" << endl;
    bool success = zypp::getZYpp()->resolver()->createSolverTestcase( testCaseDir );
    yuiMilestone() <<  "Generating solver test case END" << endl;

    if ( success )
    {
        popupInfo(wsze(8, 40),
                  _( "Dependency resolver test case written to " ) + "<br>"
                  + testCaseDir
                  );
    }
    return success;

}

bool NCPkgMenuDeps::setAutoCheck()
{
    pkg->setAutoCheck( !pkg->AutoCheck() );
    setSelected( autoCheckDeps, pkg->AutoCheck() );

    return true;
}

bool NCPkgMenuDeps::setVerifySystem()
{
    pkg->setVerifySystem( !pkg->VerifySystem() );
    setSelected( verifySystemOpt, pkg->VerifySystem() );

    return true;
}

bool NCPkgMenuDeps::setCleanDepsOnRemove()
{
    pkg->setCleanDepsOnRemove( !pkg->isCleanDepsOnRemove() );
    setSelected( cleanDepsOnRemove, pkg->isCleanDepsOnRemove() );

    return true;
}

bool NCPkgMenuDeps::setInstallRecommended()
{
    pkg->setInstallRecommended( !pkg->InstallRecommended() );
    setSelected( installRecommendedOpt, pkg->InstallRecommended() );

    return true;
}


bool NCPkgMenuDeps::setAllowVendorChange()
{
    pkg->setAllowVendorChange( !pkg->isAllowVendorChange() );
    setSelected( allowVendorChange, pkg->isAllowVendorChange() );

    return true;
}

bool NCPkgMenuDeps::verify()
{
    bool ok = false;

    yuiMilestone() << "Verifying system" << endl;

    pkg->saveState();
    //call the solver (with S_Verify it displays no popup)
    pkg->systemVerification( &ok );

    //display the popup with automatic changes
    NCPkgPopupTable * autoChangePopup =
        new NCPkgPopupTable( wpos( 3, 8 ), pkg,
                             // headline of a popup with packages
                             _("Automatic Changes"),
                             // part 1 of a text explaining the list of packages which follow
                             _("To fulfill the dependencies of already installed packages following"),
                             // part 2 of the text
                             _("packages have been automatically selected for installation:")
                             );
    NCursesEvent input = autoChangePopup->showInfoPopup();

    if ( input == NCursesEvent::cancel )
    {
        // user clicked on Cancel
        pkg->restoreState();
    }
    if ( ok && input == NCursesEvent::button )
    {
        // dependencies OK, no automatic changes/the user has accepted the changes
        popupInfo(wsze(5, 35), _( "System dependencies verify OK." ));
    }

    YDialog::deleteTopmostDialog();	// delete NCPopupInfo dialog

    pkg->updatePackageList();
    pkg->showDiskSpace();
    // the verify call sets the option verify to true
    setSelected( verifySystemOpt, true );
    return true;
}
