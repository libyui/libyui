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

   File:       PkgNames.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <ycp/YCPValue.h>
#include <ycp/YCPTerm.h>
#include <ycp/YCPString.h>

#include "PkgNames.h"

#include "NCi18n.h"

#include <libintl.h>


/*
  Textdomain "packages"
*/

//
// Creates the id_name (with ` at the beginning), e.g. `id(`id_name)
//
YCPValue PkgNames::createIdValue ( const std::string &name )
{
    YCPTerm id( name, true);
    return id->symbol(); 
}

//
// Converts a NCString into a YCPString
//
YCPString PkgNames::toYCPString ( const NCstring & str )
{
    return str.YCPstr();
}


//
// All `id s used in pkg_layout.ycp
//

// the package table ( list of packages )
const YCPValue& PkgNames::Packages ()
{
    static const YCPValue value = createIdValue ( "packages" );
    return value;
}

// the list of available packages
const YCPValue& PkgNames::AvailPkgs ()
{
    static const YCPValue value = createIdValue ( "availpkgs" );
    return value;
}

// the list of patch packages
const YCPValue& PkgNames::PatchPkgs ()
{
    static const YCPValue value = createIdValue ( "patchpkgs" );
    return value;
}

// the description section ( shown on bottom of the selection dialog )
const YCPValue& PkgNames::Description ()
{
    static const YCPValue value = createIdValue ( "description" );
    return value;
}

// the file list information ( shown on bottom of the selection dialog )
const YCPValue& PkgNames::FilelistId ()
{
    static const YCPValue value = createIdValue ( "filelist" );
    return value;
}

// widget id of the ReplacePoint package information
const YCPValue& PkgNames::ReplPoint ()
{
    static const YCPValue value = createIdValue ( "replaceinfo" );
    return value;
}

// the menu entry Action/Toggle
const YCPValue& PkgNames::Toggle ()
{
    static const YCPValue value = createIdValue ( "toggle" );
    return value;
}

// the menu entry Action/Select
const YCPValue& PkgNames::Select ()
{
    static const YCPValue value = createIdValue ( "select" );
    return value;
}

// the menu entry Action/delete
const YCPValue& PkgNames::Delete ()
{
    static const YCPValue value = createIdValue ( "delete" );
    return value;
}

// the menu entry Action/update
const YCPValue& PkgNames::Update ()
{
    static const YCPValue value = createIdValue ( "update" );
    return value;
}

// the menu entry Action/update all
const YCPValue& PkgNames::UpdateAll ()
{
    static const YCPValue value = createIdValue ( "updall" );
    return value;
}

// the menu entry Action/update all
const YCPValue& PkgNames::DontUpdate ()
{
    static const YCPValue value = createIdValue ( "dontupd" );
    return value;
}

// the menu entry Action/select all
const YCPValue& PkgNames::InstallAll ()
{
    static const YCPValue value = createIdValue ( "selall" );
    return value;
}
// the menu entry Action/deselect all
const YCPValue& PkgNames::DontInstall  ()
{
    static const YCPValue value = createIdValue ( "deselall" );
    return value;
}

// the menu entry Action/delete all
const YCPValue& PkgNames::DeleteAll ()
{
    static const YCPValue value = createIdValue ( "delall" );
    return value;
}

// the menu entry Action/delete all
const YCPValue& PkgNames::DontDelete ()
{
    static const YCPValue value = createIdValue ( "dontdel" );
    return value;
}
// the menu entry Action/Taboo On
const YCPValue& PkgNames::TabooOn ()
{
    static const YCPValue value = createIdValue ( "tabooOn" );
    return value;
}

// the menu entry Action/Taboo Off
const YCPValue& PkgNames::TabooOff ()
{
    static const YCPValue value = createIdValue ( "tabooOff" );
    return value;
}

// the menu entry Action/SPM yes/no
const YCPValue& PkgNames::ToggleSource ()
{
    static const YCPValue value = createIdValue ( "toggleSrc" );
    return value;
}

