/*---------------------------------------------------------------------\ |                                                                      |
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
   Maintainer: Michael Andres <ma@suse.de>

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
    static const string Deps();

    //its subentries
    static const string AutoDeps();
    static const string NoAutoDeps();
    static const string CheckDeps();
    static const string VerifySystem();
    static const string TestCase();
 
    // Main menu entry View
    static const string View();

    //its subentries
    static const string PackageDescription();
    static const string TechnicalData();
    static const string PackageVersions();
    static const string FileList();
    static const string Dependencies();

    // Main menu entry 'Extras'
    static const string Extras();

    //its subentries
    static const string ExportList();	
    static const string ImportList();
    static const string ShowDiskSpace();

    //Package Filters combo box label
    static const string Filter();

    //the filters itself
    static const string Patterns();
    static const string Languages();
    static const string RPMGroups();
    static const string Repos();
    static const string Search();
    static const string InstSummary();
    static const string InstPkg();

    //Label below the table - pkg name follows
    static const string PackageName();

    //Actions menu (what to do with the pkg)
    static const string Actions();

    //the actions itself
    static const string Toggle();
    static const string Install();
    static const string Delete();
    static const string Update();
    static const string Taboo();
    static const string Lock();
    static const string AllPkgsList();
    static const string InstallAll();
    static const string DeleteAll();
    static const string KeepAll();
    static const string UpdateAll();
    static const string UpdateIfNewer();

    static const string Help();

    //  The headline of the help popup
    static const string PackageHelp();

    //strings in 'General Help'
    static const string HelpPkgGen1();
    static const string HelpPkgGen2();
    static const string HelpPkgGen3();
    static const string HelpPkgGen4();
    static const string HelpPkgGen5();
    static const string HelpPkgGen6();

    //  The headline of the help popup
    static const string PackageStatusHelp();

    //strings in 'Package Status and symbols'
    static const string HelpOnStatus1();
    static const string HelpOnStatus2();
    static const string HelpOnStatus3(); 
    static const string HelpOnStatus4();
    static const string HelpOnStatus5();

    //  The headline of the help popup
    static const string PackageFiltersHelp();

    //string in 'How to use filters'
    static const string HelpOnFilters1();
    static const string HelpOnFilters2();
    static const string HelpOnFilters3();
    static const string HelpOnFilters4();
    static const string HelpOnFilters5();

    //  The headline of the help popup
    static const string PackageMenuHelp();

    // strings in 'Useful functions in menu'
    static const string HelpPkgMenu1();
    static const string HelpPkgMenu2();
    static const string HelpPkgMenu3();
    static const string HelpPkgMenu4();


    //Search filter strings
    static const string SearchPhrase();
    static const string SearchButton();
    static const string IgnoreCase();

    //Search settings
    static const string SearchIn();
    static const string CheckName();
    static const string CheckSummary();
    static const string CheckDescr();
    static const string CheckProvides();
    static const string CheckRequires();
  
    // table column headlines 
    static const string PkgStatus();
    static const string PatternsLabel();
    static const string LangCode();
    static const string LangName();
    static const string RepoURL();
    static const string PkgName();
    static const string PkgSummary();
    static const string PkgVersion();
    static const string PkgInstSource();
    static const string PkgVersionNew();
    static const string PkgVersionInst();
    static const string PkgSize();
    static const string PkgArch();
    static const string PkgSource();
    static const string PatchKind();

    //Useful busy popups
    static const string Solving();
    static const string Saving();
    static const string Loading();

   /**
     *  The headline of the disk space popup
     */ 
    static const string DiskspaceLabel();

    //column headers, diskspace table
    static const string Partition();
    static const string UsedSpace();
    static const string FreeSpace();
    static const string TotalSpace();
   /**
     * The headline of the disk space popup
     */
    static const string DiskSpaceError();
    static const string DiskSpaceWarning();
    static const string MoreText();
    static const string MoreSpaceText();

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

    static const string ListOfFiles();
    static const string LanguageDescription();
   /**
     *  text used for automatic changes popup
     */
    static const string AutoChangeLabel();
    static const string AutoChangeText1();
    static const string AutoChangeText2();

     /**
     * The headline of the help YOU popup
     */
    static const string YouHelp();

    /**
     * help text package status
     */
    static const string YouHelp1();
    static const string YouHelp2();
    static const string YouHelp3();

        static const string WarningLabel();
    static const string ErrorLabel();
    static const string NotifyLabel();

    /**
     *  The label of the OK button
     */
    static const string OKLabel();

    /**
     *  The label of the Cancel button
     */
    static const string CancelLabel();

    /**
     *  The label of the Yes button
     */
    static const string YesLabel();

   /**
     *  The label of the Accept button
     */
    static const string AcceptLabel();
    
    /**
     *  The label of the No button
     */
    static const string NoLabel();
    
     /**
     *  The label of the Solve button
     */
    static const string SolveLabel();


    static const string CancelText();
    /**
     *  The label of the language selections popup
     */
    static const string LanguageLabel();

   /**
     *  The label of the repositories selections popup
     */
    static const string RepoLabel();

   /**
     *  The label Filter: YOU Patches
     */
    static const string YOUPatches();
    static const string InstPatches();
    static const string Patches();

   /**
     *  The label for Filter: Search results
     */
    static const string SearchResults();

    /**
     *  The label for Filter: Update problem
     */
    static const string UpdateProblem();
    
    /**
     * The headline of the dependency popup
     */
    static const string PackageDeps();

    static const string HelpOnUpdate();

    /**
     * bold text Patch: (richtext) 
     */
    static const string Patch();

    /**
     * Info line in empty patch list
     */
    static const string NoPatches();
    
    static const string MenuList();
    static const string Script();



private:
    
    // Need no object of this class: hide default constructor
    NCPkgStrings();

    static int do_something(){ return 0; }
    
};


///////////////////////////////////////////////////////////////////

#endif // NCPkgStrings_h



