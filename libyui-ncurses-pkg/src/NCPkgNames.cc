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

   File:       NCPkgNames.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <ycp/YCPValue.h>
#include <ycp/YCPTerm.h>
#include <ycp/YCPString.h>

#include "NCPkgNames.h"

#include "NCi18n.h"

#include <libintl.h>


/*
  Textdomain "packages-ncurses"
*/

//
// Creates the id_name (with ` at the beginning), e.g. `id(`id_name)
//
YCPValue NCPkgNames::createIdValue ( const std::string &name )
{
    YCPTerm id( name );
    return YCPString (id->name()); 
}

//
// All `id s used in pkg_layout.ycp
//

// the package table ( list of packages )
const YCPValue& NCPkgNames::Packages ()
{
    static const YCPValue value = createIdValue ( "packages" );
    return value;
}

// the list of available packages
const YCPValue& NCPkgNames::AvailPkgs ()
{
    static const YCPValue value = createIdValue ( "availpkgs" );
    return value;
}

// the list of patch packages
const YCPValue& NCPkgNames::PatchPkgs ()
{
    static const YCPValue value = createIdValue ( "patchpkgs" );
    return value;
}

const YCPValue& NCPkgNames::PatchPkgsVersions ()
{
    static const YCPValue value = createIdValue ( "pkgsversions" );
    return value;
}

// the description section ( shown on bottom of the selection dialog )
const YCPValue& NCPkgNames::Description ()
{
    static const YCPValue value = createIdValue ( "description" );
    return value;
}

// the file list information ( shown on bottom of the selection dialog )
const YCPValue& NCPkgNames::FilelistId ()
{
    static const YCPValue value = createIdValue ( "filelist" );
    return value;
}

// widget id of the ReplacePoint package information
const YCPValue& NCPkgNames::ReplPoint ()
{
    static const YCPValue value = createIdValue ( "replaceinfo" );
    return value;
}

// the menu entry Action/Toggle
const YCPValue& NCPkgNames::Toggle ()
{
    static const YCPValue value = createIdValue ( "toggle" );
    return value;
}

// the menu entry Action/Select
const YCPValue& NCPkgNames::Select ()
{
    static const YCPValue value = createIdValue ( "select" );
    return value;
}

// the menu entry Action/delete
const YCPValue& NCPkgNames::Delete ()
{
    static const YCPValue value = createIdValue ( "delete" );
    return value;
}

// the menu entry Action/update
const YCPValue& NCPkgNames::Update ()
{
    static const YCPValue value = createIdValue ( "update" );
    return value;
}

// the menu entry Action/update if newer version available 
const YCPValue& NCPkgNames::UpdateNewer ()
{
    static const YCPValue value = createIdValue ( "updnewer" );
    return value;
}

// the menu entry Action/update all
const YCPValue& NCPkgNames::UpdateAll ()
{
    static const YCPValue value = createIdValue ( "updall" );
    return value;
}

// the menu entry Action/do not update all
const YCPValue& NCPkgNames::DontUpdate ()
{
    static const YCPValue value = createIdValue ( "dontupd" );
    return value;
}

// the menu entry Action/select all
const YCPValue& NCPkgNames::InstallAll ()
{
    static const YCPValue value = createIdValue ( "selall" );
    return value;
}
// the menu entry Action/deselect all
const YCPValue& NCPkgNames::DontInstall  ()
{
    static const YCPValue value = createIdValue ( "deselall" );
    return value;
}

// the menu entry Action/delete all
const YCPValue& NCPkgNames::DeleteAll ()
{
    static const YCPValue value = createIdValue ( "delall" );
    return value;
}

// the menu entry Action/delete all
const YCPValue& NCPkgNames::DontDelete ()
{
    static const YCPValue value = createIdValue ( "dontdel" );
    return value;
}
// the menu entry Action/Taboo On
const YCPValue& NCPkgNames::TabooOn ()
{
    static const YCPValue value = createIdValue ( "tabooOn" );
    return value;
}

// the menu entry Action/Taboo Off
const YCPValue& NCPkgNames::TabooOff ()
{
    static const YCPValue value = createIdValue ( "tabooOff" );
    return value;
}

// the menu entry Action/SPM yes/no
const YCPValue& NCPkgNames::SourceYes ()
{
    static const YCPValue value = createIdValue ( "sourceYes" );
    return value;
}
// the menu entry Action/SPM yes/no
const YCPValue& NCPkgNames::SourceNo ()
{
    static const YCPValue value = createIdValue ( "sourceNo" );
    return value;
}

// the menu entry Etc./check dep
const YCPValue& NCPkgNames::ShowDeps ()
{
    static const YCPValue value = createIdValue ( "showdeps" );
    return value;
}
// the menu entry Etc./verify system
const YCPValue& NCPkgNames::VerifySystem ()
{
    static const YCPValue value = createIdValue ( "verifysystem" );
    return value;
}
// the menu entry Etc./auto check dep
const YCPValue& NCPkgNames::AutoDeps ()
{
    static const YCPValue value = createIdValue ( "autodeps" );
    return value;
}

// the menu entry Etc./Package list/Export to file
const YCPValue& NCPkgNames::ExportToFile () 
{
	static const YCPValue value = createIdValue ( "export" );
        return value;
}

// the menu entry Etc./Package list/Import from file
const YCPValue& NCPkgNames::ImportFromFile () 
{
	static const YCPValue value = createIdValue ("import");
	return value;
}

// the menu entry Etc./Save selection
const YCPValue& NCPkgNames::SaveSel  ()
{
    static const YCPValue value = createIdValue ( "save" );
    return value;
}