// the menu entry Etc./check dep
const YCPValue& PkgNames::ShowDeps ()
{
    static const YCPValue value = createIdValue ( "showdeps" );
    return value;
}

// the menu entry Etc./auto check dep
const YCPValue& PkgNames::AutoDeps ()
{
    static const YCPValue value = createIdValue ( "autodeps" );
    return value;
}

// internal use
const YCPValue& PkgNames::ReplaceMenu ()
{
    static const YCPValue value = createIdValue ( "replacemenu" );
    return value;
}

// the search button
const YCPValue& PkgNames::Search ()
{
    static const YCPValue value = createIdValue ( "search" );
    return value;
}

// menu selection RPM groups
const YCPValue& PkgNames::RpmGroups ()
{
    static const YCPValue value = createIdValue ( "groups" );
    return value;
}

// menu selection Selections
const YCPValue& PkgNames::Selections ()
{
    static const YCPValue value = createIdValue ( "selections" );
    return value;
}

// menu selection What if ...
const YCPValue& PkgNames::Whatif ()
{
    static const YCPValue value = createIdValue ( "whatif" );
    return value;
}

// menu selection update list
const YCPValue& PkgNames::UpdateList ()
{
    static const YCPValue value = createIdValue ( "updatelist" );
    return value;
}

// menu selection long description 
const YCPValue& PkgNames::LongDescr ()
{
    static const YCPValue value = createIdValue ( "longdescr" );
    return value;
}

// menu selection all package versions 
const YCPValue& PkgNames::Versions ()
{
    static const YCPValue value = createIdValue ( "versions" );
    return value;
}

// menu selection file list 
const YCPValue& PkgNames::Files ()
{
    static const YCPValue value = createIdValue ( "files" );
    return value;
}

// menu selection relations 
const YCPValue& PkgNames::Relations ()
{
    static const YCPValue value = createIdValue ( "relations" );
    return value;
}

// menu selection help/update 
const YCPValue& PkgNames::UpdateHelp ()
{
    static const YCPValue value = createIdValue ( "help_update" );
    return value;
}

// menu selection help/search 
const YCPValue& PkgNames::SearchHelp ()
{
    static const YCPValue value = createIdValue ( "help_search" );
    return value;
}

// menu selection package info
const YCPValue& PkgNames::PkgInfo ()
{
    static const YCPValue value = createIdValue ( "pkginfo" );
    return value;
}

//  id of the label which shows the selected filter
const YCPValue& PkgNames::Filter ()
{
    static const YCPValue value = createIdValue ( "filter" );
    return value;
}

// the label for disk space
const YCPValue& PkgNames::Diskspace ()
{
    static const YCPValue value = createIdValue ( "diskspace" );
    return value;
}

// the button disk space information
const YCPValue& PkgNames::Diskinfo ()
{
    static const YCPValue value = createIdValue ( "diskinfo" );
    return value;
}

// cancel button id
const YCPValue& PkgNames::Cancel ()
{
    static const YCPValue value = createIdValue ( "cancel" );
    return value;
}

// solve button id
const YCPValue& PkgNames::Solve ()
{
    static const YCPValue value = createIdValue ( "solve" );
    return value;
}

// combo box id
const YCPValue& PkgNames::SearchBox ()
{
    static const YCPValue value = createIdValue ( "search_box" );
    return value;
}

// ok button id
const YCPValue& PkgNames::OkButton ()
{
    static const YCPValue value = createIdValue ( "ok" );
    return value;
}

// id general help
const YCPValue& PkgNames::GeneralHelp ()
{
    static const YCPValue value = createIdValue ( "help_general" );
    return value;
}

// id help on status
const YCPValue& PkgNames::StatusHelp ()
{
    static const YCPValue value = createIdValue ( "help_status" );
    return value;
}

// id help on filter
const YCPValue& PkgNames::FilterHelp ()
{
    static const YCPValue value = createIdValue ( "help_filter" );
    return value;
}


