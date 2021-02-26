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

   File:       NCPkgStrings.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/


#include "NCPkgStrings.h"

#include "NCi18n.h"

#include <libintl.h>


/*
  Textdomain "ncurses-pkg"
*/

const std::string NCPkgStrings::Deps()
{
    //menu entry 1 - all about pkg dependencies
    static const std::string value = _( "&Dependencies" );
    return value;
}

const std::string NCPkgStrings::View()
{
    //menu entry 2 - display different kinds of info on pkgs
    static const std::string value = _( "&View" );
    return value;
}

const std::string NCPkgStrings::Extras()
{
    //menu entry 3 - miscellaneous stuff
    static const std::string value = _( "&Extras" );
    return value;
}

const std::string NCPkgStrings::Filter()
{
    //pick a package filter - patterns, langs, repos, search,...
    static const std::string value = _( "&Filter" );
    return value;
}


const std::string NCPkgStrings::InstPkg()
{
    // All installed Packages
    static const std::string value = _( "Installed Packages" );
    return value;
}

const std::string NCPkgStrings::PackageName()
{
    // Label under the pkg table - pkg name follows
    static const std::string value = _( "Package: " );
    return value;
}

const std::string NCPkgStrings::Actions()
{
    // Actions on pkgs (install, remove,..)
    static const std::string value = _( "A&ctions" );
    return value;
}

const std::string NCPkgStrings::Help()
{
    // Help button
    static const std::string value = _( "&Help" );
    return value;
}

const std::string NCPkgStrings::PackageHelp()
{
    // the headline of the help window
    static const std::string value = _( "General Help" );
    return value;
}

const std::string NCPkgStrings::HelpPkgGen1()
{
    // part1 of help text package installation
    static const std::string value = _( "<p><b>Welcome to the package selector</b></p><p>This tool will help you to manage the software on your system. You can install, update or remove single packages, as well as patterns (sets of packages serving certain purpose) or languages. Usually, you do not need to care about package dependencies when installing or removing anything, the solver will do it for you. The package selector consists of three main parts: <b>filters</b>, <b>package table</b> and <b>menu</b>.</p>" );
    return value;

}