// the menu entry Etc./Load
const YCPValue& NCPkgNames::LoadSel ()
{
    static const YCPValue value = createIdValue ( "load" );
    return value;
}

// internal use
const YCPValue& NCPkgNames::ReplaceMenu ()
{
    static const YCPValue value = createIdValue ( "replacemenu" );
    return value;
}
// internal use
const YCPValue& NCPkgNames::ReplaceFilter ()
{
    static const YCPValue value = createIdValue ( "replacefilter" );
    return value;
}

// the search button
const YCPValue& NCPkgNames::Search ()
{
    static const YCPValue value = createIdValue ( "search" );
    return value;
}

// menu selection RPM groups
const YCPValue& NCPkgNames::RpmGroups ()
{
    static const YCPValue value = createIdValue ( "groups" );
    return value;
}

// menu selection Selections
const YCPValue& NCPkgNames::Selections ()
{
    static const YCPValue value = createIdValue ( "selections" );
    return value;
}

// menu selection Patterns
const YCPValue& NCPkgNames::Patterns ()
{
    static const YCPValue value = createIdValue ( "patterns" );
    return value;
}

// menu selection Languages
const YCPValue& NCPkgNames::Languages ()
{
    static const YCPValue value = createIdValue ( "languages" );
    return value;
}

const YCPValue& NCPkgNames::Repositories () 
{
    static const YCPValue value = createIdValue ( "repos");
    return value;
}

// menu selection Installation Summary
const YCPValue& NCPkgNames::Whatif ()
{
    static const YCPValue value = createIdValue ( "whatif" );
    return value;
}

// menu selection Installed Packages
const YCPValue& NCPkgNames::Installed ()
{
    static const YCPValue value = createIdValue ( "installed" );
    return value;
}

// menu selection update list
const YCPValue& NCPkgNames::UpdateList ()
{
    static const YCPValue value = createIdValue ( "updatelist" );
    return value;
}

// menu selection long description 
const YCPValue& NCPkgNames::LongDescr ()
{
    static const YCPValue value = createIdValue ( "longdescr" );
    return value;
}

// menu selection all package versions 
const YCPValue& NCPkgNames::Versions ()
{
    static const YCPValue value = createIdValue ( "versions" );
    return value;
}

// menu selection file list 
const YCPValue& NCPkgNames::Files ()
{
    static const YCPValue value = createIdValue ( "files" );
    return value;
}

// menu selection relations 
const YCPValue& NCPkgNames::Relations ()
{
    static const YCPValue value = createIdValue ( "relations" );
    return value;
}

// menu selection help/update 
const YCPValue& NCPkgNames::UpdateHelp ()
{
    static const YCPValue value = createIdValue ( "help_update" );
    return value;
}

// menu selection help/search 
const YCPValue& NCPkgNames::SearchHelp ()
{
    static const YCPValue value = createIdValue ( "help_search" );
    return value;
}

// menu selection package info
const YCPValue& NCPkgNames::PkgInfo ()
{
    static const YCPValue value = createIdValue ( "pkginfo" );
    return value;
}

//  id of the label which shows the selected filter
const YCPValue& NCPkgNames::Filter ()
{
    static const YCPValue value = createIdValue ( "filter" );
    return value;
}

// the label for disk space
const YCPValue& NCPkgNames::Diskspace ()
{
    static const YCPValue value = createIdValue ( "diskspace" );
    return value;
}

// the button disk space information
const YCPValue& NCPkgNames::Diskinfo ()
{
    static const YCPValue value = createIdValue ( "diskinfo" );
    return value;
}

// cancel button id
const YCPValue& NCPkgNames::Cancel ()
{
    static const YCPValue value = createIdValue ( "cancel" );
    return value;
}

// solve button id
const YCPValue& NCPkgNames::Solve ()
{
    static const YCPValue value = createIdValue ( "solve" );
    return value;
}

// ignore button id
const YCPValue& NCPkgNames::Ignore ()
{
    static const YCPValue value = createIdValue ( "ignore" );
    return value;
}

// ignore button id
const YCPValue& NCPkgNames::IgnoreAll ()
{
    static const YCPValue value = createIdValue ( "ignore_all" );
    return value;
}

// combo box id
const YCPValue& NCPkgNames::SearchBox ()
{
    static const YCPValue value = createIdValue ( "search_box" );
    return value;
}

// ok button id
const YCPValue& NCPkgNames::OkButton ()
{
    static const YCPValue value = createIdValue ( "ok" );
    return value;
}

// id general help
const YCPValue& NCPkgNames::GeneralHelp ()
{
    static const YCPValue value = createIdValue ( "help_general" );
    return value;
}

// id help on status
const YCPValue& NCPkgNames::StatusHelp ()
{
    static const YCPValue value = createIdValue ( "help_status" );
    return value;
}

// id help on filter
const YCPValue& NCPkgNames::FilterHelp ()
{
    static const YCPValue value = createIdValue ( "help_filter" );
    return value;
}


//
// id's used in you_layout.ycp
//

//  id help in YOU mode
const YCPValue& NCPkgNames::PatchHelp ()
{
    static const YCPValue value = createIdValue ( "youhelp" );
    return value;
}

// id filter/recommended
const YCPValue& NCPkgNames::Recommended ()
{
    static const YCPValue value = createIdValue ( "recommended" );
    return value;
}

// id filter/security
const YCPValue& NCPkgNames::Security ()
{
    static const YCPValue value = createIdValue ( "security" );
    return value;
}

// id filter installed
const YCPValue& NCPkgNames::InstalledPatches ()
{
    static const YCPValue value = createIdValue ( "instpatches" );
    return value;
}