//
// id's used in you_layout.ycp
//

//  id help in YOU mode
const YCPValue& PkgNames::PatchHelp ()
{
    static const YCPValue value = createIdValue ( "youhelp" );
    return value;
}

// id filter/recommended
const YCPValue& PkgNames::Recommended ()
{
    static const YCPValue value = createIdValue ( "recommended" );
    return value;
}

// id filter/security
const YCPValue& PkgNames::Security ()
{
    static const YCPValue value = createIdValue ( "security" );
    return value;
}

// id filter installed
const YCPValue& PkgNames::InstalledPatches ()
{
    static const YCPValue value = createIdValue ( "instpatches" );
    return value;
}

// id filter uninstalled
const YCPValue& PkgNames::NewPatches ()
{
    static const YCPValue value = createIdValue ( "newpatches" );
    return value;
}

// id filter yast2
const YCPValue& PkgNames::YaST2Patches ()
{
    static const YCPValue value = createIdValue ( "yast2" );
    return value;
}

// id filter all patches
const YCPValue& PkgNames::AllPatches ()
{
    static const YCPValue value = createIdValue ( "allpatches" );
    return value;
}

// id information patch descr
const YCPValue& PkgNames::PatchDescr ()
{
    static const YCPValue value = createIdValue ( "patchdescr" );
    return value;
}

/// id information patch descr
const YCPValue& PkgNames::PatchPackages ()
{
    static const YCPValue value = createIdValue ( "patchpackages" );
    return value;
}

//
// internal use
//
const YCPValue& PkgNames::Treeitem ()
{
    static const YCPValue value = createIdValue ( "dummy" );
    return value;
}


//
// label, text for translation
//
const NCstring PkgNames::WarningLabel()
{
    // label for a warning popup
    static const NCstring value = _("Warning");
    return value;
}

const NCstring PkgNames::ErrorLabel()
{
     // label for an error popup
    static const NCstring value = _("Error");
    return value;
}

const NCstring PkgNames::NotifyLabel()
{
    // label for a notify popup 
    static const NCstring value = _("Notify");
    return value;
}

const NCstring PkgNames::RpmTreeLabel()
{
    // the label of the RPM-group-tags tree widget
    static const NCstring value = _("RPM Groups");
    return value;
}

const NCstring PkgNames::SelectionLabel()
{
   // the label of the selections
    static const NCstring value = _("Package Categories");
    return value;  
}

const NCstring PkgNames::YOUPatches()
{
   // the label Filter: YOU Patches
    static const NCstring value = _("Online Update Patches");
    return value;  
}

const NCstring PkgNames::UpdateProblem()
{
   // the label Filter: Update Problem ( keep it short - max. 25 chars )
    static const NCstring value = _("Update Problem - see help");
    return value;  
}

const NCstring PkgNames::OKLabel()
{
    // the label of an OK button
    static const NCstring value = _("&OK");
    return value;
}

const NCstring PkgNames::CancelLabel()
{
    // the label of a Cancel button
    static const NCstring value = _("&Cancel");
    return value;
}

const NCstring PkgNames::CancelIgnore()
{
    // the label of a Cancel button
    static const NCstring value = _("&Cancel/Ignore");
    return value;
}

const NCstring PkgNames::IgnoreAll()
{
    // the label of a Ingnire all button
    static const NCstring value = _("Ignore &All");
    return value;
}

const NCstring PkgNames::SolveLabel()
{
    // the label of a Cancel button
    static const NCstring value = _("&Solve");
    return value;
}

const NCstring PkgNames::Ignore()
{
    // the label of a Ignore button
    static const NCstring value = _("&Ignore");
    return value;
}

const NCstring PkgNames::SearchResults()
{
    // the label for Filter: Search results 
    static const NCstring value = _("Search Results");
    return value;
}

const NCstring PkgNames::InstSummary()
{
    // the label for Filter: Installation summary
    static const NCstring value = _("Inst. Summary");
    return value;
}

