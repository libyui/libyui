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

   File:       PkgNames.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PkgNames_h
#define PkgNames_h

#include "NCstring.h"
#include <string>


// Forward declaration
class YCPValue;


// Class to make available globally used names in package selection dialog,
// such as widget ids, text which has to be translated and so on

class PkgNames
{
public:
    
    /**
     *  Widget id of the package table ( list of packages )
     */
    static const YCPValue& Packages ();

    /**
     *  Widget id of the list of available packages
     */
    static const YCPValue& AvailPkgs ();

    /**
     *  Widget id of the list of patch packages
     */
    static const YCPValue& PatchPkgs ();

    /**
     *	Widget id of the description section ( shown on bottom of the selection dialog )
     */
    static const YCPValue& Description ();

    /**
     *  Widget id of the file list information ( shown on bottom of the selection dialog )
     */
    static const YCPValue& FilelistId ();

    /**
     * Widget id of the ReplacePoint
     */
    static const YCPValue& ReplPoint();
    
    /**
     * Widget id of the menu item Action/Toggle
     */
    static const YCPValue& Toggle ();

    /**
     * Widget id of the menu item Action/Select
     */
    static const YCPValue& Select ();

    /**
     * Widget id of the menu item Action/Delete
     */
    static const YCPValue& Delete ();

    /**
     * Widget id of the menu item Action/Taboo On
     */
    static const YCPValue& TabooOn ();

   /**
     * Widget id of the menu item Action/Taboo Off 
     */
    static const YCPValue& TabooOff ();
    
   /**
     * Widget id of the menu item Action/SPM yes/no
     */
    static const YCPValue& ToggleSource ();
    
    /**
     * Widget id of the menu item Action/All packages/Delete all
     */
    static const YCPValue& DeleteAll ();

   /**
     * Widget id of the menu item Action/All packages/Delete all
     */
    static const YCPValue& DontDelete ();
    
    /**
     * Widget id of the menu item Action/All packages/Select all
     */
    static const YCPValue& InstallAll ();

    /**
     * Widget id of the menu item Action/All packages/Select all
     */
    static const YCPValue& DontInstall ();
    
    /**
     * Widget id of the menu item Action/All packages/Update all
     */
    static const YCPValue& UpdateAll();

    /**
     * Widget id of the menu item Action/All packages/Update all
     */
    static const YCPValue& DontUpdate();
    
    /**
     * Widget id of the menu item Action/Update
     */
    static const YCPValue& Update ();
    
    /**
     *  Widget id of the search button
     */
    static const YCPValue& Search ();

    /**
     *  Widget id of the menu item Information/Long description
     */
    static const YCPValue& LongDescr ();

    /**
     *  Widget id of the menu item Information/Versions
     */
    static const YCPValue& Versions ();
    
    /**
     *  Widget id of the menu item Information/Package info
     */
    static const YCPValue& PkgInfo ();

    /**
     *  Widget id of the menu item Information/File list
     */
    static const YCPValue& Files ();

   /**
     *  Widget id of the menu item Help/Udpate
     */
    static const YCPValue& UpdateHelp ();  

    /**
     *  Widget id of the menu item Help/Search
     */
    static const YCPValue& SearchHelp ();
    
    /**
     *  Widget id of the menu item Information/Relations
     */
    static const YCPValue& Relations ();
    
    /**
     *  Widget id of the menu item RPM groups
     */
    static const YCPValue& RpmGroups ();

   /**
     *  Widget id of the menu item What if ...
     */
    static const YCPValue& Whatif ();

   /**
    * Widget id of the menu item Installed Packages
    */
    static const YCPValue& Installed ();
    
    /**
     *  Widget id of the menu item Selections
     */
    static const YCPValue& Selections ();

    /**
     *  Widget id of the menu item update list
     */
    static const YCPValue& UpdateList ();

    /**
     *  Widget id of the label which shows the selected filter
     */
    static const YCPValue& Filter ();

    /**
     *  Widget id of the label for disk space information
     */
    static const YCPValue& Diskspace ();

   /**
     *  Widget id of the Etc./Dependencies/Check deps menu item
     */
    static const YCPValue& ShowDeps ();

   /**
     *  Widget id of the Etc./Dependencies/Auto check menu item
     */
    static const YCPValue& AutoDeps ();

    /**
     *  Widget id of the Etc./Selections menu item
     */
    static const YCPValue& SaveSel ();

   /**
     *  Widget id of the Etc./Selections check menu item
     */
    static const YCPValue& LoadSel ();

    /**
     *  Widget id for internal use
     */
    static const YCPValue& ReplaceMenu ();

    /**
     *  Widget id of the button disk space
     */
    static const YCPValue& Diskinfo ();

    /**
     *  Widget id of the cancel button
     */
    static const YCPValue& Cancel ();

    /**
     *  Widget id of the ok button
     */
    static const YCPValue& OkButton ();

    /**
     *  Widget id combo box
     */
    static const YCPValue& SearchBox ();
    
   /**
     *  Widget id of the solve button
     */
    static const YCPValue& Solve ();