// id filter installable
const YCPValue& NCPkgNames::InstallablePatches ()
{
    static const YCPValue value = createIdValue ( "installablepatches" );
    return value;
}


// id filter uninstalled
const YCPValue& NCPkgNames::NewPatches ()
{
    static const YCPValue value = createIdValue ( "newpatches" );
    return value;
}

// id filter yast2
const YCPValue& NCPkgNames::YaST2Patches ()
{
    static const YCPValue value = createIdValue ( "yast2" );
    return value;
}

// id filter optional
const YCPValue& NCPkgNames::Optional ()
{
    static const YCPValue value = createIdValue ( "optional" );
    return value;
}

// id filter all patches
const YCPValue& NCPkgNames::AllPatches ()
{
    static const YCPValue value = createIdValue ( "allpatches" );
    return value;
}

// id information patch descr
const YCPValue& NCPkgNames::PatchDescr ()
{
    static const YCPValue value = createIdValue ( "patchdescr" );
    return value;
}

// id information patch descr
const YCPValue& NCPkgNames::PatchPackages ()
{
    static const YCPValue value = createIdValue ( "patchpackages" );
    return value;
}

const YCPValue& NCPkgNames::PatchPackagesVersions()
{
    static const YCPValue value = createIdValue ( "packagesversions" );
    return value;  
}

// id information directory list
const YCPValue& NCPkgNames::DirList ()
{
    static const YCPValue value = createIdValue ( "dirlist" );
    return value;
}

// id information file list
const YCPValue& NCPkgNames::FileList ()
{
    static const YCPValue value = createIdValue ( "filelist" );
    return value;
}

// id information directory name
const YCPValue& NCPkgNames::DirName ()
{
    static const YCPValue value = createIdValue ( "dirname" );
    return value;
}

// id information details view
const YCPValue& NCPkgNames::Details ()
{
    static const YCPValue value = createIdValue ( "details" );
    return value;
}

// id testcase
const YCPValue& NCPkgNames::Testcase ()
{
    static const YCPValue value = createIdValue ( "testcase" );
    return value;
}

//
// internal use
//
const YCPValue& NCPkgNames::Treeitem ()
{
    static const YCPValue value = createIdValue ( "dummy" );
    return value;
}


//
// label, text for translation
//
const string NCPkgNames::WarningLabel()
{
    // label for a warning popup
    static const string value = _( "Warning" );
    return value;
}

const string NCPkgNames::ErrorLabel()
{
     // label for an error popup
    static const string value = _( "Error" );
    return value;
}

const string NCPkgNames::NotifyLabel()
{
    // label for a notify popup 
    static const string value = _( "Notify" );
    return value;
}

const string NCPkgNames::RpmTreeLabel()
{
    // the label of the RPM-group-tags tree widget
    static const string value = _( "RPM Groups" );
    return value;
}

const string NCPkgNames::SelectionLabel()
{
   // the label of the selections
    static const string value = _( "Package Categories" );
    return value;  
}

const string NCPkgNames::LanguageLabel()
{
   // the label of language table
    static const string value = _( "Available Languages" );
    return value;  
}

const string NCPkgNames::RepoLabel()
{
   // the label of language table
    static const string value = _( "Available Repositories" );
    return value;  
}

const string NCPkgNames::YOUPatches()
{
    // A label for a list of YOU Patches - keep it short - max 25 chars! 
    // (the list shows all patches which can be installed) 
    static const string value = _( "Installable Patches" );
    return value;  
}

const string NCPkgNames::InstPatches()
{
    // A label for a list of YOU Patches - keep it short - max. 25 chars! 
    // (the list shows all patches which are already installed) 
    static const string value = _( "Installed Patches" );
    return value; 
}

const string NCPkgNames::Patches()
{
    // A common label for a list of YOU Patches - keep it short - max. 25 chars! 
    static const string value = _( "Online Update Patches" );
    return value; 
}

const string NCPkgNames::UpdateProblem()
{
   // the label Filter: Update Problem ( keep it short - max. 25 chars )
    static const string value = _( "Update Problem -- see help" );
    return value;  
}

const string NCPkgNames::OKLabel()
{
    // the label of an OK button
    static const string value = _( "&OK" );
    return value;
}

const string NCPkgNames::YesLabel()
{
    // the label of the Yes button
    static const string value = _( "&Yes" );
    return value;
}

const string NCPkgNames::AcceptLabel()
{
    // the label of an Accept button
    static const string value = _( "&Accept" );
    return value;
}

const string NCPkgNames::SaveLabel()
{
    // the label of the Save button
    static const string value = _( "&Save" );
    return value;
}

const string NCPkgNames::LoadLabel()
{
    // the label of the Load button
    static const string value = _( "&Load" );
    return value;
}

const string NCPkgNames::CancelLabel()
{
    // Please note: use unique hot keys for the buttons from 'begin:' to 'end:'
    // begin: the label of the Cancel button
    static const string value = _( "&Cancel" );
    return value;
}

const string NCPkgNames::NoLabel()
{
    // the label of the No button
    static const string value = _( "&No" );
    return value;
}

const string NCPkgNames::IgnAllLabel()
{
    // the label of the Ingnore all button
    static const string value = _( "Ignore &All" );
    return value;
}

const string NCPkgNames::SolveLabel()
{
    // the label of the Solve button - 'try again' implies that user
    // has to make some action (#213602)
    static const string value = _( "&OK -- Try Again" );
    return value;
}

const string NCPkgNames::IgnLabel()
{
    // end: the label of a Ignore button
    static const string value = _( "&Ignore" );
    return value;
}

const string NCPkgNames::SearchResults()
{
    // the label for Filter: Search results 
    static const string value = _( "Search Results" );
    return value;
}