const NCstring PkgNames::PackageSearch()
{
    // the headline of the search popup 
    static const NCstring value = _("Package Search");
    return value;
}

const NCstring PkgNames::DepsHelpLine()
{
    // a help line for the dependency popup
    static const NCstring value = _( " [+] Select    [-] Delete    [>] Update " );
    return value;
}

const NCstring PkgNames::PackageDeps()
{
    // the headline of the dependency popup 
    static const NCstring value = _("Package Dependencies");
    return value;
}

const NCstring PkgNames::SelectionDeps()
{
    // the headline of the popup showing dependencies between selections of packages 
    static const NCstring value = _("Selection Dependencies");
    return value;
}

const NCstring PkgNames::Solving()
{
    // a label for a popup which is shown during package dependency checking
    static const NCstring value = _("Solving ...");
    return value;
}    
	
const NCstring PkgNames::SearchPhrase()
{
    // begin: text/labels for search popups (use unique hotkeys until the end:)
    // text for the package search popup 
    static const NCstring value = _("&Search phrase");
    return value;
}

const NCstring PkgNames::SearchIn()
{
    // label of a frame in search popup (without hotkey)
    static const NCstring value = _(" Search in ");
    return value;
}

const NCstring PkgNames::CheckDescr()
{
    // label of a combo box  (search popup)
    static const NCstring value = _("&Description (time-consuming!)");
    return value;
}

const NCstring PkgNames::CheckName()
{
    // label of a combo box  (search popup)
    static const NCstring value = _("&Name of the package");
    return value;
}

const NCstring PkgNames::CheckSummary()
{
    // label of a combo box  (search popup)
    static const NCstring value = _("S&ummary");
    return value;
}

const NCstring PkgNames::CheckProvides()
{
    // label of a combo box  (search popup)
    static const NCstring value = _("&Provides");
    return value;
}

const NCstring PkgNames::CheckRequires()
{
    // label of a combo box  (search popup)
    static const NCstring value = _("&Requires");
    return value;
}

const NCstring PkgNames::IgnoreCase()
{
    // end: text/labels for search popups
    // label of a combo box  (search popup)
    static const NCstring value = _("&Ignore case");
    return value;
}

const NCstring PkgNames::DiskspaceLabel()
{
    // the headline of the disk space popup
    static const NCstring value = _("&Disk Usage Overview");
    return value;
}

const NCstring PkgNames::PackageHelp()
{
    // the headline of the help popup 
    static const NCstring value = _("Help Package Installation");
    return value;
}

const NCstring PkgNames::SearchHeadline()
{
    // the headline of the popup showing help on search  
    static const NCstring value = _("Help Package Search");
    return value;
}

const NCstring PkgNames::YouHelp()
{
    // the headline of the help popup 
    static const NCstring value = _("Help Online Update");
    return value;
}
const NCstring PkgNames::LabelUnresolvable()
{
    // text part describing package conflict (it's a label - keep it short)
    static const NCstring value = _("Unresolvable package conflict.");
    return value;   
}

const NCstring PkgNames::LabelUnres1()
{
    // text part 1 describing package conflict (it's a label - keep it short)
    static const NCstring value = _("Solve the conflict by selecting");
    return value;  
}

const NCstring PkgNames::LabelUnres2()
{
    // text part 2 describing package conflict (it's a label - keep it short) 
    static const NCstring value =  _("or deselecting packages.");
    return value;
}

const NCstring PkgNames::LabelAlternative()
{
     // text part describing package dependency (it's a label - keep it short)
    static const NCstring value =  _("Select one of the alternatives below.");
    return value;  
}

const NCstring PkgNames::LabelConflict1()
{
     // text part 1 describing package conflict (it's a label - keep it short)
    static const NCstring value = _( "Solve the conflict by deleting (or");
    return value;
}

