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

// the menu entry Action/deselect
const YCPValue& PkgNames::Deselect ()
{
    static const YCPValue value = createIdValue ( "deselect" );
    return value;
}

// the menu entry Action/delete
const YCPValue& PkgNames::Delete ()
{
    static const YCPValue value = createIdValue ( "delete" );
    return value;
}

// the menu entry Action/replace
const YCPValue& PkgNames::Replace ()
{
    static const YCPValue value = createIdValue ( "replace" );
    return value;
}

// the menu entry Action/installed
const YCPValue& PkgNames::Installed()
{
    static const YCPValue value = createIdValue ( "installed" );
    return value;
}

// the menu entry Action/update
const YCPValue& PkgNames::Update ()
{
    static const YCPValue value = createIdValue ( "update" );
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

// the menu entry Action/SPM yes/no
const YCPValue& PkgNames::ShowDeps ()
{
    static const YCPValue value = createIdValue ( "showdeps" );
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

// dependency menu selection requires 
const YCPValue& PkgNames::RequRel ()
{
    static const YCPValue value = createIdValue ( "requires" );
    return value;
}
// dependency menu selection conflicts 
const YCPValue& PkgNames::ConflRel ()
{
    static const YCPValue value = createIdValue ( "conflicts" );
    return value;
}
// dependency menu selection alternatves 
const YCPValue& PkgNames::AlterRel ()
{
    static const YCPValue value = createIdValue ( "alternatives" );
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
    static const YCPValue value = createIdValue ( "help_patch" );
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

const YCPString PkgNames::RpmTreeLabel()
{
    // the label of the RPM-group-tags tree widget
    static const YCPString value = toYCPString ( _("RPM Groups") );
    return value;
}

const YCPString PkgNames::SelectionLabel()
{
   // the label of the selections
    static const YCPString value = toYCPString ( _("Package categories") );
    return value;  
}

const YCPString PkgNames::YOUPatches()
{
   // the label Filter: YOU Patches
    static const YCPString value = toYCPString ( _("YOU Patches") );
    return value;  
}

const YCPString PkgNames::OKLabel()
{
    // the label of an OK button
    static const YCPString value = toYCPString ( _("&OK") );
    return value;
}

const YCPString PkgNames::CancelLabel()
{
    // the label of a Cancel button
    static const YCPString value = toYCPString ( _("&Cancel") );
    return value;
}

const YCPString PkgNames::SolveLabel()
{
    // the label of a Cancel button
    static const YCPString value = toYCPString ( _("&Solve") );
    return value;
}

const YCPString PkgNames::SearchResults()
{
    // the label for Filter: Search results 
    static const YCPString value = toYCPString ( _("Search results") );
    return value;
}

const YCPString PkgNames::PackageSearch()
{
    // the headline of the search popup 
    static const YCPString value = toYCPString ( _("Package search") );
    return value;
}

const YCPString PkgNames::PackageDeps()
{
    // the headline of the dependency popup 
    static const YCPString value = toYCPString ( _("Package dependencies") );
    return value;
}

const YCPString PkgNames::SearchPhrase()
{
    // text for search popup 
    static const YCPString value = toYCPString ( _("&Search phrase") );
    return value;
}

const YCPString PkgNames::CheckDescr()
{
    // text for search popup 
    static const YCPString value = toYCPString ( _("&Check description") );
    return value;
}

const YCPString PkgNames::IgnoreCase()
{
    // text for search popup 
    static const YCPString value = toYCPString ( _("&Ignore case") );
    return value;
}

const YCPString PkgNames::RequiredBy()
{
    // text for menu selection (package dependencies)
    static const YCPString value = toYCPString ( _("&Required by") );
    return value;
}

const YCPString PkgNames::ConflictDeps()
{
    // text for  menu selection (package dependencies)
    static const YCPString value = toYCPString ( _("&Conflicts") );
    return value;
}

const YCPString PkgNames::Alternatives()
{
    // text for menu selection (package dependencies)
    static const YCPString value = toYCPString ( _("&Alternatives") );
    return value;
}

const YCPString PkgNames::PackageHelp()
{
    // the headline of the help popup 
    static const YCPString value = toYCPString ( _("Help package installation") );
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
    static const NCstring value =  _("<p><b>Actions:</b> provides the possibilities to change the status of the selected package, for example, to delete a package or select an additional package for installation. The status change can also be done directly by pressing the key specified in the menu item (for detailed information about the package status, see 'Help/Help on package status').</p>");
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
    static const NCstring value =  _("<p><b>Etc.:</b> offers different settings for the dependency checking. Automatic dependency check means check, after every change of the package status, whether all requirements are fulfilled or whether conflicts occurred. For more information, see 'Help/Help on dependencies').</p>");
    return value;
}

const YCPString PkgNames::HelpOnSearch()
{
    // help text package search
    static const YCPString value = toYCPString( _("<p>Enter a keyword for your package search. It is possible to enter only parts of a package name, for example, to search for all 3D packages with \"3d\".<br> If you are looking for a term in a package description, click the appropriate check box. Start the search with 'Enter'.</p>"));
    return value;
}

const NCstring PkgNames::HelpOnStatus1()
{
    // part 1 of help text package status
    static const YCPString value = toYCPString( _("<p>The package status can be changed using the <i>Actions</i> menu or the keys specified in the menu items. By default, the package dependencies are checked with every status change. You will be informed about package conflicts and additionally required packages will be selected (<i>Etc.</i> offers different settings).</p>"));
    return value;
}

const NCstring PkgNames::HelpOnStatus2()
{
    // part 2 of help text package status
    static const YCPString value = toYCPString( _("<p>There is also the possibility to use <b>F4</b> to toggle between the package status, <b>F3</b> to select or <b>F5</b> to delete a package.</p>") );
    return value;
}

const NCstring PkgNames::HelpOnStatus3()
{
    // part 3 of help text package status
    static const YCPString value = toYCPString( _("<p>The meaning of the status flags:</p>") );
    return value;
}

const NCstring PkgNames::HelpOnStatusNew()
{
    // help text package status (new installation)
    static const YCPString value = toYCPString( _("<p><b>X</b> : this package is selected for installation</p><p><b>_</b> : the package will not be installed</p><p><b>a</b> : automatically selected package (another selected package depends on it)</p>") );
    return value;
}

const NCstring PkgNames::HelpOnStatusPost()
{
    // help text package status (post installation)
    static const YCPString value = toYCPString( _("<p><b>i</b> : the package is installed<p><b>X</b> : the package is selected for installation</p><p><b>d</b> : the package will be deleted</p><p><b>a</b> : automatically selected for installation</p><p><b>u</b> : update this package</p>" ) );
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
    // column header installed package version (keep it short!)
    static const NCstring value = _("Avail. Version");
    return value;
}

const NCstring PkgNames::PkgVersionInst()
{
    // column header installed package version (keep it short!)
    static const NCstring value = _("Inst. Version");
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

///////////////////////////////////////////////////////////////////