const string NCPkgNames::InstSummary()
{
    // the label for Filter: Installation summary
    static const string value = _( "Inst. Summary" );
    return value;
}

const string NCPkgNames::PackageSearch()
{
    // the headline of the search popup 
    static const string value = _( "Package Search" );
    return value;
}

const string NCPkgNames::DepsHelpLine()
{
    // a help line for the dependency popup
    static const string value = _( " [+] Select    [-] Delete    [>] Update " );
    return value;
}

const string NCPkgNames::PackageDeps()
{
    // the headline of the dependency popup 
    static const string value = _( "Package Dependencies" );
    return value;
}

const string NCPkgNames::SelectionDeps()
{
    // the headline of the popup showing dependencies between selections of packages 
    static const string value = _( "Selection Dependencies" );
    return value;
}

const string NCPkgNames::Solving()
{
    // a text for a small popup which is shown during package dependency checking
    static const string value = _( "Solving..." );
    return value;
}    
const string NCPkgNames::Saving()
{
    // a text for a small popup which is shown during writing package selection to a file
    static const string value = _( "Saving..." );
    return value;
} 
const string NCPkgNames::Loading()
{
    // a text for a samll popup which is shown during loading package selections from a file
    static const string value = _( "Loading..." );
    return value;
} 
const string NCPkgNames::SearchPhrase()
{
    // begin: text/labels for search popups (use unique hotkeys until the end:)
    // text for the package search popup 
    static const string value = _( "&Search Phrase" );
    return value;
}

const string NCPkgNames::SearchIn()
{
    // label of a frame in search popup (without hotkey)
    static const string value = _( " Search in " );
    return value;
}

const string NCPkgNames::CheckDescr()
{
    // label of a combo box  (search popup)
    static const string value = _( "&Description (time-consuming)" );
    return value;
}

const string NCPkgNames::CheckName()
{
    // label of a combo box  (search popup)
    static const string value = _( "&Name of the Package" );
    return value;
}

const string NCPkgNames::CheckSummary()
{
    // label of a combo box  (search popup)
    static const string value = _( "S&ummary" );
    return value;
}

const string NCPkgNames::CheckProvides()
{
    // label of a combo box  (search popup)
    static const string value = _( "&Provides" );
    return value;
}

const string NCPkgNames::CheckRequires()
{
    // label of a combo box  (search popup)
    static const string value = _( "&Requires" );
    return value;
}

const string NCPkgNames::IgnoreCase()
{
    // end: text/labels for search popups
    // label of a combo box  (search popup)
    static const string value = _( "&Ignore Case" );
    return value;
}

const string NCPkgNames::DiskspaceLabel()
{
    // the headline of the disk space popup
    static const string value = _( "Disk Usage Overview" );
    return value;
}

const string NCPkgNames::File()
{
    // additional file (included in a YOU patch)
    static const string value = _( "Additional File" );
    return value;  
}


const string NCPkgNames::PreScript()
{
    // a pre script (included in a YOU patch)
    static const string value = _( "Pre-Script" );
    return value;
}

const string NCPkgNames::PostScript()
{
    // a post script (included in a YOU patch)
    static const string value = _( "Post-Script" );
    return value;
}

const string NCPkgNames::Script()
{
    // a script (included in a YOU patch)
    static const string value = _( "Script" );
    return value;
}

const string NCPkgNames::MediumLabel()
{
    // label of a combo box (medium means floppy or hard disk)
    static const string value = _( "&Medium" );
    return value;
}

const string NCPkgNames::Harddisk()
{
    // combo box entry
    static const string value = _( "Hard Disk" );
    return value;
}

const string NCPkgNames::Floppy()
{
    // combo box entry
    static const string value = _( "Floppy" );
    return value;
}

const string NCPkgNames::PackageHelp()
{
    // the headline of the help popup 
    static const string value = _( "Package Installation Help" );
    return value;
}

const string NCPkgNames::SearchHeadline()
{
    // the headline of the popup showing help on search  
    static const string value = _( "Package Search Help" );
    return value;
}

const string NCPkgNames::YouHelp()
{
    // the headline of the help popup 
    static const string value = _( "Online Update Help" );
    return value;
}
const string NCPkgNames::LabelUnresolvable()
{
    // text describing package conflict (it's a label - keep it short)
    static const string value = _( "Unresolvable package conflict." );
    return value;   
}

const string NCPkgNames::LabelUnres()
{
    // text describing package conflict (it's a label - keep it short!)
    static const string value = _( "Solve the conflict by selecting or deselecting packages." );
    return value;  
}

const string NCPkgNames::LabelAlternative()
{
     // text describing package dependency (it's a label - keep it short)
    static const string value =  _( "Select one of the alternatives below." );
    return value;  
}

const string NCPkgNames::LabelConflict()
{
     // text describing a conflict (it's a label; text continous  )
    static const string value = _( "Solve the conflict by deleting (or deselecting)" );
    return value;
}

const string NCPkgNames::LabelPkgConflict()
{
      // text describing a conflict continous (package conflict)
    static const string value = _( "the unwanted package or packages." );
    return value;
}

const string NCPkgNames::LabelSelConflict()
{
      // text describing a conflict continous (selection conflict)
    static const string value = _( "the unwanted selection or selections." );
    return value;
}

const string NCPkgNames::LabelSelRequBy1()
{
    // text describing package does not work (it's a label; text contonous)
    static const string value = _( "The selection or selections below will not work" );
    return value;
}

const string NCPkgNames::LabelSelRequBy2()
{
    // text describing selection does not work continous
    static const string value = _( "without the selection to delete." );
    return value;
}