const NCstring PkgNames::LabelConflict2()
{
      // text part 2 describing package conflict (it's a label - keep it short)
    static const NCstring value = _( "deselecting) the unwanted package(s)." );
    return value;
}

const NCstring PkgNames::LabelSelConflict2()
{
      // text part 2 describing package conflict (it's a label - keep it short)
    static const NCstring value = _( "deselecting) the unwanted selection(s)." );
    return value;
}

const NCstring PkgNames::LabelRequBy1()
{
    // text part 1 describing package does not work (it's a label - keep it short)
    static const NCstring value = _( "The packages below will not work without" );
    return value;
}

const NCstring PkgNames::LabelSelRequBy1()
{
    // text part 1 describing package does not work (it's a label - keep it short)
    static const NCstring value = _( "The selection(s) below will not work without" );
    return value;
}

const NCstring PkgNames::LabelRequBy2()
{
    // text part 2 describing packages does not work (it's a label - keep it short)
    static const NCstring value = _( "the package you want to delete." );
    return value;
}

const NCstring PkgNames::LabelSelRequBy2()
{
    // text part 2 describing packages does not work (it's a label - keep it short)
    static const NCstring value = _( "the selection you want to delete." );
    return value;
}

const NCstring PkgNames::LabelRequire1()
{
    // text part 1 describing packages are not available (it's a label - keep it short)
    static const NCstring value = _( "The required libraries or packages" );
    return value;
}
const NCstring PkgNames::LabelSelRequire1()
{
    // text part 1 describing selections are not available (it's a label - keep it short)
    static const NCstring value = _( "The required selections" );
    return value;
}

const NCstring PkgNames::LabelRequire2()
{
    // text part 2 describing packages are not available (it's a label - keep it short)
    static const NCstring value =  _( "are not installed." );
    return value;
}
const NCstring PkgNames::FileList()
{
    // label of the frame around the file list
    static const NCstring value = _("File list");
    return value;
}

const NCstring PkgNames::Version()
{
    // part of the package description
    static const NCstring value = _("<b>Version: </b>");
    return value;
}

const NCstring PkgNames::InstVersion()
{
    // part of the package description
    static const NCstring value = _("<b>Installed: </b>");
    return value;
}

const NCstring PkgNames::Authors()
{
    // part of the package description
    static const NCstring value = _("<b>Authors: </b>");
    return value;
}

const NCstring PkgNames::License()
{
    // part of the package description
    static const NCstring value = _("<b>License: </b>");
    return value;
}

const NCstring PkgNames::MediaNo()
{
    // part of the package description
    static const NCstring value = _("<b>Media No.: </b>");
    return value;
}

const NCstring PkgNames::Size()
{
    // part of the package description
    static const NCstring value = _("<b>Size: </b>");
    return value;
}

const NCstring PkgNames::Provides()
{
    // part of the package description
    static const NCstring value = _("<b>Provides: </b>");
    return value;
}

const NCstring PkgNames::Requires()
{
    // part of the package description
    static const NCstring value = _("<b>Requires: </b>");
    return value;
}

const NCstring PkgNames::PreRequires()
{
    // part of the package description
    static const NCstring value = _("<b>Prerequires: </b>");
    return value;
}

const NCstring PkgNames::RequText()
{
    // text is shown in a column of a package list entry
    // e.g. | i | aaa_base | requires ..... |
    static const NCstring value = _("requires ...");
    return value;
}
const NCstring PkgNames::RequByText()
{
    // text is shown in a column of a package list entry
    static const NCstring value = _("is required by ...");
    return value;
}
const NCstring PkgNames::UnresText()
{
     // text is shown in a  column of a package list entry
    static const NCstring value = _("unresolvable ...");
    return value;
}
const NCstring PkgNames::NoAvailText()
{
     // text is shown in a line of a package list
    static const NCstring value = _("no package available");
    return value;
}
const NCstring PkgNames::NeedsText()
{
    // text is shown in a line of a package list 
    static const NCstring value = _("needs packages ...");
    return value;
}
const NCstring PkgNames::ConflictText()
{
     // text is shown in a line of a package list
    static const NCstring value = _("conflicts with ...");
    return value;
}
const NCstring PkgNames::NoConflictText()
{
     // text is shown in a line of a package list
    static const NCstring value = _("No conflicts or unresolved dependencies");
    return value;
}

