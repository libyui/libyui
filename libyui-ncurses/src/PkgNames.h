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
     * Widget id of the menu selection Action/Toggle
     */
    static const YCPValue& Toggle ();

    /**
     * Widget id of the menu selection Action/Select
     */
    static const YCPValue& Select ();

    /**
     * Widget id of the menu selection Action/Deselect
     */
    static const YCPValue& Deselect ();

    /**
     * Widget id of the menu selection Action/Delete
     */
    static const YCPValue& Delete ();

    /**
     * Widget id of the menu selection Action/Taboo On
     */
    static const YCPValue& TabooOn ();

   /**
     * Widget id of the menu selection Action/Taboo =ff 
     */
    static const YCPValue& TabooOff ();
    
   /**
     * Widget id of the menu selection Action/SPM yes/no
     */
    static const YCPValue& ToggleSource ();
    
    /**
     * Widget id of the menu selection Action/Replace
     */
    static const YCPValue& Replace ();

    /**
     * Widget id of the menu selection Action/Installed
     */
    static const YCPValue& Installed();
    /**
     * Widget id of the menu selection Action/Update
     */
    static const YCPValue& Update ();
    
    /**
     *  Widget id of the search button
     */
    static const YCPValue& Search ();

    /**
     *  Widget id of the menu selection Information/Long description
     */
    static const YCPValue& LongDescr ();

    /**
     *  Widget id of the menu selection Information/Versions
     */
    static const YCPValue& Versions ();
    
    /**
     *  Widget id of the menu selection Information/Package info
     */
    static const YCPValue& PkgInfo ();

    /**
     *  Widget id of the menu selection Information/File list
     */
    static const YCPValue& Files ();

   /**
     *  Widget id of the menu selection Help/Udpate
     */
    static const YCPValue& PkgNames::UpdateHelp ();  

    /**
     *  Widget id of the menu selection Information/Relaione
     */
    static const YCPValue& PkgNames::Relations ();
    
    /**
     *  Widget id of the menu selection RPM groups
     */
    static const YCPValue& RpmGroups ();

    /**
     *  Widget id of the menu selection Selections
     */
    static const YCPValue& Selections ();

    /**
     *  Widget id of the label which shows the selected filter
     */
    static const YCPValue& Filter ();

    /**
     *  Widget id of the label for disk space information
     */
    static const YCPValue& Diskspace ();

    static const YCPValue& ShowDeps ();
    static const YCPValue& AutoDeps ();
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
     *  Widget id of the solve button
     */
    static const YCPValue& Solve ();
    
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
    static const YCPString RpmTreeLabel();

    /**
     *  The label of the package selections popup
     */
    static const YCPString SelectionLabel();

   /**
     *  The label Filter: YOU Patches
     */
    static const YCPString YOUPatches();
    
    /**
     *  The label for Filter: Search results
     */
    static const YCPString SearchResults();

    /**
     *  The label for Filter: Update problem
     */
    static const YCPString UpdateProblem();
    
    /**
     * The headline of the search popup
     */
    static const YCPString PackageSearch();

    /**
     * The headline of the dependency popup
     */
    static const YCPString PackageDeps();

    static const YCPString LabelUnres1();
    static const YCPString LabelUnres2();
    static const YCPString LabelAlternative();
    static const YCPString LabelConflict1();
    static const YCPString LabelConflict2();
    static const YCPString LabelRequBy1();
    static const YCPString LabelRequBy2();
    static const YCPString LabelRequire1();
    static const YCPString LabelRequire2();
    static const YCPString LabelUnresolvable();
    static const YCPString WarningLabel();
    static const YCPString ErrorLabel();
    static const YCPString NotifyLabel();
    static const YCPString DepsHelpLine();
    static const YCPString Solving();
    
   /**
     * text used in search popup
     */
    static const YCPString SearchPhrase();

   /**
     * text used in search popup
     */
    static const YCPString CheckDescr();
    
   /**
     * text used in search popup
     */
    static const YCPString IgnoreCase();

    /**
     * menu entry dependency popup
     */
    static const YCPString RequiredBy();
    
    /**
     * menu entry dependency popup
     */
    static const YCPString Alternatives();
        /**
     * menu entry dependency popup
     */
    static const YCPString ConflictDeps();
    
    /**
     * The headline of the help popup
     */
    static const YCPString PackageHelp();

    /**
     * The headline of the help YOU popup
     */
    static const YCPString YouHelp();

    /**
     *  The label of the OK button
     */
    static const YCPString OKLabel();

    /**
     *  The label of the Cancel button
     */
    static const YCPString CancelLabel();
    
    /**
     *  The label of the Cancel/Ignore button
     */
    static const YCPString CancelIgnore();
    
    /**
     *  The label of the Solve button
     */
    static const YCPString SolveLabel();
    
    /**
     * The label of the file list frame
     */
    static const NCstring FileList();
    
    /**
     * Part 1 of help text package installation
     */
    static const NCstring HelpPkgInst1();

    /**
     * Part 12 of help text package installation
     */
    static const NCstring HelpPkgInst12();

    /**
     * Part 13 of help text package installation
     */
    static const NCstring HelpPkgInst13();

    /**
     * help text package status
     */
    static const NCstring HelpOnStatus1();
    static const NCstring HelpOnStatus2();
    static const NCstring HelpOnStatus3(); 
    static const NCstring HelpOnStatus();

    static const NCstring YouHelp1();
    static const NCstring YouHelp2();
    static const NCstring YouHelp3();
   /**
     * Part of help text used in installed system
     */
    static const NCstring HelpPkgPostInst();

    /**
     * Part 2 of help text package installation
     */
    static const NCstring HelpPkgInst2();
       /**
     * Part 3 of help text package installation
     */
    static const NCstring HelpPkgInst3();
    /**
     * Part 4 of help text package installation
     */
    static const NCstring HelpPkgInst4();

    /**
     * Part 5 of help text package installation
     */
    static const NCstring HelpPkgInst5();

    static const NCstring HelpOnUpdate();

    /**
     * help package search
     */
    static const YCPString HelpOnSearch();

    /**
     * bold text Version: (richtext) 
     */
    static const NCstring Version();

    /**
     * bold text Installes version: (richtext) 
     */
    static const NCstring InstVersion();

    /**
     * bold text Size: (richtext) 
     */
    static const NCstring Size();

    /**
     * bold text Authors: (richtext) 
     */
    static const NCstring Authors();

    /**
     * bold text Provides: (richtext) 
     */
    static const NCstring Provides();

    /**
     * bold text Description: (richtext) 
     */
    static const NCstring ShortDescr();
    
    /**
     * bold text Requires: (richtext) 
     */
    static const NCstring Requires();

    /**
     * bold text Required by: (richtext) 
     */
    static const NCstring PreRequires();

    /**
     * bold text Conflicts with: (richtext) 
     */
    static const NCstring Conflicts();
    
    /**
     *  Column header package name
     */
    static const NCstring PkgName();

     /**
     *  Column header package version installed
     */
    static const NCstring PkgVersion();

    /**
     *  Column header package version new
     */
    static const NCstring PkgVersionNew();

    /**
     *  Column header package version installed
     */
    static const NCstring PkgVersionInst();
    
    /**
     *  Column header package summary
     */
    static const NCstring PkgSummary();

    /**
     *  Column header package size
     */
    static const NCstring PkgSize();

     /**
     *  Column header patch kind
     */
    static const NCstring PatchKind();
    
    /**
     *  Column header package status
     */
    static const NCstring PkgStatus();

    /**
     * dependency popup text
     */ 
    static const NCstring RequText();
    static const NCstring RequByText();
    static const NCstring UnresText();
    static const NCstring NoAvailText();
    static const NCstring NeedsText();
    static const NCstring ConflictText();
    
private:
    
    // Need no object of this class: hide default constructor
    PkgNames();

    /**
     *  Create an id value with ` at the beginning
     */
    static YCPValue createIdValue ( const std::string &name );

    /**
     *  Convert a NCstring into a YCPString
     */
    static YCPString toYCPString ( const NCstring & str );

};


///////////////////////////////////////////////////////////////////

#endif // PkgNames_h