const string NCPkgNames::LabelPkgRequBy1()
{
    // text describing package does not work (it's a label; text continous )
    static const string value = _( "The package or packages below will not work" );
    return value;
}

const string NCPkgNames::LabelPkgRequBy2()
{
    // text describing package does not work continous
    static const string value = _( "without the package to delete." );
    return value;
}

const string NCPkgNames::LabelRequire()
{
    // text describing packages are not available (it's a label - keep it short!)
    static const string value = _( "The required libraries or packages are not installed." );
    return value;
}
const string NCPkgNames::LabelSelRequire()
{
    // text describing selections are not available (it's a label - keep it short!)
    static const string value = _( "The required selections are not installed." );
    return value;
}

const string NCPkgNames::FileName()
{
    // label of a text input field
    static const string value = _( "Enter the file name" );
    return value;
}

const string NCPkgNames::Version()
{
    // part of the package description
    static const string value = _( "<b>Version: </b>" );
    return value;
}

const string NCPkgNames::InstVersion()
{
    // part of the package description
    static const string value = _( "<b>Installed: </b>" );
    return value;
}

const string NCPkgNames::Authors()
{
    // part of the package description
    static const string value = _( "<b>Authors: </b>" );
    return value;
}

const string NCPkgNames::License()
{
    // part of the package description
    static const string value = _( "<b>License: </b>" );
    return value;
}

const string NCPkgNames::MediaNo()
{
    // part of the package description
    static const string value = _( "<b>Media No.: </b>" );
    return value;
}

const string NCPkgNames::Size()
{
    // part of the package description
    static const string value = _( "<b>Size: </b>" );
    return value;
}

const string NCPkgNames::RpmGroup()
{
    // part of the package description
    static const string value = _( "<b>Package Group: </b>" );
    return value;
}

const string NCPkgNames::Provides()
{
    // part of the package description
    static const string value = _( "<b>Provides: </b>" );
    return value;
}

const string NCPkgNames::Requires()
{
    // part of the package description
    static const string value = _( "<b>Requires: </b>" );
    return value;
}

const string NCPkgNames::Patch()
{
    // part of the patch description
    static const string value = _( "<b>Patch: </b>" );
    return value;
}

const string NCPkgNames::UpToDate()
{
    // info line is shown if YOU patch list is empty
    static const string value = _( "The system is up to date" );
    return value;
}

const string NCPkgNames::NoPatches()
{
    // info line is shown if YOU patch list is empty
    static const string value = _( "No patches available" );
    return value;
}

const string NCPkgNames::PreRequires()
{
    // part of the package description
    static const string value = _( "<b>Prerequires: </b>" );
    return value;
}

const string NCPkgNames::RequText()
{
    // text is shown in a column of a package list entry
    // e.g. | i | aaa_base | requires ..... |
    static const string value = _( "requires..." );
    return value;
}

const string NCPkgNames::RequConflictText()
{
    // text is shown in a column of a package list entry
    static const string value = _( "has unresolved requirements..." );
    return value;
}

const string NCPkgNames::ObsoleteText()
{
    // text is shown in a column of a package list entry
    static const string value = _( "obsoletes..." );
    return value;
}

const string NCPkgNames::RequByText()
{
    // text is shown in a column of a package list entry
    static const string value = _( "is required by..." );
    return value;
}

const string NCPkgNames::RequiredByText()
{
    // part of a text
    static const string value = _( "is required by" );
    return value;
}

const string NCPkgNames::NotAvailableText()
{
    // part of a text
    static const string value = _( "is not available" );
    return value;
}

const string NCPkgNames::MoreText()
{
    // part of a text
    static const string value = _( "needs" );
    return value;
}

const string NCPkgNames::TabooText()
{
    // part of a text (package 'aa' has Taboo status)
    static const string value = _( "is set to \"Taboo\" status" );
    return value;
}


const string NCPkgNames::MoreSpaceText()
{
    // part of a text
    static const string value = _( "more disk space." );
    return value;
}


const string NCPkgNames::NoAvailText()
{
     // text is shown in a line of a package list
    static const string value = _( "no package available" );
    return value;
}
const string NCPkgNames::NeedsText()
{
    // text is shown in a line of a package list 
    static const string value = _( "needs packages..." );
    return value;
}
const string NCPkgNames::ConflictText()
{
     // text is shown in a line of a package list
    static const string value = _( "conflicts with..." );
    return value;
}
const string NCPkgNames::ReinstallText()
{
     // text is shown in a line of a package list
    static const string value = _( "needs to be reinstalled" );
    return value;
}
const string NCPkgNames::NoConflictText()
{
     // text is shown in a line of a package list
    static const string value = _( "No conflicts or unresolved dependencies" );
    return value;
}

const string NCPkgNames::ContinueRequ()
{
     // label continues the part required by ...
    static const string value = _( "...the package or packages below" );
    return value;   
}

const string NCPkgNames::ContinueSelRequ()
{
     // label continues the part required by ...
    static const string value = _( "...the selection or selections below" );
    return value;   
}

const string NCPkgNames::Conflicts()
{
    // part of the package description
    static const string value = _( "<b>Conflicts with: </b>" );
    return value;
}

const string NCPkgNames::ShortDescr()
{
    // part of the package description
    static const string value = _( "<b>Description: </b>" );
    return value;
}

const string NCPkgNames::MenuFilter()
{
    // menu Filter
    static const string value = _( "&Filter" );
    return value;
}

const string NCPkgNames::MenuEntryRPMGroups()
{
    // menu entry RpmGroups
    static const string value = _( "RPM &Groups" );
    return value;
}

const string NCPkgNames::MenuEntryLanguages()
{
    // menu entry Languages
    static const string value = _( "&Languages" );
    return value;
}