const NCstring PkgNames::ContinueRequ()
{
     // label continues the part required by ...
    static const NCstring value = _("... the package(s) below");
    return value;   
}

const NCstring PkgNames::ContinueSelRequ()
{
     // label continues the part required by ...
    static const NCstring value = _("... the selection(s) below");
    return value;   
}

const NCstring PkgNames::Conflicts()
{
    // part of the package description
    static const NCstring value = _("<b>Conflicts with: </b>");
    return value;
}

const NCstring PkgNames::ShortDescr()
{
    // part of the package description
    static const NCstring value = _("<b>Description: </b>");
    return value;
}

const NCstring PkgNames::MenuEtc()
{
    // menu Etc.
    static const NCstring value = _("&Etc.");
    return value;
}

const NCstring PkgNames::MenuDeps()
{
    // submenu: dependency
    static const NCstring value = _("&Dependencies");
    return value;
}

const NCstring PkgNames::MenuCheckDeps()
{
    // menu entry: do a dependency check 
    // Please note: preserve the whitespaces at the beginning
    static const NCstring value = _("    &Check dependencies now");
    return value;
}

const NCstring PkgNames::MenuNoAutoDeps()
{
    // menu entry: dependency check off
    static const NCstring value = _("[ ] &Automatic dependency check");
    return value;
}

const NCstring PkgNames::MenuAutoDeps()
{
    // menu entry: dependency check on
    static const NCstring value = _("[X] &Automatic dependency check");
    return value;
}

const NCstring PkgNames::MenuSel()
{
    // submenu: selections
    static const NCstring value = _("&Selections");
    return value;
}

const NCstring PkgNames::MenuSaveSel()
{
    // menu entry
    static const NCstring value = _("S&ave selection");
    return value;
}

const NCstring PkgNames::MenuLoadSel()
{
    // last menu entry of the Etc. menu
    static const NCstring value = _("&Load selection");
    return value;
}

const NCstring PkgNames::HelpPkgInst1()
{
    // part1 of help text package installation 
    static const NCstring value = _("<p>The package installation dialog offers the features to customize your SuSE Linux installation. You have the opportunity to make your own decision about the installation or removal of particular packages. For example, select additional categories for installation (see menu <i>Filter</i>) or deselect unwanted packages.</p>" );
    return value;
}

const NCstring PkgNames::HelpPkgInst12()
{
    // part of help text package installation 
    static const NCstring value = _("<p>The list shows all packages belonging to the current filter with a status information in the first column. To change the package status, see menu <i>Actions</i>.</p>");
    return value;
}

const NCstring PkgNames::HelpPkgInst13()
{
    // additional help text for post installation 
    static const NCstring value = _("<p>Be careful when deleting packages and always pay attention to <i>Warning</i> popups.</p>");
    return value;  
}

const NCstring PkgNames::HelpPkgInst2()
{
    // part2 of help text package installation 
    static const NCstring value =  _("Description of the menues:<br><p><b>Filter:</b> the packages shown in the package list match the selected filter, such as an RPM group, a keyword, or a category of packages (like Development or Games).</p>");
    return value;
}

const NCstring PkgNames::HelpPkgInst3()
{
    // part3 of help text package installation 
    static const NCstring value =  _("<p><b>Actions:</b> provides the possibilities to change the status of the selected package (or all packages in the list), for example, to delete a package or select an additional package for installation. The status change can also be done directly by pressing the key specified in the menu item (for detailed information about the package status, see 'Help/Help on package status').</p>");
    return value;
}

