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

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuDeps::NCPkgMenuDeps (YWidget *parent, string label, NCPackageSelector *pkger )
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
    string oldLabel = item->label();

    string newLabel = oldLabel.replace(1,1,1, selected ? 'x' : ' ');

    item->setLabel( newLabel);
}

void NCPkgMenuDeps::createLayout()
{

    autoCheckDeps = new YMenuItem( CHECK_BOX + _("&Automatic Dependency Check") );
    items.push_back( autoCheckDeps );
    setSelected( autoCheckDeps, pkg->isAutoCheck() );
    
    checkNow = new YMenuItem( NO_CHECK_BOX + _("&Check Dependencies Now") );
    items.push_back( checkNow );

    verifySystem = new YMenuItem( NO_CHECK_BOX + _("&Verify System Now") );
    items.push_back( verifySystem );

    ignoreAlreadyRecommendedOpt = new YMenuItem( CHECK_BOX + _("&Ignore Recommended Packages for Already Installed Packages") );
    items.push_back( ignoreAlreadyRecommendedOpt );
    setSelected( ignoreAlreadyRecommendedOpt, pkg->isIgnoreAlreadyRecommended() );
    
    verifySystemOpt = new YMenuItem( CHECK_BOX + _("&System Verification Mode") );
    items.push_back( verifySystemOpt );
    
    cleanDepsOnRemove = new YMenuItem( CHECK_BOX + _( "&Cleanup when deleting packages" ) );
    items.push_back ( cleanDepsOnRemove );
    setSelected( cleanDepsOnRemove, pkg->isCleanDepsOnRemove() );

    allowVendorChange = new YMenuItem( CHECK_BOX + _( "&Allow vendor change" ) );
    items.push_back ( allowVendorChange );
    setSelected( allowVendorChange, pkg->isAllowVendorChange() );
    
    testCase = new YMenuItem( NO_CHECK_BOX + _("&Generate Dependency Solver Testcase") );
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
    else if (event.selection == ignoreAlreadyRecommendedOpt )
	return setIgnoreAlreadyRecommended();
    else if (event.selection == allowVendorChange )
	return setAllowVendorChange();
    else if (event.selection == testCase)
	return generateTestcase();
    return true;   
}

bool NCPkgMenuDeps::checkDependencies()
{
    NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-35)/2 ), "",
					  _( "All package dependencies are OK." ),
					  NCPkgStrings::OKLabel() );
    info->setPreferredSize( 35, 5 );

    bool ok = false;

    if ( pkg->DepsPopup() )
    {
        yuiMilestone() << "Checking dependencies" << endl;
        pkg->checkNow (  &ok );
    }
    
    if ( ok )
    {
        info->showInfoPopup();
        YDialog::deleteTopmostDialog();
    }
    
    // update the package list and the disk space info
    pkg->updatePackageList();
    pkg->showDiskSpace();
   
    return true;
}

bool NCPkgMenuDeps::generateTestcase()
{
    string testCaseDir = "/var/log/YaST2/solverTestcase";

    yuiMilestone() << "Generating solver test case START" << endl;
    bool success = zypp::getZYpp()->resolver()->createSolverTestcase( testCaseDir );
    yuiMilestone() <<  "Generating solver test case END" << endl;

    if ( success )
    {
        NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-8)/2, (NCurses::cols()-40)/2 ),
                                              "",
                                              _("Dependency resolver test case written to ") + "<br>"
                                              + testCaseDir
                                              );
        info->setPreferredSize( 40, 8 );
        info->showInfoPopup( );

        YDialog::deleteTopmostDialog();
    }
    return success;

}

bool NCPkgMenuDeps::setAutoCheck()
{
    pkg->AutoCheck( !pkg->isAutoCheck() );
    setSelected( autoCheckDeps, pkg->isAutoCheck() );

    return true;
}

bool NCPkgMenuDeps::setVerifySystem()
{
    pkg->setVerifySystem( !pkg->isVerifySystem() );
    setSelected( verifySystemOpt, pkg->isVerifySystem() );

    return true;
}

bool NCPkgMenuDeps::setCleanDepsOnRemove()
{
    pkg->setCleanDepsOnRemove( !pkg->isCleanDepsOnRemove() );
    setSelected( cleanDepsOnRemove, pkg->isCleanDepsOnRemove() );

    return true;
}

bool NCPkgMenuDeps::setIgnoreAlreadyRecommended()
{
    pkg->setIgnoreAlreadyRecommended( !pkg->isIgnoreAlreadyRecommended() );
    setSelected( ignoreAlreadyRecommendedOpt, pkg->isIgnoreAlreadyRecommended() );

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
    bool cancel = false;

    
    yuiMilestone() << "Verifying system" << endl;

    pkg->saveState();
    //call the solver (with S_Verify it displays no popup)
    cancel = pkg->verifySystem ( &ok );
    
    //display the popup with automatic changes
    NCPkgPopupTable * autoChangePopup = new NCPkgPopupTable( wpos( 3, 8 ), pkg );
    NCursesEvent input = autoChangePopup->showInfoPopup();
    
    if ( input == NCursesEvent::cancel )
    {
        // user clicked on Cancel
        pkg->restoreState();
        cancel = true;
    }
    if ( ok && input == NCursesEvent::button )
    {
        // dependencies OK, no automatic changes/the user has accepted the changes
        NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-5)/2, (NCurses::cols()-30)/2 ),
    				  "",
    				  _( "System dependencies verify OK." ),
    				  NCPkgStrings::OKLabel()
    				  );
         info->setPreferredSize( 35, 5 );
         info->showInfoPopup();
         YDialog::deleteTopmostDialog();
    }

    YDialog::deleteTopmostDialog();	// delete NCPopupInfo dialog

    pkg->updatePackageList();
    pkg->showDiskSpace();
    // the verify call sets the option verify to true
    setSelected( verifySystemOpt, pkg->isVerifySystem() );
    return true;
}