   /**
     *  Widget id of the Ignore button
     */
    static const YCPValue& Ignore ();

    /**
     *  Widget id of the Ignore all button
     */
    static const YCPValue& IgnoreAll ();
    
    /**
     *  Widget id of general help menu item
     */
    static const YCPValue& GeneralHelp ();
    
   /**
     *  Widget id of status help menu item
     */
    static const YCPValue& StatusHelp ();

    /**
     *  Widget id of filter help menu item
     */
    static const YCPValue& FilterHelp ();

    /**
     *  Widget id of  help  YOU
     */
    static const YCPValue& PatchHelp ();
    
    /**
     *  Widget id of Filter/recommended
     */
    static const YCPValue& Recommended ();

   /**
     *  Widget id of Filter/security
     */
    static const YCPValue& Security ();

    /**
     *  Widget id of Filter/installed
     */
    static const YCPValue& InstalledPatches ();

    /**
     *  Widget id of Filter/all patches
     */
    static const YCPValue& AllPatches ();

   /**
     *  Widget id of Filter/new patches
     */
    static const YCPValue& NewPatches ();

   /**
     *  Widget id of Filter/YaST2 patches
     */
    static const YCPValue& YaST2Patches ();
    
   /**
     *  Widget id information patch descr
     */
    static const YCPValue& PatchDescr ();

    /**
     *  Widget id information patch packages
     */
    static const YCPValue& PatchPackages ();

    /**
     *  Internal use (id for the tree items)
     */
    static const YCPValue& Treeitem();

    /**
     *  The label of the RPM-group-tags tree widget
     */
    static const string RpmTreeLabel();

    /**
     *  The headline of the disk space popup
     */ 
    static const string DiskspaceLabel();

   /**
     *  The label of the package selections popup
     */
    static const string SelectionLabel();

   /**
     *  The label Filter: YOU Patches
     */
    static const string YOUPatches();
    
    /**
     *  The label for Filter: Search results
     */
    static const string SearchResults();

   /**
     *  The label for Filter: Installation Summary
     */
    static const string InstSummary();
    
    /**
     *  The label for Filter: Update problem
     */
    static const string UpdateProblem();
    
    /**
     * The headline of the search popup
     */
    static const string PackageSearch();

    /**
     * The headline of the dependency popup
     */
    static const string PackageDeps();
    static const string SelectionDeps();

   /**
     * The headline of the disk space popup
     */
    static const string DiskSpaceError();
    static const string DiskSpaceWarning();
    
    static const string LabelUnres();
    static const string LabelAlternative();
    static const string LabelConflict();
    static const string LabelSelConflict();
    static const string LabelRequBy();
    static const string LabelSelRequBy();
    static const string LabelRequire();
    static const string LabelSelRequire();
    static const string LabelUnresolvable();
    static const string WarningLabel();
    static const string ErrorLabel();
    static const string NotifyLabel();
    static const string DepsHelpLine();
    static const string Solving();
    static const string Saving();
    static const string Loading();
    static const string ContinueRequ();
    static const string ContinueSelRequ();
    
   /**
     * text used in search popup
     */
    static const string SearchPhrase();

   /**
     * text used in search popup
     */
    static const string CheckDescr();
    
   /**
     * text used in search popup
     */
    static const string IgnoreCase();

   /**
     * text used in search popup
     */
    static const string SearchIn();

   /**
     * text used in search popup
     */
    static const string CheckName();
    
   /**
     * text used in search popup
     */
    static const string CheckSummary();

   /**
     * text used in search popup
     */
    static const string CheckProvides();
    
   /**
     * text used in search popup
     */
    static const string CheckRequires();
    
    /**
     * The headline of the help popup
     */
    static const string PackageHelp();

    /**
     * The headline of the search help popup
     */
    static const string SearchHeadline();
    
    /**
     * The headline of the help YOU popup
     */
    static const string YouHelp();

    /**
     *  The label of the OK button
     */
    static const string OKLabel();

    /**
     *  The label of the Cancel button
     */
    static const string CancelLabel();
    
    /**
     *  The label of the Cancel/Ignore button
     */
    static const string CancelIgnore();

   /**
     *  The label of the Ignore All button
     */
    static const string IgnAllLabel();

    /**
     *  The label of the Ignore button
     */
    static const string IgnLabel();
    
    /**
     *  The label of the Solve button
     */
    static const string SolveLabel();

   /**
     *  The label of the Save button
     */
    static const string SaveLabel();

    /**
     *  The label of the Load button
     */
    static const string LoadLabel();
    
    /**
     * The label of text input field 
     */
    static const string FileName();

   /**
     * The label of the combo box medium 
     */
    static const string MediumLabel();

    /**
     * A combo box entry 
     */
    static const string Harddisk();

   /**
     * A combo box entry 
     */
    static const string Floppy();

    /**
     * Part 1 of help text package installation
     */
    static const string HelpPkgInst1();

    /**
     * Part 12 of help text package installation
     */
    static const string HelpPkgInst12();

    /**
     * Part 13 of help text package installation
     */
    static const string HelpPkgInst13();

