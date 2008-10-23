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

   File:       NCPackageSelector.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPackageSelector_h
#define NCPackageSelector_h

#include <iosfwd>

#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "YNCursesUI.h"
#include "NCPkgStrings.h"

#include <map>          
#include <string>
#include <utility>      // for STL pair

#include <YRpmGroupsTree.h>
#include "YWidgetID.h"

#include <zypp/ResObject.h>
#include <zypp/ui/Selectable.h>
#include <zypp/Patch.h>

#include "NCPkgFilterRPMGroups.h"
#include "NCPkgFilterSearch.h"
#include "NCPkgFilterInstSummary.h"
#include "NCPkgTable.h"
#include "NCEmpty.h"
#include "NCPkgMenuFilter.h"
#include "NCPkgPackageDetails.h"
#include "NCPkgPopupDeps.h" 
#include "NCPkgFilterMain.h" 
#include "NCPkgSearchSettings.h"

#include "NCPkgSelMapper.h"

class NCPkgFilterPattern;
class NCPkgLocaleTable;
class NCPkgRepoTable;
class LangCode;
class NCPkgPopupDeps;
class NCPkgDiskspace;
class NCPkgFilterRPMGroups;
class NCPkgFilterSearch;
class NCPkgFilterInstSummary;
class NCPkgPopupFile;
class NCPkgMenuExtras;
class NCPkgMenuDeps;
class NCPkgMenuAction;
class NCPkgMenuView;
class NCPkgMenuHelp;
class NCPkgSearchSettings;
class NCPkgPackageDetails;
class NCPkgFilterMain;
class NCPushButton;
class YNCursesUI;
class NCMenuButton;
class YReplacePoint;