const NCstring PkgNames::HelpPkgInst4()
{
    // part4 of help text package installation
    static const NCstring value =  _("<p><b>Information:</b> you have the possibility to view different information about the selected package in the window on the bottom of the dialog, such as the long description, all available versions of the package, or the file list.</p>");
    return value;
}

const NCstring PkgNames::HelpPkgInst5()
{
    // part5 of help text package installation
    static const NCstring value =  _("<p><b>Etc.:</b> The menu item 'Dependencies' offers different settings for the dependency checking. Automatic dependency check means check, after every change of the package status, whether all requirements are fulfilled or whether conflicts occurred.</p>");
    return value;
}

const NCstring PkgNames::HelpPkgInst6()
{
    // part6 of help text package installation
    static const NCstring value =  _("<p>If you want to save or load a package selections chose menu item 'Selections'.</p>");
    return value;
}

const NCstring PkgNames::HelpOnSearch()
{
    // help text package search
    static const NCstring value = _("<p>Enter a keyword for your package search. It is possible to enter only parts of a package name, for example, to search for all 3D packages with \"3d\".<br> If you are looking for a term in a package description, click the appropriate check box. Start the search with 'Enter'.</p>");
    return value;
}

const NCstring PkgNames::HelpOnStatus1()
{
    // part 1 of help text package status
    static const NCstring value = _("<p>The package status can be changed using the <i>Actions</i> menu or the keys specified in the menu items. By default, the package dependencies are checked with every status change. You will be informed about package conflicts and additionally required packages will be selected (<i>Etc.</i> offers different settings).</p>");
    return value;
}

const NCstring PkgNames::HelpOnStatus2()
{
    // part 2 of help text package status
    static const NCstring value = _("<p>You can also use <b>RET</b> or <b>SPACE</b> to toggle between the package status.");
    return value;
}

const NCstring PkgNames::HelpOnStatus3()
{
    // part 3 of help text package status
    static const NCstring value = _("<br>The meaning of the status flags:");
    return value;
}

const NCstring PkgNames::HelpOnStatus4()
{
    // help text package status
    static const NCstring value =  _("<p><b> + </b> : this package is selected for installation</p><p><b>a+ </b> : automatically selected for installation</p><p><b> > </b> : update this package</p><p><b>a> </b> : automatic update</p><p><b> i </b> : this package is installed</p><p><b> - </b> : the package will be deleted</p><p><b>---</b> : never install this package</p>");
    return value;
}

const NCstring PkgNames::HelpOnStatus5()
{
    // help text package status
    static const NCstring value =  _("<p>The <i>Actions</i> menu also provides the possibilty to change the status for all packages in the list (select menu item 'All list packages').</p>");
    return value;
}

const NCstring PkgNames::HelpOnUpdate()
{
    // help text package status
    static const NCstring value = _("<b>Update Problem list</b><br><p>The packages in the list cannot be updated automatically.</p><p>Possible reasons:</p><p>They are obsoleted by other packages.</p><p>There is no newer version to update to on any installation media.</p><p>They are third-party packages</p><p>Please choose manually what to do with them.The safest course of action is to delete them.</p>");
    return value;
}

const NCstring PkgNames::PkgName()
{
    // column header package name (keep it short!)
    static const NCstring value = _("Name");
    return value;
}

const NCstring PkgNames::PkgVersion()
{
    // column header installed package version (keep it short!)
    static const NCstring value = _("Version");
    return value;
}

const NCstring PkgNames::PkgVersionNew()
{
    // column header available package version (keep it short - use abbreviation!)
    static const NCstring value = _("Avail. Vers.");
    return value;
}

const NCstring PkgNames::PkgVersionInst()
{
    // column header installed package version (keep it short - use abbreviation!)
    static const NCstring value = _("Inst. Vers.");
    return value;
}

const NCstring PkgNames::PkgSummary()
{
    // column header package description (keep it short!)
    static const NCstring value = _("Summary");
    return value;  
}


const NCstring PkgNames::PkgSize()
{
    // column header package size (keep it short!)
    static const NCstring value = _("Size");
    return value;
}

