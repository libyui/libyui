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

   File:       NCPkgStrings.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgStrings_h
#define NCPkgStrings_h

#include "NCstring.h"
#include <string>



// Class to make available globally used names in package selection dialog,
// such as widget ids, text which has to be translated and so on

class NCPkgStrings
{
public:

    // Main menu entry Dependencies
    static const std::string Deps();

    // Main menu entry View
    static const std::string View();

    // Main menu entry 'Extras'
    static const std::string Extras();

    // Package Filters combo box label
    static const std::string Filter();

    static const std::string InstPkg();

    // Label below the table - pkg name follows
    static const std::string PackageName();

    // Actions menu (what to do with the pkg)
    static const std::string Actions();

    static const std::string Help();

    //  The headline of the help popup
    static const std::string PackageHelp();

    // strings in 'General Help'
    static const std::string HelpPkgGen1();
    static const std::string HelpPkgGen2();
    static const std::string HelpPkgGen3();
    static const std::string HelpPkgGen4();
    static const std::string HelpPkgGen5();
    static const std::string HelpPkgGen6();

    //  The headline of the help popup
    static const std::string PackageStatusHelp();

    // strings in 'Package Status and symbols'
    static const std::string HelpOnStatus1();
    static const std::string HelpOnStatus2();
    static const std::string HelpOnStatus3();
    static const std::string HelpOnStatus4();
    static const std::string HelpOnStatus5();

    //  The headline of the help popup
    static const std::string PackageFiltersHelp();

    // string in 'How to use filters'
    static const std::string HelpOnFilters1();
    static const std::string HelpOnFilters2();
    static const std::string HelpOnFilters3();
    static const std::string HelpOnFilters4();
    static const std::string HelpOnFilters5();

    //  The headline of the help popup
    static const std::string PackageMenuHelp();

    // strings in 'Useful functions in menu'
    static const std::string HelpPkgMenu1();
    static const std::string HelpPkgMenu2();
    static const std::string HelpPkgMenu2_2();
    static const std::string HelpPkgMenu2_3();
    static const std::string HelpPkgMenu3();
    static const std::string HelpPkgMenu4();
    static const std::string HelpPkgMenu5();

    // Search settings
    static const std::string SearchIn();
    static const std::string SearchPhrase();

    // table column headlines
    static const std::string PkgStatus();
    static const std::string PatternsLabel();
    static const std::string LangCode();
    static const std::string LangName();
    static const std::string RepoURL();
    static const std::string PkgName();
    static const std::string PkgSummary();
    static const std::string PkgVersion();
    static const std::string PkgInstSource();
    static const std::string PkgVersionNew();
    static const std::string PkgVersionInst();
    static const std::string PkgSize();
    static const std::string PkgArch();
    static const std::string PkgSource();
    static const std::string PatchKind();

    // Useful busy popups
    static const std::string Solving();
    static const std::string Saving();
    static const std::string Loading();

    /**
     *  The headline of the disk space popup
     */
    static const std::string DiskspaceLabel();

    // column headers, diskspace table
    static const std::string Partition();
    static const std::string UsedSpace();
    static const std::string FreeSpace();
    static const std::string TotalSpace();
    /**
     * The headline of the disk space popup
     */
    static const std::string DiskSpaceError();
    static const std::string DiskSpaceWarning();
    static const std::string MoreText();
    static const std::string MoreSpaceText();

    /**
     * bold text Version: (richtext)
     */
    static const std::string Version();

    /**
     * bold text Installed version: (richtext)
     */
    static const std::string InstVersion();

    /**
     * bold text License: (richtext)
     */
    static const std::string License();

    /**
     * bold text Media No.: (richtext)
     */
    static const std::string MediaNo();

    /**
     * bold text Size: (richtext)
     */
    static const std::string Size();

    /**
     * bold text Authors: (richtext)
     */
    static const std::string Authors();

    /**
     * bold text Provides: (richtext)
     */
    static const std::string Provides();

    /**
     * bold text Requires: (richtext)
     */
    static const std::string Requires();

    /**
     * bold text Required by: (richtext)
     */
    static const std::string PreRequires();

    /**
     * bold text Conflicts with: (richtext)
     */
    static const std::string Conflicts();

    static const std::string ListOfFiles();
    static const std::string LanguageDescription();
    /**
     *  text used for automatic changes popup
     */
    static const std::string AutoChangeLabel();
    static const std::string AutoChangeText1();
    static const std::string AutoChangeText2();

    /**
     * The headline of the help YOU popup
     */
    static const std::string YouHelp();

    /**
     * help text package status
     */
    static const std::string YouHelp1();
    static const std::string YouHelp2();
    static const std::string YouHelp3();
    static const std::string YouHelp4();
    static const std::string YouHelp5();

    static const std::string WarningLabel();
    static const std::string ErrorLabel();
    static const std::string NotifyLabel();

    /**
     *  The label of the OK button
     */
    static const std::string OKLabel();

    /**
     *  The label of the Cancel button
     */
    static const std::string CancelLabel();

    /**
     *  The label of the Continue button
     */
    static const std::string ContinueLabel();

    /**
     *  The label of the Yes button
     */
    static const std::string YesLabel();

    /**
     *  The label of the Accept button
     */
    static const std::string AcceptLabel();

    /**
     *  The label of the No button
     */
    static const std::string NoLabel();

    /**
     *  The label of the Solve button
     */
    static const std::string SolveLabel();


    static const std::string CancelText();
    /**
     *  The label of the language selections popup
     */
    static const std::string LanguageLabel();

    /**
     *  The label of the repositories selections popup
     */
    static const std::string RepoLabel();

    /**
     *  The label Filter: YOU Patches
     */
    static const std::string YOUPatches();
    static const std::string InstPatches();
    static const std::string Patches();

    /**
     *  The label for Filter: Search results
     */
    static const std::string SearchResults();

    /**
     *  The label for Filter: Update problem
     */
    static const std::string UpdateProblem();

    /**
     * The headline of the dependency popup
     */
    static const std::string PackageDeps();

    static const std::string HelpOnUpdate();

    /**
     * bold text Patch: (richtext)
     */
    static const std::string Patch();

    /**
     * Info line in empty patch list
     */
    static const std::string NoPatches();

    static const std::string MenuList();
    static const std::string Script();

    /**
     * Info about multiversion packages
     */
    static const std::string MultiversionHead();
    static const std::string MultiversionIntro();
    static const std::string MultiversionText();
    static const std::string NotMultiversionText();

    /**
     * Marking for package versions that are retracted.
     **/
    static const std::string RetractedLabel();


private:

    // Need no object of this class: hide default constructor
    NCPkgStrings();

    static int do_something(){ return 0; }

};


///////////////////////////////////////////////////////////////////

#endif // NCPkgStrings_h