inline bool ic_compare ( char c1, char c2 )
{
    return ( toupper( c1 ) == toupper( c2 ) );
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPackageSelector
//
//	DESCRIPTION : holds the data and handles events
//
class NCPackageSelector
{

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPackageSelector & OBJ );

  NCPackageSelector & operator=( const NCPackageSelector & );
  NCPackageSelector            ( const NCPackageSelector & );

  private:

    // typedef for the pointer to handler member function
    typedef bool (NCPackageSelector::* tHandlerFctPtr) ( const NCursesEvent& event );

    // typedef for the internal map: key=nameId, value=handler-fct-ptr
    typedef std::map<std::string, tHandlerFctPtr> tHandlerMap;

    tHandlerMap eventHandlerMap;    	// event handler map
    
    NCPkgFilterRPMGroups * filterPopup;	// the rpm group tags popup

    NCPkgPopupDeps * depsPopup;		// the package dependeny popup

    NCPkgFilterPattern * patternPopup;    	// the pattern popup
    NCPkgLocaleTable * languagePopup;	// language popup
    NCPkgRepoTable * repoPopup;

    NCPkgDiskspace * diskspacePopup;	// the popup showing the disk usage

    NCPkgFilterSearch * searchPopup; 	// the package search popup
    NCPkgFilterInstSummary *inst_summary;

    bool youMode;			// YOU
    bool updateMode;			// Update
    bool testMode;			// testing
    bool repoMgrEnabled;
    bool autoCheck;
    
    YRpmGroupsTree * _rpmGroupsTree;	// rpm groups of the found packages

    // the package table
    NCPkgTable * pkgList;

    NCPkgMenuDeps *depsMenu;
    NCPkgMenuView *viewMenu;
    NCPkgMenuExtras *extrasMenu;
    NCPkgMenuHelp *helpMenu;
    NCPkgFilterMain *filterMain;
    NCPkgMenuAction *actionMenu;
    NCPkgMenuFilter *filterMenu;
    // FIXME - add update list to NCPkgFilterMain
    YMenuItem * updatelistItem;
    
     // labels
    YLabel * packageLabel;
    YLabel * diskspaceLabel;
    YLabel *patternLabel;

    // information about packages
    NCPkgPackageDetails * infoText;	// short/longdecsription, filelist
    NCRichText * filter_desc;
    NCInputField *searchField;
    NCPkgSearchSettings *searchSet;
    YReplacePoint * replacePoint; // replace point for info text
    YReplacePoint * replPoint; 
    YReplacePoint * replPoint2; //tohle pak urcite prejmenuj, Bublino
    
    NCPkgTable * versionsList;	// list of available package versions
    // information about patches
    NCPkgTable * patchPkgs;	// pakages belonging to a patch
    NCPkgTable * patchPkgsVersions;	// versions of packages above 
    
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    
    YMenuItem * visibleInfo;		// current visible package info (description, file list, ...)
    
    // Mapping from ZyppPkg to the correspoinding ZyppSel.
    NCPkgSelMapper selMapper;

    set<string> verified_pkgs;

  public:
	enum FilterMode 
	{
	    Patterns,
	    Languages,
	    Repositories,
	    RPMGroups,
	    Search,
	    Summary
	};

   /**
     * The package selector class handles the events and holds the
     * data needed for the package selection.
     * @param ui The NCurses UI
     * @param opt The widget options
     */
    NCPackageSelector( long modeFlags );

    /**
     * Destructor
     */ 
    virtual ~NCPackageSelector();

    void setFlags( long modeFlags);

    /**
    * Create layout for the PackageSelector
    * @param parent Parent is PackageSelectorStart
    * @param type   The package table type	
    * @return void
    */
    void createPkgLayout( YWidget * parent, NCPkgTable::NCPkgTableType type );

    /**
    * Create layout for the Online Update
    * @param parent Parent is PackageSelectorStart
    * @return void
    */
    void createYouLayout( YWidget * parent ); 	

    // returns the package table widget 
    NCPkgTable * PackageList();
    NCPkgPopupDeps *DepsPopup() { return depsPopup; }
    NCPkgDiskspace *diskSpacePopup() { return diskspacePopup; }
    YLabel *PackageLabel() { return packageLabel; } 
    YLabel *PatternLabel() { return patternLabel; } 

    NCPkgPackageDetails *InfoText() { return infoText; }
    void setInfoText ( NCPkgPackageDetails *itext ) { infoText = itext ;}

    NCPkgTable *VersionsList() { return versionsList; }
    void setVersionsList ( NCPkgTable *table ) { versionsList = table; }

    NCPkgTable * PatchPkgs() { return patchPkgs; }
    NCPkgTable * PatchPkgsVersions() { return patchPkgsVersions; }
    
    YReplacePoint *ReplacePoint() { return replacePoint; }
    NCRichText *FilterDescription() { return filter_desc; }
    NCPkgSearchSettings *SearchSettings() { return searchSet; }
    void setSearchField( NCInputField *expr ) { searchField = expr; }

    bool checkNow( bool *ok ); 
    bool verifySystem( bool *ok ); 

    /**
    * Fills the package table with YOU patches matching the filter
    * @param filter
    * @return bool
    */
    bool fillPatchList( NCPkgMenuFilter::PatchFilter filter );
			
   /**
    * Fills the package table with packages with update problems
    * @return bool
    */
    bool fillUpdateList( );

      
   /**
    * Fills the list of packages belonging to the youPatch
    * @param pkgTable  The table widget
    * @param youPatch Show all packages belonging to the patch
    * @param versions Show all versions of all packages belonging to the patch
    * @return bool
    */ 
    bool fillPatchPackages ( NCPkgTable * pkgTable, ZyppObj youPatch, bool versions = false );
    
   /**
    * Fills the package table with packages matching the search expression
    * @param expr The search expression
    * @param ignoreCase Ignore case (true or false)
    * @param checkName Search in package name (true or false)
    * @param checkSummary Check the summary (true or false)
    * @param checkProvides Check in Provides (true or false)
    * @param checkRequires Check in Requires (true or false)
    * @return bool
    */ 
    bool fillPatchSearchList( const string & expr );

    bool fillDefaultList();
    
    bool isYouMode() { return youMode; }

    bool isUpdateMode() { return updateMode; }

    bool isRepoMgrEnabled() { return repoMgrEnabled; }

    bool isAutoCheck() { return autoCheck; }

    bool isTestMode() { return testMode; }
    
    void AutoCheck( bool check) { autoCheck = check; }

     /**
     * Handle the given event. For the given event (the widget-id
     * is contained in the event) the corresponding handler is executed.
     * @param event The NCurses event
     * @return bool
     */
    bool handleEvent( const NCursesEvent& event );

    /**
    * Creates an NCPkgTable widget and shows all versions
    * of all packages belonging to a patch
    * @return void 
    */
    void showPatchPkgVersions();

    /**
    * Creates an NCPkgTable widget and shows all packages
    * belonging to a patch
    * @return void 
    */
    void showPatchPackages();

    /**
    * Creates an NCPkgTable widget and shows all verions
    * a the selected package
    * @return void 
    */
    void showVersionsList();

    /**
    * Creates an NCRichText widget for package (patch)
    * information
    * @return void 
    */
    void showInformation();
    
    void clearInfoArea();

    wrect deleteReplacePoint();
    
    void replaceFilter ( FilterMode mode);
    void replaceFilterDescr ( bool b );
    /**
    * Handler function for "OK button pressed"
    * @param event The Ncurses event
    * @return bool 
    */
    bool OkButtonHandler ( const NCursesEvent& event );

   /**
    * Handler function for "Cancel button pressed"
    * @param event The Ncurses event
    * @return bool
    */
    bool CancelHandler ( const NCursesEvent& event );

   /**
     * Handles hyperlinks in package description
     * @param link The link
     * @return bool
     */
    bool LinkHandler ( string link );
    
    /**
    * Checks and shows the dependencies
    * @param doit true: do the check, false: only check if auto check is on
    */
    bool showPackageDependencies ( bool doit );

    /**
    * Checks and shows the selectiondependencies
    */
    void showSelectionDependencies ( );
    
   /**
    * Updates the status in list of packages
    */
    void updatePackageList();
    
     /**
     * Check if 'patch' matches the selected filter.
     * Returns true if there is a match, false otherwise or if 'patch' is 0.
     * @return bool
     **/
    bool checkPatch( ZyppPatch patch,
		     ZyppSel selectable,
		     NCPkgMenuFilter::PatchFilter filter );
   
    /**
    * Returns whether automatic dependency is on or off
    * @return bool   
    */ 
    bool autoChecking() { return autoCheck; }

    /**
     * Creates a text from a list of strings which may contain HTML tags
     * @param t used to be list, now a single string. but what does it contain?
     * @return string	The text
     */
    string createDescrText( string t );

    /**
     * Calls the package mananager (updateDu()) and shows the required disk space
     */ 
    void showDiskSpace();

    /**
     * Shows the total download size
     */ 
    void showDownloadSize();

    /**
     * Check for changes
     */
    void saveState();
    void restoreState();
    bool diffState();

   /**
     * Check for license
     */
    bool showPendingLicenseAgreements();
    bool showPendingLicenseAgreements( ZyppPoolIterator begin, ZyppPoolIterator end );

   /**
    * Show popup with license.
    * @return bool
    */   
    bool showLicenseAgreement( ZyppSel & slbPtr , string licenseText );

   /**
    * Get list of packages already selected for automatic changes
    * (usually via 'verify system' call)
    * @return std::set <string>
    */
    set <string> getVerifiedPkgs() 
    {
	return verified_pkgs;
    }

    /**
     * Insert package name into the list of already selected for automatic changes
     * @param pkgname Package name
     */
    void insertVerifiedPkg( string pkgname ) {
	verified_pkgs.insert( pkgname);
    }

    /**
     * Empty the set of packages selected for automatic changes
     * @return void
     */
    void clearVerifiedPkgs() {
	if ( !verified_pkgs.empty() )
	{
	    yuiMilestone() << "Discarding auto-dependency changes" << endl;
	    verified_pkgs.clear();
	}
    }

};

///////////////////////////////////////////////////////////////////

#endif // NCPackageSelector_h