const string NCPkgNames::MenuEntrySelections()
{
    // menu entry Selections
    static const string value = _( "S&elections" );
    return value;
}

const string NCPkgNames::MenuEntryRepos()
{
    // menu entry Repositories
    static const string value = _( "&Repositories" );
    return value;
}

const string NCPkgNames::MenuEntryPatterns()
{
    // menu entry Patterns
    static const string value = _( "Pa&tterns" );
    return value;
}

const string NCPkgNames::MenuEntrySearch()
{
    // menu entry Search
    static const string value = _( "&Search" );
    return value;
}

const string NCPkgNames::MenuEntryInstPkg()
{
    // menu entry Installed Packages
    static const string value = _( "Installed &Packages" );
    return value;
}

const string NCPkgNames::MenuEntryInstSummary()
{
    // menu entry Installed Packages
    static const string value = _( "&Installation Summary" );
    return value;
}

const string NCPkgNames::MenuEntryUpdateList()
{
    // menu entry Update List
    static const string value = _( "&Update List" );
    return value;
}

const string NCPkgNames::MenuEtc()
{
    // menu Etc.
    static const string value = _( "&Etc." );
    return value;
}

const string NCPkgNames::MenuDeps()
{
    // submenu: dependency
    static const string value = _( "&Dependencies" );
    return value;
}

const string NCPkgNames::MenuCheckDeps()
{
    // menu entry: do a dependency check 
    // Please note: preserve the whitespaces at the beginning
    static const string value = _( "    &Check Dependencies Now" );
    return value;
}

const string NCPkgNames::MenuNoAutoDeps()
{
    // menu entry: dependency check off
    static const string value = _( "[ ] &Automatic Dependency Check" );
    return value;
}

const string NCPkgNames::MenuAutoDeps()
{
    // menu entry: dependency check on
    static const string value = _( "[X] &Automatic Dependency Check" );
    return value;
}

const string  NCPkgNames::MenuVerifySystem()
{
    // menu entry: verify system
    // Please note: preserve the whitespaces at the beginning
    static const string value = _( "    &Verify System" );
    return value;
}

const string NCPkgNames::MenuTestCase()
{
    // menu entry: generate test case for solver
    static const string value = _( "Generate Dependency Resolver &Test Case" );
    return value;
}

const string NCPkgNames::MenuList()
{
    // submenu: selections
    static const string value = _( "All &Packages List" );
    return value;
}

const string NCPkgNames::MenuExportList()
{
    // menu entry
    static const string value = _( "&Export to File" );
    return value;
}

const string NCPkgNames::MenuImportList()
{
    // last menu entry of the Etc. menu
    static const string value = _( "&Import from File" );
    return value;
}

const string NCPkgNames::DiskSpaceError()
{
    // 
    static const string value = _( "<i>Out of disk space!</i>" );
    return value;
}

const string NCPkgNames::DiskSpaceWarning()
{
    // 
    static const string value = _( "<b>Disk space is running out!</b>" );
    return value;
}

const string NCPkgNames::AutoChangeLabel()
{
    // the headline of the popup containing a list with packages with status changes
    static const string value = _( "Automatic Changes" );
    return value;
}

const string NCPkgNames::AutoChangeText1()
{
    // text part1 of popup with automatic changes (it's a label; text continous) 
    static const string value = _( "In addition to your manual selections, the following" );
    return value;
}

const string NCPkgNames::AutoChangeText2()
{
    // text part1 of popup with automatic changes continous 
    static const string value = _( "packages have been changed to resolve dependencies:" );
    return value;
}