const NCstring PkgNames::PatchKind()
{
    // column header patch kind (keep it short!)
    static const NCstring value = _("Kind");
    return value;
}

const NCstring PkgNames::PkgStatus()
{
    // column header status
    static const NCstring value =  NCstring("     ");
    return value;
}

const NCstring PkgNames::Partition()
{
    // column header name of the partition (keep it short!)
    static const NCstring value =  _("Partition");
    return value;
}

const NCstring PkgNames::UsedSpace()
{
    // column header used disk space (keep it short!)
    static const NCstring value =  _("Used");
    return value;
}

const NCstring PkgNames::FreeSpace()
{
    // column header free disk space (keep it short!)
    static const NCstring value =  _("Free");
    return value;
}

const NCstring PkgNames::TotalSpace()
{
    // column header total disk space (keep it short!)
    static const NCstring value =  _("Total");
    return value;
}

const NCstring PkgNames::YouHelp1()
{
    // help text online udpate
    // Do NOT translate 'recommended' and 'security'! because the patch kind is always shown as english text. 
    static const NCstring value =  _("<p>Mode \"recommended\" means you should install the patch. \"security\" is a security patch and it is highly recommended to install it. \"YaST2\" patches will be always installed first. The other patches must be installed on a second run.</p>");
    return value;
}						  

const NCstring PkgNames::YouHelp2()
{
    // help text online udpate continue
    static const NCstring value = _("<p>Meaning of the status flags:</p><p><b>+</b>: Patches concerning your installation are preselected. They will be downloaded and installed on your system. If you do not want a certain patch, deselect it with '-'.</p>");
    return value;
}

const NCstring PkgNames::YouHelp3()
{
    // help text online udpate continue
    static const NCstring value =  _("<p><b>i</b>: This patch is already installed.</p><p><b>></b>: The patch will be reinstalled.</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp()
{
    // headline of the text mode help
    static const NCstring value =  _("Text mode navigation"); 
    return value;
}

const NCstring PkgNames::TextmodeHelp1()
{
    // part of help for textmode navigation (shown if there is further help available) 
    static const NCstring value =  _("<p>Press <b>F1</b> again to get further help or <b>ESC</b> to close this dialog.</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp2()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Press <b>F1</b> or <b>ESC</b> to close this dialog.</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp3()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Function key bindings:</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp4()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Special keys:<br>Function keys provide a quick access to the main functions (frequently used buttons) of this dialog. See listing below to get the bindings.</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp5()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Shortcuts allow to do the desired action by pressing the <b>Alt</b> key followed by the key which is marked at the corresponding pushbutton, checkbox, radiobutton and so on.</p>If the <b>Alt</b> key combinations are set by the X server use <b>ESC</b> instead.");
    return value;
}

const NCstring PkgNames::TextmodeHelp6()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>General keyboard navigation:</p>");
    return value;
}

const NCstring PkgNames::TextmodeHelp7()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Movement inside a dialog is done with <b>Tab</b> to go forward and <b>Alt</b> or <b>Shift</b> <b>Tab</b> to go back.<br> If the <b>Alt</b> and <b>Shift</b> key combinations are occupied by the windowmanager or the terminal, <b>Ctrl</b> <b>f</b> (forward) and <b>Ctrl</b> <b>b</b> (back) can be used instead.</p>.");
    return value;
}
const NCstring PkgNames::TextmodeHelp8()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>The command specified on an activated button (e.g. go Next, Add someting) is executed if the <b>Enter</b> key is pressed.</p> <p>Radio buttons or check boxes are switched on or off with <b>Enter</b> or <b>Space</b> key. ");

   return value;
}
const NCstring PkgNames::TextmodeHelp9()
{
    // part of help for text mode navigation
    static const NCstring value =  _("<p>Scrolling in lists or text (e.g. the help text) is done with the <b>Arrow</b> keys.</p>");
    return value;
}


///////////////////////////////////////////////////////////////////