    /**
     * help text package status
     */
    static const string HelpOnStatus1();
    static const string HelpOnStatus2();
    static const string HelpOnStatus3(); 
    static const string HelpOnStatus4();
    static const string HelpOnStatus5();
    static const string HelpOnStatus6();
    static const string HelpOnStatus7();

    static const string YouHelp1();
    static const string YouHelp2();
    static const string YouHelp3();

    static const string TextmodeHelp1();
    static const string TextmodeHelp2();
    static const string TextmodeHelp3();
    static const string TextmodeHelp4();
    static const string TextmodeHelp5();
    static const string TextmodeHelp6();
    static const string TextmodeHelp7();
    static const string TextmodeHelp8();
    static const string TextmodeHelp9();
    static const string TextmodeHelp();

   /**
     *  text used for automatic changes popup
     */
    static const string AutoChangeLabel();
    static const string AutoChangeText();

    /**
     * Part of help text used in installed system
     */
    static const string HelpPkgPostInst();

    /**
     * Part 2 of help text package installation
     */
    static const string HelpPkgInst2();
       /**
     * Part 3 of help text package installation
     */
    static const string HelpPkgInst3();
    /**
     * Part 4 of help text package installation
     */
    static const string HelpPkgInst4();

    /**
     * Part 5 of help text package installation
     */
    static const string HelpPkgInst5();

   /**
     * Part 6 of help text package installation
     */
    static const string HelpPkgInst6();
    
    static const string HelpOnUpdate();

    /**
     * help package search
     */
    static const string HelpOnSearch();

    /**
     * bold text Version: (richtext) 
     */
    static const string Version();

    /**
     * bold text Installed version: (richtext) 
     */
    static const string InstVersion();

    /**
     * bold text License: (richtext) 
     */
    static const string License();

    /**
     * bold text Media No.: (richtext) 
     */
    static const string MediaNo();
    
    /**
     * bold text Size: (richtext) 
     */
    static const string Size();
    
   /**
     * bold text Package Group: (richtext) 
     */
    static const string RpmGroup();

   /**
     * bold text Authors: (richtext) 
     */
    static const string Authors();

    /**
     * bold text Provides: (richtext) 
     */
    static const string Provides();

    /**
     * bold text Description: (richtext) 
     */
    static const string ShortDescr();
    
    /**
     * bold text Requires: (richtext) 
     */
    static const string Requires();

    /**
     * bold text Required by: (richtext) 
     */
    static const string PreRequires();

    /**
     * bold text Conflicts with: (richtext) 
     */
    static const string Conflicts();
    
    /**
     *  Column header package name
     */
    static const string PkgName();

    /**
     *  Column header package name
     */
    static const string PackageName();

    /**
     *  Column header selection name
     */
    static const string SelectionName();
    
     /**
     *  Column header package version installed
     */
    static const string PkgVersion();

    /**
     *  Column header package version new
     */
    static const string PkgVersionNew();

    /**
     *  Column header package version installed
     */
    static const string PkgVersionInst();
    
    /**
     *  Column header package summary
     */
    static const string PkgSummary();

    /**
     *  Column header package size
     */
    static const string PkgSize();

   /**
     *  Column header package source
     */
    static const string PkgSource();
    
     /**
     *  Column header patch kind
     */
    static const string PatchKind();

   /**
     *  Column header dependency kind
     */
    static const string DepsKind();

    /**
     *  Column header comment
     */
    static const string Comment();

    /**
     *  Column header package status
     */
    static const string PkgStatus();

    static const string Partition();
    static const string UsedSpace();
    static const string FreeSpace();
    static const string TotalSpace();

    static const string ListOfFiles();

   /**
     * dependency popup text
     */ 
    static const string RequText();
    static const string RequConflictText();
    static const string ObsoleteText();
    
    static const string RequByText();
    static const string RequiredByText();
    static const string NoAvailText();
    static const string NeedsText();
    static const string ConflictText();
    static const string NoConflictText();
    static const string NotAvailableText();

    static const string MenuEtc();
    static const string MenuDeps();
    static const string MenuCheckDeps();
    static const string MenuAutoDeps();
    static const string MenuNoAutoDeps();
    static const string MenuSel();
    static const string MenuSaveSel();	
    static const string MenuLoadSel();

    static const string MoreText();
    static const string MoreSpaceText();

    /**
     * save/load selections
     */
    static const string SaveErr1Text();
    static const string SaveErr2Text();
    static const string SaveSelHeadline();
    static const string SaveSelText();
    static const string Saved1Text();
    static const string Saved2Text();
    
    static const string LoadSelHeadline();
    static const string LoadSel1Text();
    static const string LoadSel2Text();
    static const string LoadErr1Text();
    static const string LoadErr2Text();
    static const string LoadedText();
    static const string CancelText();
    
private:
    
    // Need no object of this class: hide default constructor
    PkgNames();

    /**
     *  Create an id value with ` at the beginning
     */
    static YCPValue createIdValue ( const std::string &name );

};


///////////////////////////////////////////////////////////////////

#endif // PkgNames_h