const std::string NCPkgStrings::HelpPkgGen2()
{
    // part of help text package installation
    static const std::string value = _( "<p><b>Filter</b> on left panel is designed for easy orientation in a large amount of packages. Use filters to display only packages from a certain repository or in a selected pattern (for example, Games or C/C++ Development) or to search for particular keywords. More information on filters can be found in <i>How to use filters</i>.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgGen3()
{
    // additional help text for post installation
    static const std::string value = _( "<p><b>Package table</b> is the main component of the package selector. You will see a list of packages matching the current filter (for example, the selected RPM group or search result). Each line of the package table has several columns:</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgGen4 ()
{
    // part2 of help text package installation
    static const std::string value =  _( "<ol><li>Package status (for more information see <i>Package Status and Symbols</i>)</li> <li>Package name</li><li>Package summary</li><li>Available version (in some of the configured repositories)</li> <li>Installed version(empty for not yet installed packages)</li> <li>Package size</li></ol>" );
    
    return value;
}

const std::string NCPkgStrings::HelpPkgGen5()
{
    // part3 of help text package installation
    static const std::string value =  _( "<p>The <b>Actions</b> menu below the table allows you to change the status of a selected package (or all packages in the list), for example, to delete a package or select an additional package for installation. The status change can also be done directly by pressing the key specified in the menu item (for detailed information about the package status, see <i>Package Status and Symbols</i>).</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgGen6()
{
    // part4 of help text package installation
    static const std::string value =  _( "<p><b>Menu</b> provides functions related to the handling of package dependencies. Display relevant information on packages or perform actions like opening the repository editor. For more information, see <i>Useful Functions in Menu</i>.</p>" );
    return value;
}

const std::string NCPkgStrings::PackageStatusHelp()
{
    // the headline of the help window
    static const std::string value = _( "Package Status and Symbols" );
    return value;
}

const std::string NCPkgStrings::HelpOnStatus1()
{
    // part 1 of help text package status
    static const std::string value = _( "<p>The package status can be changed using the <i>Actions</i> menu or the keys specified in the menu items. For example, use '+' to install an additional package.</p><p>The \"Taboo\" status means the package should never be installed. On the contrary, the \"Locked\" status means that the installed version of a package should always be kept.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnStatus2()
{
    // part 2 of help text package status
    static const std::string value = _( "<p>You can also use <b>RET</b> or <b>SPACE</b> to toggle the package status. The <i>Actions</i> menu also allows you to change the status for all packages in the list (select 'All Listed Packages').</p>" );
    return value;
}
const std::string NCPkgStrings::HelpOnStatus3()
{
    // part 3 of help text package status
    static const std::string value = _( "<p>The meaning of the status flags:</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnStatus4()
{
    // help text package status
    static const std::string value =  _( "<p><b> + </b>:  package will be installed</p><p><b>a+ </b>: package will be installed automatically</p><p><b> > </b>: package will be updated</p><p><b>a> </b>: package will be automatically updated</p><p><b> i </b>: package is installed</p><p><b> - </b>:  package will be deleted</p><p><b>---</b>: never install this package (taboo)</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnStatus5()
{
    // help text package status
    static const std::string value =  _( "<p><b>-i-</b>: keep the installed version and never update or delete it ( package locked )</p><p>Status information for pattern and languages:</p><p><b> i </b>: All requirements of this pattern/language are satisfied</p>" );
    return value;
}

const std::string NCPkgStrings::PackageFiltersHelp()
{
     // label for an error popup
    static const std::string value = _( "How to Use Filters" );
    return value;
}

const std::string NCPkgStrings::HelpOnFilters1()
{
    // help text package status
    static const std::string value =  _( "<p><b>Filter</b> allow you to filter all available packages according to the selected criteria. Package filters are based on package properties (repository, RPM group), package \"containers\" (patterns, languages), package classification or search results. Select the desired filter from the drop-down menu. Specific filters are described below.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnFilters2()
{
    // help text package status
    static const std::string value =  _( "<p><b>Patterns</b> describe the features and functions a system should have (for example, X server or Console tools). Each pattern contains a set of packages it requires (must have), recommends (should have) and suggests (may have). If you select a pattern for installation, update, or deletion, the solver will run and change the status of subordinate packages accordingly.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnFilters3()
{
    // help text package status
    static const std::string value =  _( "<p><b>Languages</b> are package containers very much like patterns. They contain packages with translations, dictionaries and other language-specific files for a selected language. <b>RPM Groups</b> are not package containers that can be installed. Instead, membership in a certain RPM group is a property of the package itself. They have a hierarchical (tree) structure. The <b>Repositories</b> filter displays packages available from a specific repository. </p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnFilters4()
{
    // help text package search
    static const std::string value = _( "<p>To use the <b>Search</b> filter, enter a keyword (or part of keyword) for the package search. For example, search for all 3D packages using the expression \"3d\". You can also search in package descriptions, RPM provides or requires. Select the appropriate check box and click the 'Search' button.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpOnFilters5()
{
    // help text package status
static const std::string value =  _( "<p>The <b>Installation summary</b> presents an overview of packages whose status has changed during this session (e.g. marked for installation or removal), either by the user or automatically by the solver. The filter <b>Package Classification</b> provides information about <i>Recommended</i>, <i>Suggested</i>, <i>Orphaned</i> and <i>Unneeded</i> packages.</p>" );
    return value;
}

const std::string NCPkgStrings::PackageMenuHelp()
{
     // label for an error popup
    static const std::string value = _( "Useful Functions in Menu" );
    return value;
}

const std::string NCPkgStrings::HelpPkgMenu1()
{
    static const std::string value =  _( "<p><b>Dependencies:</b><br> This menu offers various actions related to the handling of package dependencies. By default, package dependencies are checked with every status change. You will be informed about package conflicts in a dialog proposing possible conflict resolutions. To resolve the conflict, select one of the offered solutions and press 'OK -- Try Again'.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgMenu2()
{
    static const std::string value =  _( "<p>To disable dependency checking on every status change, toggle <i>Automatic Dependency Check</i> OFF. You can check dependencies manually by selecting <i>Check Dependencies Now</i>. The <i>Verify system</i> entry will check the dependencies of already installed packages and resolve conflicts non-interactively, marking missing packages for automatic installation if necessary. For debugging purposes, use <i>Generate Dependency Solver Testcase</i>. It will dump package dependencies data into the directory <tt>/var/log/YaST2/solverTestcase</tt>. This is usually what you need when asked for a \"solver testcase\" in Bugzilla.</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgMenu2_2()
{
    static const std::string value =  _(
        "<p>Available options for dependency checking are:<br>"
        "<i>Automatic Dependency Check</i> (see above), "
        "<i>Install Recommended Packages</i>: if ON, weak dependencies will be honored, "
        "<i>System Verification Mode</i>: repair dependencies of installed packages and solve immediately. "
        "Please note: after checking the system with <i>Verify System Now</i> the option <i>System Verification Mode</i> is ON (uncheck the option, if desired). "
        "These options are saved in the YaST configuration file <tt>/etc/sysconfig/yast2</tt>.</p>"
        );
    return value;
}

const std::string NCPkgStrings::HelpPkgMenu2_3()
 {
    static const std::string value =  _("<p>Advanced options:<br> <i>Cleanup when deleting packages</i>: remove dependent unused packages. <i>Allow vendor change</i>: package vendor may differ from vendor of installed package. These options will not be saved, they can only be set in the configuration file of the package library <tt>/etc/zypp/zypp.conf</tt>.</p>" );
    return value;
}
                                   
const std::string NCPkgStrings::HelpPkgMenu3()
{
    static const std::string value =  _( "<p><b>View:</b><br>Choose which information about the selected package will be displayed in the window below the package table. Available options are: package description, technical data (version, size, license etc.) package versions (all available), file list (all files included in the package) and dependencies (provides, requires etc.).</p>" );
    return value;
}

const std::string NCPkgStrings::HelpPkgMenu4()
{
    static const std::string value = _( "<p><b>Configuration:</b><br>This menu integrates package selector with the rest of package management utils. From here, you can <b>Launch Repository Manager</b> and edit configured repositories or register to update repository and configure periodic download of available updates (<b>Launch Online Update Configuration</b>). Also, you can pick one of the three possible behaviours of package selector at exit - in <b>Action after Package Installation</b> menu.</p>" );
    return value;
}
const std::string NCPkgStrings::HelpPkgMenu5()
{
    static const std::string value =  _( "<p><b>Extras:</b><br>Miscellaneous functions reside here. <i>Export Package List to File</i> will dump data on installed packages, patterns and languages into specified XML file. This file can be later read by <i>Import Package List from File</i> option e.g. on different computer. It will bring the set of packages on the target computer into the same state as described in provided XML file. <i>Show Available Disk Space</i> will show a popup table displaying disk usage and free disk space on currently mounted partition.</p>" );
    return value;
}


const std::string NCPkgStrings::SearchIn()
{
    // label of a frame with search settings
    static const std::string value = _( "&Search in " );
    return value;
}

const std::string NCPkgStrings::SearchPhrase()
{
    // begin: text/labels for search popups (use unique hotkeys until the end:)
    // text for the package search popup
    static const std::string value = _( "Search &Phrase" );
    return value;
}

const std::string NCPkgStrings::PkgStatus()
{
    // column header status
    static const std::string value =  std::string("     ");
    return value;
}

const std::string NCPkgStrings::PatternsLabel()
{
   // the label of the selections
    static const std::string value = _( "Patterns" );
    return value;
}

const std::string NCPkgStrings::LangCode()
{
    static const std::string value = _( "Code" );
    return value;
}

const std::string NCPkgStrings::LangName()
{
    static const std::string value = _( "Language" );
    return value;
}

const std::string NCPkgStrings::RepoURL()
{
    static const std::string value = _( "URL" );
    return value;

}

const std::string NCPkgStrings::PkgName()
{
    // column header package name (keep it short!)
    static const std::string value = _( "Name" );
    return value;
}

const std::string NCPkgStrings::PkgVersion()
{
    // column header installed package version (keep it short!)
    static const std::string value = _( "Version" );
    return value;
}

const std::string NCPkgStrings::PkgInstSource()
{
    // column header - repository of the package (e.g. openSUSE 10.3)
    static const std::string value = _( "Repository" );
    return value;
}

const std::string NCPkgStrings::PkgVersionNew()
{
    // column header available package version (keep it short - use abbreviation!)
    static const std::string value = _( "Avail. Vers." );
    return value;
}

const std::string NCPkgStrings::PkgVersionInst()
{
    // column header installed package version (keep it short - use abbreviation!)
    static const std::string value = _( "Inst. Vers." );
    return value;
}

const std::string NCPkgStrings::PkgSummary()
{
    // column header package description (keep it short!)
    static const std::string value = _( "Summary" );
    return value;
}


const std::string NCPkgStrings::PkgSize()
{
    // column header package size (keep it short!)
    static const std::string value = _( "Size" );
    return value;
}

const std::string NCPkgStrings::PkgArch()
{
    // column header package architecture  (keep it short!)
    static const std::string value = _( "Architecture" );
    return value;
}

const std::string NCPkgStrings::PatchKind()
{
    // column header patch kind (keep it short!)
    static const std::string value = _( "Kind" );
    return value;
}

const std::string NCPkgStrings::Solving()
{
    // a text for a small popup which is shown during package dependency checking
    static const std::string value = _( "Solving..." );
    return value;
}
const std::string NCPkgStrings::Saving()
{
    // a text for a small popup which is shown during writing package selection to a file
    static const std::string value = _( "Saving..." );
    return value;
}
const std::string NCPkgStrings::Loading()
{
    // a text for a samll popup which is shown during loading package selections from a file
    static const std::string value = _( "Loading..." );
    return value;
}

const std::string NCPkgStrings::DiskspaceLabel()
{
    // the headline of the disk space popup
    static const std::string value = _( "Disk Usage Overview" );
    return value;
}

const std::string NCPkgStrings::Partition()
{
    // column header name of the partition (keep it short!)
    static const std::string value =  _( "Partition" );
    return value;
}

const std::string NCPkgStrings::UsedSpace()
{
    // column header used disk space (keep it short!)
    static const std::string value =  _( "Used" );
    return value;
}

const std::string NCPkgStrings::FreeSpace()
{
    // column header free disk space (keep it short!)
    static const std::string value =  _( "Free" );
    return value;
}

const std::string NCPkgStrings::TotalSpace()
{
    // column header total disk space (keep it short!)
    static const std::string value =  _( "Total" );
    return value;
}

const std::string NCPkgStrings::DiskSpaceError()
{
    //
    static const std::string value = _( "<i>Out of disk space!</i>" );
    return value;
}

const std::string NCPkgStrings::DiskSpaceWarning()
{
    //
    static const std::string value = _( "<b>Disk space is running out!</b>" );
    return value;
}

const std::string NCPkgStrings::MoreText()
{
    // part of a text
    static const std::string value = _( "needs" );
    return value;
}

const std::string NCPkgStrings::MoreSpaceText()
{
    // part of a text
    static const std::string value = _( "more disk space." );
    return value;
}

const std::string NCPkgStrings::Version()
{
    // part of the package description
    static const std::string value = _( "<b>Version: </b>" );
    return value;
}

const std::string NCPkgStrings::Size()
{
    // part of the package description
    static const std::string value = _( "<b>Size: </b>" );
    return value;
}

const std::string NCPkgStrings::InstVersion()
{
    // part of the package description
    static const std::string value = _( "<b>Installed: </b>" );
    return value;
}

const std::string NCPkgStrings::Authors()
{
    // part of the package description
    static const std::string value = _( "<b>Authors: </b>" );
    return value;
}

const std::string NCPkgStrings::License()
{
    // part of the package description
    static const std::string value = _( "<b>License: </b>" );
    return value;
}

const std::string NCPkgStrings::MediaNo()
{
    // part of the package description
    static const std::string value = _( "<b>Media No.: </b>" );
    return value;
}

const std::string NCPkgStrings::RpmGroup()
{
    // part of the package description
    static const std::string value = _( "<b>Package Group: </b>" );
    return value;
}

const std::string NCPkgStrings::Provides()
{
    // part of the package description
    static const std::string value = _( "<b>Provides: </b>" );
    return value;
}

const std::string NCPkgStrings::Requires()
{
    // part of the package description
    static const std::string value = _( "<b>Requires: </b>" );
    return value;
}

const std::string NCPkgStrings::PreRequires()
{
    // part of the package description
    static const std::string value = _( "<b>Prerequires: </b>" );
    return value;

}

const std::string NCPkgStrings::Conflicts()
{
    // part of the package description
    static const std::string value = _( "<b>Conflicts with: </b>" );
    return value;
}

const std::string NCPkgStrings::ListOfFiles()
{
    // headline for a list of installed files
    static const std::string value =  _( "<i>List of Installed Files:</i><br>" );
    return value;
}

const std::string NCPkgStrings::LanguageDescription()
{
    static const std::string value =  _( "Translations, dictionaries and other language-related files for " );
    return value;
}

const std::string NCPkgStrings::AutoChangeLabel()
{
    // the headline of the popup containing a list with packages with status changes
    static const std::string value = _( "Automatic Changes" );
    return value;
}

const std::string NCPkgStrings::AutoChangeText1()
{
    // text part1 of popup with automatic changes (it's a label; text continous)
    static const std::string value = _( "In addition to your manual selections, the following" );
    return value;
}

const std::string NCPkgStrings::AutoChangeText2()
{
    // text part1 of popup with automatic changes continous
    static const std::string value = _( "packages have been changed to resolve dependencies:" );
    return value;
}

const std::string NCPkgStrings::YouHelp()
{
    // the headline of the help popup
    static const std::string value = _( "Patch Status and Patch Installation" );
    return value;
}

const std::string NCPkgStrings::YouHelp1()
{
    // help text online udpate
    // Do NOT translate 'recommended' and 'security'! because the patch kind is always shown as english text.
    static const std::string value =  _( "<p>General information about patches:</p><p>The patches of kind <b>security</b> are solving security issues and we highly recommend to install it. You should also install <b>recommended</b> patches, they usually contain important bug-fixes. Install <b>feature</b> patches if you are interested in the feature.</p>" );
    return value;
}

const std::string NCPkgStrings::YouHelp2()
{
    // help text online udpate continue
    static const std::string value =  _( "<p>Patches for \"libzypp\" (Package, Patch, Pattern and Product Management) will always get installed first. Other patches must be installed on a second run.</p>" );
    return value;
}

const std::string NCPkgStrings::YouHelp3()
{
    // help text online udpate continue
    static const std::string value = _( "<p>Meaning of the status flags:</p><p><b>a+ </b>: Patches concerning your installation are preselected. They will be downloaded and installed on your system. If you do not want a certain patch, deselect it with '-'.</p><p><b> i </b>: All requirements of this patch are satisfied.</p><p><b> + </b>: You have selected this patch for installation.</p>" );
    return value;
}

const std::string NCPkgStrings::YouHelp4()
{
 // help text online udpate continue
    static const std::string value = _( "<p>More details about the status:<br>If there are several patches for a package (or a set of packages) which aren't yet applied to the system all got preselected and have status <b>a+</b>. If one of these patches is deselected with '-' it might show the status <b>i</b> afterwards. This is because any of the other patches concerning the same package(s) is still selected. The newer version(s) of the package(s) will be installed and with it this patch is satisfied. Deselecting of all patches is required if the patches are not wanted.</p>");
    return value;
}

const std::string NCPkgStrings::YouHelp5()
{
 // help text online udpate continue
    static const std::string value = _( "<p>The menus:</p><p>The <b>Filter</b> menu allows to filter the patches, e.g. show the 'Installed' ones or list 'Security' patches. It also provides to search for patches.<br>Use the <b>Actions</b> menu to change the status of a patch.<br>The <b>View</b> menu offers the possibility to see which packages are concerned by the patch. Please note: If the filter is 'All Patches' the package list for some patches might be empty. This means no packages are concerned because none of the patch packages is installed on the system.<br>The <b>Dependencies</b> menu contains dependencies checks and the 'Generate Solver Testcase' entry.</p>");
    return value;
}
//
// label, text for translation
//
const std::string NCPkgStrings::WarningLabel()
{
    // label for a warning popup
    static const std::string value = _( "Warning" );
    return value;
}

const std::string NCPkgStrings::ErrorLabel()
{
     // label for an error popup
    static const std::string value = _( "Error" );
    return value;
}

const std::string NCPkgStrings::NotifyLabel()
{
    // label for a notify popup
    static const std::string value = _( "Notify" );
    return value;
}


const std::string NCPkgStrings::OKLabel()
{
    // the label of an OK button
    static const std::string value = _( "&OK" );
    return value;
}

const std::string NCPkgStrings::ContinueLabel()
{
    // begin: the label of the Contiunue button
    static const std::string value = _( "C&ontinue" );
    return value;
}

const std::string NCPkgStrings::CancelLabel()
{
    // begin: the label of the Cancel button
    static const std::string value = _( "&Cancel" );
    return value;
}

const std::string NCPkgStrings::YesLabel()
{
    // the label of the Yes button
    static const std::string value = _( "&Yes" );
    return value;
}

const std::string NCPkgStrings::AcceptLabel()
{
    // the label of an Accept button
    static const std::string value = _( "&Accept" );
    return value;
}

const std::string NCPkgStrings::NoLabel()
{
    // the label of the No button
    static const std::string value = _( "&No" );
    return value;
}

const std::string NCPkgStrings::SolveLabel()
{
    // the label of the Solve button - 'try again' implies that user
    // has to make some action (#213602)
    static const std::string value = _( "&OK -- Try Again" );
    return value;
}


const std::string NCPkgStrings::CancelText()
{
    // text for a Notify popup
    static const std::string value =  _( "<p>All changes in the package, patch or pattern selection will be lost.<br>Really exit?</p>" );
    return value;
}

//FIXME: remove these if possible
const std::string NCPkgStrings::LanguageLabel()
{
   // the label of language table
    static const std::string value = _( "Available Languages" );
    return value;
}

const std::string NCPkgStrings::RepoLabel()
{
   // the label of language table
    static const std::string value = _( "Available Repositories" );
    return value;
}

const std::string NCPkgStrings::YOUPatches()
{
    // A label for a list of YOU Patches - keep it short - max 25 chars!
    // (the list shows all patches which are needed)
    //static const std::string value = _( "Installable Patches" );
    //static const std::string value = _( "Relevant Patches" );
    static const std::string value = _( "Needed Patches" );
    return value;
}

const std::string NCPkgStrings::InstPatches()
{
    // A label for a list of YOU Patches - keep it short - max. 25 chars!
    // (the list shows all patches which are already installed)
    static const std::string value = _( "Installed Patches" );
    //static const std::string value = _( "Satisfied Patches" );
    return value;
}

const std::string NCPkgStrings::Patches()
{
    // A common label for a list of YOU Patches - keep it short - max. 25 chars!
    static const std::string value = _( "Online Update Patches" );
    return value;
}

const std::string NCPkgStrings::UpdateProblem()
{
   // the label Filter: Update Problem ( keep it short - max. 25 chars )
    static const std::string value = _( "Update Problem -- see help" );
    return value;
}

const std::string NCPkgStrings::SearchResults()
{
    // the label for Filter: Search results
    static const std::string value = _( "Search Results" );
    return value;
}

const std::string NCPkgStrings::PackageDeps()
{
    // the headline of the dependency popup
    static const std::string value = _( "Package Dependencies" );
    return value;
}

const std::string NCPkgStrings::HelpOnUpdate()
{
    // help text package status
    static const std::string value = _( "<b>Update Problem List</b><br><p>The packages in the list cannot be updated automatically.</p><p>Possible reasons:</p><p>They are obsoleted by other packages.</p><p>There is no newer version to which to update on any installation media.</p><p>They are third-party packages</p><p>Manually select what to do with them. The safest course of action is to delete them.</p>" );
    return value;
}

const std::string NCPkgStrings::PkgSource()
{
    // column header source RPM installation (keep it short!)
    static const std::string value = _( "Source" );
    return value;
}

#if 0
const std::string NCPkgStrings::MenuEntryUpdateList()
{
    // menu entry Update List
    static const std::string value = _( "&Update List" );
    return value;
}
#endif
const std::string NCPkgStrings::Patch()
{
    // part of the patch description
    static const std::string value = _( "<b>Patch: </b>" );
    return value;
}

const std::string NCPkgStrings::NoPatches()
{
    // info line is shown if YOU patch list is empty
    static const std::string value = _( "No patches available" );
    return value;
}

const std::string NCPkgStrings::Script()
{
    static const std::string value = _( "Script" );
    return value;
}

const std::string NCPkgStrings::MultiversionHead()
{
    static const std::string value = _( "Incompatible Package Versions" );
    return value;
}

const std::string NCPkgStrings::MultiversionIntro()
{
    static const std::string value =
      _( "<p>You are trying to install multiversion-capable "
	 "and non-multiversion-capable versions of this "
	 "package at the same time.</p>" );
    return value;
}

const std::string NCPkgStrings::MultiversionText()
{
    static const std::string value =
      _( "<p>This version is multiversion-capable.</p>"
	 "<p>Press \"Continue\" to install this version "
	 "and unselect the non-multiversion-capable version, "
	 "\"Cancel\" to unselect this version and keep the other one.</p>" );
    return value;
}

const std::string NCPkgStrings::NotMultiversionText()
{
    static const std::string value =
      _( "<p>This version is not multiversion-capable.</p>"
	 "<p>Press \"Continue\" to install only this version "
	 "and unselect all other versions, "
	 "\"Cancel\" to unselect this version and keep the other ones.<p>" );
    return value;
}