const string NCPkgNames::HelpPkgInst1()
{
    // part1 of help text package installation 
    static const string value = _( "<p>The package installation dialog offers the features to customize your SuSE Linux installation. You have the opportunity to make your own decision about the installation or removal of particular packages. For example, select additional categories for installation (see menu <i>Filter</i>) or deselect unwanted packages.</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst12()
{
    // part of help text package installation 
    static const string value = _( "<p>The list shows all packages belonging to the current filter with status information in the first column. To change the package status, see <i>Actions</i>.</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst13()
{
    // additional help text for post installation 
    static const string value = _( "<p>Be careful when deleting packages and always pay attention to <i>Warning</i> pop-ups.</p>" );
    return value;  
}

const string NCPkgNames::HelpPkgInst2()
{
    // part2 of help text package installation 
    static const string value =  _( "Description of the menus:<br><p><b>Filter:</b> the packages shown in the package list match the selected filter, such as an RPM group, a keyword, or a category of packages (like Development or Games).</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst3()
{
    // part3 of help text package installation 
    static const string value =  _( "<p><b>Actions:</b> provides the possibilities to change the status of the selected package (or all packages in the list), for example, to delete a package or select an additional package for installation. The status change can also be done directly by pressing the key specified in the menu item (for detailed information about the package status, see 'Help on package status').</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst4()
{
    // part4 of help text package installation
    static const string value =  _( "<p><b>Information:</b> you have the possibility to view different information about the selected package in the window on the bottom of the dialog, such as the long description, all available versions of the package, or the file list.</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst5()
{
    // part5 of help text package installation
    static const string value =  _( "<p><b>Etc.:</b> The menu item 'Dependencies' offers different settings for the dependency checking. Automatic dependency check means check after every change of the package status whether all requirements are fulfilled or whether conflicts occurred.</p>" );
    return value;
}

const string NCPkgNames::HelpPkgInst6()
{
    // part6 of help text package installation
    static const string value =  _( "<p>To save or load a package selection, choose 'Selections'.</p>" );
    return value;
}

const string NCPkgNames::HelpOnSearch()
{
    // help text package search
    static const string value = _( "<p>Enter a keyword for your package search. It is possible to enter only parts of a package name, for example, to search for all 3D packages with \"3d\".<br> If you are looking for a term in a package description, click the appropriate check box. Start the search with 'Enter'.</p>" );
    return value;
}

const string NCPkgNames::HelpOnStatus1()
{
    // part 1 of help text package status
    static const string value = _( "<p>The package status can be changed using the <i>Actions</i> menu or the keys specified in the menu items. For example, use '+' to install an additional package. The \"Taboo\" status means the package should not be installed or the installed version should always be kept.</p>" );
    return value;
}

const string NCPkgNames::HelpOnStatus2()
{
    // part 1 of help text package status
    static const string value = _( "<p>By default, the package dependencies are checked with every status change. You will be informed about package conflicts and additionally required packages will be selected (the menu item <i>Etc.</i>/<i>Dependencies</i> offers different settings). To resolve the conflict, select one of the offered solution and press 'OK -- Try Again'.</p>" );
    return value;
}

const string NCPkgNames::HelpOnStatus3()
{
    // part 2 of help text package status
    static const string value = _( "<p>You can also use <b>RET</b> or <b>SPACE</b> to toggle the package status." );
    return value;
}

const string NCPkgNames::HelpOnStatus4()
{
    // part 3 of help text package status
    static const string value = _( "<br>The meaning of the status flags:" );
    return value;
}

const string NCPkgNames::HelpOnStatus5()
{
    // help text package status
    static const string value =  _( "<p><b> + </b>: this package is selected for installation</p><p><b>a+ </b>: automatically selected for installation</p><p><b> > </b>: update this package</p><p><b>a> </b>: automatic update</p><p><b> i </b>: this package is installed</p><p><b> - </b>: the package will be deleted</p><p><b>---</b>: never install this package</p>" );
    return value;
}

const string NCPkgNames::HelpOnStatus6()
{
    // help text package status
    static const string value =  _( "<p><b>-i-</b>: the package is \"protected\" -- keep the installed version and never update or delete it.</p>" );
    return value;
}

const string NCPkgNames::HelpOnStatus7()
{
    // help text package status
    static const string value =  _( "<p>The <i>Actions</i> menu also provides the possibility to change the status for all packages in the list (select 'All Listed Packages').</p>" );
    return value;
}

const string NCPkgNames::HelpOnUpdate()
{
    // help text package status
    static const string value = _( "<b>Update Problem List</b><br><p>The packages in the list cannot be updated automatically.</p><p>Possible reasons:</p><p>They are obsoleted by other packages.</p><p>There is no newer version to which to update on any installation media.</p><p>They are third-party packages</p><p>Manually select what to do with them. The safest course of action is to delete them.</p>" );
    return value;
}

const string NCPkgNames::PkgName()
{
    // column header package name (keep it short!)
    static const string value = _( "Name" );
    return value;
}

const string NCPkgNames::PackageName()
{
    // column header (keep it short!)
    static const string value = _( "Package/Library" );
    return value;
}

const string NCPkgNames::SelectionName()
{
    // column header selection name (keep it short!)
    static const string value = _( "Selection" );
    return value;
}

const string NCPkgNames::PkgVersion()
{
    // column header installed package version (keep it short!)
    static const string value = _( "Version" );
    return value;
}

const string NCPkgNames::PkgInstSource()
{
    // column header - repository of the package (e.g. openSUSE 10.3)
    static const string value = _( "Repository" );
    return value;
}

const string NCPkgNames::PkgVersionNew()
{
    // column header available package version (keep it short - use abbreviation!)
    static const string value = _( "Avail. Vers." );
    return value;
}

const string NCPkgNames::PkgVersionInst()
{
    // column header installed package version (keep it short - use abbreviation!)
    static const string value = _( "Inst. Vers." );
    return value;
}

const string NCPkgNames::PkgSummary()
{
    // column header package description (keep it short!)
    static const string value = _( "Summary" );
    return value;  
}


const string NCPkgNames::PkgSize()
{
    // column header package size (keep it short!)
    static const string value = _( "Size" );
    return value;
}

const string NCPkgNames::PkgArch()
{
    // column header package architecture  (keep it short!)
    static const string value = _( "Architecture" );
    return value;
}

const string NCPkgNames::PkgSource()
{
    // column header source RPM installation (keep it short!)
    static const string value = _( "Source" );
    return value;
}

const string NCPkgNames::PatchKind()
{
    // column header patch kind (keep it short!)
    static const string value = _( "Kind" );
    return value;
}

const string NCPkgNames::DepsKind()
{
    // column header kind of the package dependency, e.g. conflict (keep it short!)
    static const string value = _( "Kind of Dependency" );
    return value;
}

const string NCPkgNames::Comment()
{
    // column header comment (keep it short!)
    static const string value = _( "Additional Information" );
    return value;
}

const string NCPkgNames::PkgStatus()
{
    // column header status
    static const string value =  string("     ");
    return value;
}

const string NCPkgNames::LangCode()
{
    static const string value = _("Code");
    return value;
}

const string NCPkgNames::LangName()
{
    static const string value = _("Language");
    return value;
}

const string NCPkgNames::RepoURL()
{
    static const string value = _("URL");
    return value;

}

const string NCPkgNames::Partition()
{
    // column header name of the partition (keep it short!)
    static const string value =  _( "Partition" );
    return value;
}

const string NCPkgNames::UsedSpace()
{
    // column header used disk space (keep it short!)
    static const string value =  _( "Used" );
    return value;
}

const string NCPkgNames::FreeSpace()
{
    // column header free disk space (keep it short!)
    static const string value =  _( "Free" );
    return value;
}

const string NCPkgNames::TotalSpace()
{
    // column header total disk space (keep it short!)
    static const string value =  _( "Total" );
    return value;
}

const string NCPkgNames::ListOfFiles()
{
    // headline for a list of installed files
    static const string value =  _( "<i>List of Installed Files:</i><br>" );
    return value;
}

const string NCPkgNames::YouHelp1()
{
    // help text online udpate
    // Do NOT translate 'recommended' and 'security'! because the patch kind is always shown as english text. 
    static const string value =  _( "<p>Mode \"recommended\" means you should install the patch. \"security\" is a security patch and it is highly recommended to install it. \"YaST2\" patches will be always installed first. The other patches must be installed on a second run.</p>" );
    return value;
}						  

const string NCPkgNames::YouHelp2()
{
    // help text online udpate continue
    static const string value = _( "<p>Meaning of the status flags:</p><p><b>+</b>: Patches concerning your installation are preselected. They will be downloaded and installed on your system. If you do not want a certain patch, deselect it with '-'.</p>" );
    return value;
}

const string NCPkgNames::YouHelp3()
{
    // help text online udpate continue
    static const string value =  _( "<p><b>i</b>: This patch is already installed.</p><p><b>></b>: The patch will be reinstalled.</p>" );
    return value;
}

const string NCPkgNames::TextmodeHelp1()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>Special keys:<br>Function keys provide a quick access to the main functions (frequently used buttons) of this dialog. See the listing below to get the bindings.</p>" );
    return value;
}

const string NCPkgNames::TextmodeHelp2()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>Shortcuts are used to perform the desired action by pressing the <b>Alt</b> key followed by the key marked in the corresponding button, check box, radio button, or other item.</p>If the <b>Alt</b> key combinations are captured by the X server, use <b>ESC</b> instead." );
    return value;
}

const string NCPkgNames::TextmodeHelp3()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>General keyboard navigation:</p>" );
    return value;
}

const string NCPkgNames::TextmodeHelp4()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>To move inside a dialog, use <b>Tab</b> to go forward and <b>Alt</b> or <b>Shift</b> + <b>Tab</b> to go back.<br> If the <b>Alt</b> and <b>Shift</b> key combinations are occupied by the window manager or the terminal, <b>Ctrl</b> + <b>F</b> (forward) and <b>Ctrl</b> + <b>B</b> (back) can be used instead.</p>." );
    return value;
}
const string NCPkgNames::TextmodeHelp5()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>The command specified on an activated button (e.g., Next, Add) is executed if the <b>Enter</b> key is pressed.</p> <p>Radio buttons or check boxes are switched on or off with <b>Enter</b> or <b>Space</b>. " );

   return value;
}
const string NCPkgNames::TextmodeHelp6()
{
    // part of help for text mode navigation
    static const string value =  _( "<p>Scroll in lists or text (e.g., the help text) with the <b>arrow</b> keys.</p>" );
    return value;
}

const string NCPkgNames::SaveSelHeadline()
{
    // headline for the popup
    static const string value =  _( "Save Package List" );
    return value;
}

const string NCPkgNames::SaveSelText()
{
    // text for thepopup "Save Package List"
    static const string value =  _( "<p>The current package selection can be saved on a floppy (insert a formatted floppy) or on the hard disk.<br>Select the medium and specify the file name in the input field below.</p>" );
    return value;
}

const string NCPkgNames::SaveErr1Text()
{
    // text for an Error popup
    static const string value =  _( "<p>Could not find a formatted floppy. Check that a formatted and verified floppy is inserted in the drive.</p>" );
    return value;
}

const string NCPkgNames::SaveErr2Text()
{
    // text for an Error popup
    static const string value =  _( "<p>Could not write to file.<br>Check that the path name is correct and the directory is accessible.</p>" );
    return value;
}

const string NCPkgNames::Saved1Text()
{
    // text for a Notify popup
    static const string value =  _( "<p>Your settings have been saved to the floppy disk.</p>" );
    return value;
}

const string NCPkgNames::Saved2Text()
{
    // text for a Notify popup
    static const string value =  _( "<p>Your package selection has been written to the hard disk.</p>" );
    return value;
}

const string NCPkgNames::LoadSelHeadline()
{
    // headline for the popup
    static const string value =  _( "Load Package List" );
    return value;  
}

const string NCPkgNames::LoadSel1Text()
{
    // text for a Notify popup
    static const string value =  _( "<p>Really overwrite your package selection?</p>" );
    return value;
}

const string NCPkgNames::LoadSel2Text()
{
    // text for popup "Load Package List"
    static const string value =  _( "<p>Specify the medium and the name of the file containing the user-defined package selection to install.<br>If required, insert the floppy.</p>" );
    return value;
}

const string NCPkgNames::LoadErr1Text()
{
    // text for an Error popup
    static const string value =  _( "<p>Could not mount the floppy. Check that the floppy is inserted in the drive.</p>" );
    return value;
}

const string NCPkgNames::LoadErr2Text()
{
    // text for an Error popup
    static const string value =  _( "<p>Could not load the settings.<br>Check that the path name is correct and the floppy disk, if required, is inserted correctly.</p>" );
    return value;
}

const string NCPkgNames::LoadedText()
{
    // text for a Notify popup
    static const string value =  _( "<p>The package selection is loaded succesfully.</p>" );
    return value;
}

const string NCPkgNames::CancelText()
{
    // text for a Notify popup
    static const string value =  _( "<p>All changes in package, patch or pattern selection will be lost.<br>Really exit?</p>" );
    return value;
}

///////////////////////////////////////////////////////////////////



