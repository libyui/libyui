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

   File:       PackageSelector.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCPkgTable.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPopupSearch.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCPopupTree.h"
#include "NCPopupSelection.h"
#include "PackageSelector.h"
#include "YSelectionBox.h"

#include <iomanip>
#include <list>
#include <string>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMYouPatchManager.h>

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPParser.h>
#include <ycp/YCPBlock.h>

using namespace std;

#define FAKE_INST_SRC 0

///////////////////////////////////////////////////////////////////
//
// CompFunc 
//
bool sortByName( PMSelectablePtr ptr1, PMSelectablePtr ptr2 )
{
    if ( ptr1->name() < ptr2->name() )
    {
	return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
// Constructor
//
PackageSelector::PackageSelector( Y2NCursesUI * ui, YWidgetOpt & opt )
    : y2ui( ui )
      , visibleInfo( YCPNull() )
      , filterPopup( 0 )
      , selectionPopup( 0 )
      , youMode( false )
{
    // Fill the handler map
    eventHandlerMap[ PkgNames::Packages()->toString() ]	= &PackageSelector::PackageListHandler;
    eventHandlerMap[ PkgNames::Cancel()->toString() ] 	= &PackageSelector::CancelHandler;
    eventHandlerMap[ PkgNames::OkButton()->toString() ]	= &PackageSelector::OkButtonHandler;
    eventHandlerMap[ PkgNames::Search()->toString() ] 	= &PackageSelector::SearchHandler;
    eventHandlerMap[ PkgNames::Diskspace()->toString() ] = &PackageSelector::DiskspaceHandler;
    eventHandlerMap[ PkgNames::Diskinfo()->toString() ]	= &PackageSelector::DiskinfoHandler;
    // Filter menu
    eventHandlerMap[ PkgNames::RpmGroups()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Selections()->toString() ] = &PackageSelector::FilterHandler;
    // Information menu
    eventHandlerMap[ PkgNames::Files()->toString() ]   	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PkgInfo()->toString() ] 	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::LongDescr()->toString() ]= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Versions()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::RequRel()->toString() ] = &PackageSelector::InformationHandler;
    // Action menu
    eventHandlerMap[ PkgNames::Toggle()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Select()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Deselect()->toString() ] = &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Delete()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Replace()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Update()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Installed()->toString() ]= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Taboo()->toString() ]	= &PackageSelector::StatusHandler;

    // help menu
    eventHandlerMap[ PkgNames::GeneralHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::StatusHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::FilterHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::InstSourceHelp()->toString() ] = &PackageSelector::HelpHandler;
    // FIXME: add handler for all `id s

    if ( opt.notifyMode.defined() )
	youMode = true;
    
    if ( !youMode )
    {
	// create the filter popup
	filterPopup = new NCPopupTree( wpos( 1, 1 ),	// position
				       this );	 

#if FAKE_INST_SRC
	{
	    Y2PM y2pm;
	    InstSrcManager& MGR = y2pm.instSrcManager();

	    Url url( "dir:///space/instTest" );

	    InstSrcManager::ISrcIdList nids;
	    PMError err = MGR.scanMedia( nids, url );

	    if ( nids.size() )
	    {
		err = MGR.enableSource( *nids.begin() );
		NCMIL << "Source enabled: " << err << endl;
	    }
	}
#endif
    
	// create the selections popup
	selectionPopup = new NCPopupSelection( wpos( 1, 1 ),
					       this );
    }
    
    NCstring text( "This is a development version of the NCurses single package selection, included in this Beta as a kind of demo.<br>Some things work, some don't, some work but are still really slow. You can view some package data, but setting a package status does not have any effect yet. As of now, all bug reports for this thing will be routed to /dev/null.<br>We are working heavily on it, so if you want to contribute, the easiest thing you can do right now is not swamp us with bugzilla reports, but let us work in the remaining time.<br> Thank you.<br> -gs" );
    NCPopupInfo info( wpos( 5, 5 ), YCPString( "Warning" ), text.YCPstr() );
    info.setNiceSize( 50, 20 );
    info.showInfoPopup( );

}



///////////////////////////////////////////////////////////////////
//
// Destructor
//
PackageSelector::~PackageSelector()
{
    if ( filterPopup )
    {
	delete filterPopup;
    }
    if ( selectionPopup )
    {
	delete selectionPopup;
    }
}

///////////////////////////////////////////////////////////////////
//
// handleEvent
//
// Handle event, calls corresponding handler-function
//
bool PackageSelector::handleEvent ( const NCursesEvent&   event )
{
    bool retVal = false;
    YCPValue currentId = YCPNull();

    if ( event == NCursesEvent::handled )
	return false;
    
    // Get the id of the widget which is affected
    if ( event == NCursesEvent::button )
    {
	currentId =  dynamic_cast<YWidget *>(event.widget)->id();
	UIMIL <<  "Selected widget id: " << currentId->toString() << endl;

    }
    else if ( event == NCursesEvent::menu )
    {
	currentId =  event.selection;
	UIMIL <<  "Selected menu item: " << currentId->toString() << endl;
    }
    
    // Find the handler-function for the given widget-nameId
    if ( ! currentId.isNull() )
    {
	tHandlerMap::iterator it = eventHandlerMap.find ( currentId->toString() );
    
	if (it != eventHandlerMap.end())    // if currentId found in map
	{ 
	    tHandlerFctPtr pFct = (*it).second;
	    retVal = (this->*pFct) ( event );    // call handler
	}
	else
	{
	    UIERR <<  "Unhandled event for widget-Id: " << currentId->toString() << endl;
	    
	    // return true means: don't leave the event loop in runPkgSelection  
	    retVal = true;
	}
    }
    else
    {
	UIERR << "Unknown event or id not valid" << endl;
    }

    return retVal;
}


///////////////////////////////////////////////////////////////////
//
// setVisibleInfo
//
// currently displayed package information
//
void PackageSelector::setVisibleInfo( const YCPValue & info )
{
    visibleInfo = info;
}

///////////////////////////////////////////////////////////////////
//
// fillHeader
//
// Fillup the column headers of the package table 
//
void PackageSelector::fillHeader( NCPkgTable *pkgTable )
{
    vector<NCstring> header;
    header.reserve(5);
    
    header.push_back( PkgNames::PkgStatus() );
    header.push_back( PkgNames::PkgName() );
    header.push_back( PkgNames::PkgVersion() );
    header.push_back( PkgNames::PkgSummary() );
    header.push_back( PkgNames::PkgSize() );

    if ( pkgTable )
    {
	pkgTable->setHeader( header );
    }
    else
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
// fillAvailableList
//
// Fills the package table (on bottom ) with the list of available packages 
//
bool PackageSelector::fillAvailableList( NCPkgTable * pkgTable, PMObjectPtr pkgPtr )
{
    unsigned int i = 0;

    if ( !pkgTable )
    {
	NCERR << "No table widget for availbale packages existing" << endl;
	return false;
    }

    if ( !pkgPtr->hasSelectable() )
    {
	NCERR << "Package pointer not valid" << endl;
	return false;
    }
    
    // clear the package table
    pkgTable->itemsCleared ();
    
    // get the selectable
    PMSelectablePtr selectable = pkgPtr->getSelectable();
    NCMIL << "Number of available packages: " << selectable->availableObjs() << endl;
    
    std::list<PMObjectPtr>::const_iterator it = selectable->av_begin();

    // show all availables
    for ( i = 0, it = selectable->av_begin(); it != selectable->av_end();  ++it, i++ )
    {
	vector<string> pkgLine;
	pkgLine.reserve(5);	

	pkgLine.push_back( selectable->name() );	// package name
	pkgLine.push_back( (*it)->version() );		// the version
	pkgLine.push_back( (*it)->summary() );  	// short description
	FSize size = (*it)->size();     	// installed size
	pkgLine.push_back( size.asString() );
	
	pkgTable->addLine( selectable->status(), // get the package status
			   pkgLine,
			   i,		 // the index
			   (*it) );	 // the corresponding package pointer
    }

    if ( pkgTable->getNumLines() > 0 )
    {
	pkgTable->setCurrentItem( 0 );
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// showSelPackages
//
// Fills the package table with the list of packages of the given selection
//
bool PackageSelector::showSelPackages( const YCPString & label,  PMSelectionPtr sel )
{
    list<PMPackagePtr> pkgList;
    list<PMPackagePtr>::iterator listIt;
    unsigned int i;
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();
    
    if ( sel )
    {
	pkgList = sel->inspacks_ptrs ( );
    }

    for ( i = 0, listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt, i++ )    
    {
	createListEntry( packageList, (*listIt), i );
    }
      
    if ( !label.isNull() )
    {
	NCMIL << "  Label: " << label->toString() << endl;

        // show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }
   
    return true;

}

///////////////////////////////////////////////////////////////////
//
// fillSearchList
//
// Fills the package table
//
bool PackageSelector::fillSearchList( const YCPString & expr )
{
    if ( !packageList )
    {
	return false;
    }

    // FIXME: get packages from package manager
    
    // set filter label to 'Search'
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( PkgNames::SearchResults() );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillDefaultList
//
// Fills the package table with the list of default rpm group
// or show the complete patch list in YOU mode
//
bool PackageSelector::fillDefaultList( NCPkgTable * pkgTable )
{
    if ( !packageList )
    {    
	// set the package list widget
	packageList = pkgTable;
    }

    if ( !youMode )
    {
	YStringTreeItem * defaultGroup =  filterPopup->getDefaultGroup();

	if ( defaultGroup )
	{
	    fillPackageList ( YCPString( defaultGroup->value().translation()),
			      defaultGroup );
	}
	else
	{
	    NCERR << "No default RPM group availbale" << endl;
	}
    }
    else
    {
	fillPatchList();
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchList
//
// Fills the package table with the list of YOU patches
//
bool PackageSelector::fillPatchList( )
{
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    PMManager::PMSelectableVec::const_iterator it = Y2PM::youPatchManager().begin();

    // fill the package table
    unsigned int i;
    PMYouPatchPtr patchPtr;    

    for ( i = 0, it = Y2PM::youPatchManager().begin() ;
	  it != Y2PM::youPatchManager().end();
	  ++it, i++ )
    {
	PMSelectablePtr selectable = *it;

	if ( selectable->installedObj() )
	    patchPtr = selectable->installedObj();
	else if ( selectable->candidateObj() )
	    patchPtr = selectable->candidateObj();
	else
	    patchPtr = selectable->theObject();

	createPatchEntry( packageList, patchPtr, i );
			  
    }

    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( YCPString( "Patches" ) );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPackageList
//
// Fills the package table with the list of packages matching
// the selected filter
//
bool PackageSelector::fillPackageList( const YCPString & label, YStringTreeItem * rpmGroup )
{
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    // get the package list and sort it
    list<PMSelectablePtr> pkgList( Y2PM::packageManager().begin(), Y2PM::packageManager().end() );
    pkgList.sort( sortByName );

    // fill the package table
    list<PMSelectablePtr>::iterator listIt;
    unsigned int i;
    PMPackagePtr pkgPtr;


    for ( i = 0, listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt, i++ )
    {
	PMSelectablePtr selectable = *listIt;
	    
	// Multiple instances of this package may or may not be in the same
	// RPM group, so let's check both the installed version (if there
	// is any) and the candidate version.
	//
	// Make sure we emit only one filterMatch() signal if both exist
	// and both are in the same RPM group. We don't want multiple list
	// entries for the same package!
	    
	bool match =
	    check( selectable->installedObj(), rpmGroup, i ) || 
	    check( selectable->candidateObj(), rpmGroup, i );  

	// If there is neither an installed nor a candidate package, check
	// any other instance.  
	    
	if ( ! match			&&
	     ! selectable->installedObj()	&&
	     ! selectable->candidateObj()     )
	    check( selectable->theObject(), rpmGroup, i );

    }

    if ( !label.isNull() )
    {
	NCMIL <<  "Label: " <<  label->toString() << endl;

	// show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// check
//
//
bool PackageSelector::check( PMPackagePtr pkg,
			     YStringTreeItem * rpmGroup,
			     int index )
{
    if ( ! pkg || ! rpmGroup )
	return false;

    if ( pkg->group_ptr() == 0 )
    {
	y2error( "NULL pointer in group_ptr()!" );
	return false;
    }

    if ( pkg->group_ptr()->isChildOf( rpmGroup ) )
    {
	createListEntry( packageList, pkg, index );
	
	return true;
    }
    else
    {
	return false;
    }
}

///////////////////////////////////////////////////////////////////
//
// createListEntry
//
//
bool PackageSelector::createListEntry ( NCPkgTable *pkgTable,
					PMPackagePtr pkgPtr,
					unsigned int index )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);
    
    if ( !pkgPtr || !pkgTable )
    {
	NCERR << "No valid package available" << endl;
	return false;
    }

    pkgLine.push_back( pkgPtr->getSelectable()->name() );	// package name
    pkgLine.push_back( pkgPtr->version() );	// version
    pkgLine.push_back( pkgPtr->summary() );  	// short description
    FSize size = pkgPtr->size();     	// installed size
    pkgLine.push_back( size.asString() );

    pkgTable->addLine( pkgPtr->getSelectable()->status(), //  get the package status
		       pkgLine,
		       index,		// the index
		       pkgPtr );	// the corresponding package pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// createPatchEntry
//
//
bool PackageSelector::createPatchEntry ( NCPkgTable *pkgTable,
					 PMYouPatchPtr patchPtr,
					 unsigned int index )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);
    
    if ( !patchPtr || !pkgTable )
    {
	NCERR << "No valid patch available" << endl;
	return false;
    }

    // FIXME: get more patch attributes/use summary()
    pkgLine.push_back( patchPtr->getSelectable()->name() );	// name
    pkgLine.push_back( patchPtr->shortDescription() );  	// short description
    //FSize size = patchPtr->size();     	// installed size
    //pkgLine[2] = size.asString();
    pkgLine.push_back( "950 kB" );
    
    pkgTable->addLine( patchPtr->getSelectable()->status(), //  get the status
		       pkgLine,
		       index,		// the index
		       patchPtr );	// the corresponding pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// SearchHandler
//
// Opens popup for package search and asks the package manager
//
bool PackageSelector::SearchHandler( const NCursesEvent& event)
{
    if ( !packageList )
    {
	return false;
    }
    
    // open the search poup and get the search expression
    NCPopupSearch pkgSearch( wpos( 1, 1 ) );

    NCursesEvent retEvent = pkgSearch.showSearchPopup();

    if ( !retEvent.result.isNull() )
    {	
	YCPString searchExpr = retEvent.result->asString();
    
	NCMIL << "Searching for: " <<  searchExpr->toString() << endl;

	if ( !youMode )
	{
	    // fill the package list with packages matching the search expression
	    fillSearchList ( searchExpr );
	    showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
	}
    }
    else
    {
	NCMIL << "Search is canceled"  << endl;
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// InformationHandler
//
// Sets the member variable visibleInfo. If required,
// replaces the currently active package information widget
// ( e.g. by the list of available packages ).
//
bool PackageSelector::InformationHandler( const NCursesEvent&  event )
{
    if ( !packageList )
    {
	return false;
    }
    
    if ( visibleInfo->compare( event.selection ) == YO_EQUAL )
    {
	// information selection has not changed
	return true;
    }

    // set visibleInfo
    visibleInfo = event.selection;

    if ( visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	// show the package table
	const char * tableLayout = "`ReplacePoint( `id(`replaceinfo), `PkgSpecial( `id(`availpkgs), `opt(`notify), \"pkgTable\" ) )"; 
	YCPParser parser( tableLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );

	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));

	if ( pkgAvail )
	{
	    // fillHeader( pkgAvail );  
	    // set the connection to the PackageSelector !!!!
	    pkgAvail->setPackager( this );
	    // set status strategy
	    ObjectStatStrategy * strategy = new AvailableStatStrategy();
	    pkgAvail->setStatusStrategy( strategy );
	    fillAvailableList( pkgAvail, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}
    }
    else
    {
	// show the rich text widget
	const char * textLayout = "`ReplacePoint( `id(`replaceinfo), `RichText( `id(`description), \" \") )"; 
	YCPParser parser( textLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );
    
	showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    }

    packageList->setKeyboardFocus();
    
    UIMIL << "Change package info to: " << visibleInfo->toString() << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
//
// FilterHandler
//
// Opens the popup with rpm group tree, selections ... and display the
// corresponding package list
//
bool PackageSelector::FilterHandler( const NCursesEvent&  event )
{
    NCursesEvent retEvent;

    if ( !packageList || event.selection.isNull() )
    {
	return false;
    }

    if ( youMode )
    {
	// up to now: nothing to do
	return true;
    }
    
    if ( event.selection->compare( PkgNames::RpmGroups() ) == YO_EQUAL )
    {
	if ( filterPopup )
	{
	    // show the filter popup (fills the package list) 
	    retEvent = filterPopup->showFilterPopup( );
	}
	
    }
    else if ( event.selection->compare( PkgNames::Selections() ) == YO_EQUAL )
    {
	if ( selectionPopup )
	{
	    // show the selection popup
	    retEvent = selectionPopup->showSelectionPopup( );
	}
    }

    showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    packageList->setKeyboardFocus();	
    
    // return true means: don't leave the event loop in runPkgSelection 
    return true;
}

///////////////////////////////////////////////////////////////////
//
// StatusHandler
//
// Sets the package status
//
bool PackageSelector::StatusHandler( const NCursesEvent&  event )
{
    if ( !packageList )
    {
	return false;
    }
    
    if (  packageList->getNumLines() == 0 )
    {
	// nothing to do
	return true;
    }

    // call the corresponding method of NCPkgTable
    if ( event.selection->compare( PkgNames::Toggle() ) == YO_EQUAL )
    {
	packageList->toggleStatus( packageList->getDataPointer( packageList->getCurrentItem() ));
    }
    else if ( event.selection->compare( PkgNames::Select() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToInstall );
    }
    else if ( event.selection->compare( PkgNames::Deselect() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgNoInstall );
    }
    else if ( event.selection->compare( PkgNames::Delete() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToDelete );
    }
    else if ( event.selection->compare( PkgNames::Replace() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToReplace );
    }
    else if ( event.selection->compare( PkgNames::Update() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgToUpdate );
    }
    else if ( event.selection->compare( PkgNames::Installed() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgInstalled );	
    }
    else if ( event.selection->compare( PkgNames::Taboo() ) == YO_EQUAL )
    {
	packageList->setNewStatus( PkgTaboo );	
    }
    packageList->setKeyboardFocus();
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// PackageListHandler
//
// Handles actions not directly related to the package list.
// 
bool PackageSelector::PackageListHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskinfoHandler
//
// Opens a popup with disk space information.
// 
bool PackageSelector::DiskinfoHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskspaceHandler
// 
// Show the required disk space
//
bool PackageSelector::DiskspaceHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// HelpHandler
// 
// Show the help popup
//
bool PackageSelector::HelpHandler( const NCursesEvent&  event )
{
    NCstring text ( "" );
    
    if ( event.selection->compare( PkgNames::GeneralHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpPkgInst1();
	text += PkgNames::HelpPkgInst12();
	text += PkgNames::HelpPkgInst13();
	text += PkgNames::HelpPkgInst2();
	text += PkgNames::HelpPkgInst3();
	text += PkgNames::HelpPkgInst4();
	text += PkgNames::HelpPkgInst5();		
    }
    else if ( event.selection->compare( PkgNames::StatusHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnStatus1();
	text += PkgNames::HelpOnStatus2();
	text += PkgNames::HelpOnStatus3();
	// FIXME: text depends on flag isNew/isPostInstallation 

	// text += PkgNames::HelpOnStatusNew();
	text += PkgNames::HelpOnStatusPost();
    }
    
    // open the popup with the help text
    NCPopupInfo pkgHelp( wpos( 1, 1 ), PkgNames::PackageHelp(), text.YCPstr() );
    pkgHelp.showInfoPopup( );
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// CancelHandler
//
// Cancel button handler.
// 
bool PackageSelector::CancelHandler( const NCursesEvent&  event )
{
    // FIXME - show a popup and ask the user
    // FIXME - restore state

    NCMIL <<  "Cancel button pressed - leaving package selection" << endl;

    // return false, which means stop the event loop (see runPkgSelection)
    return false;
}

///////////////////////////////////////////////////////////////////
//
// OkButtonHandler
//
// OK button handler.
// 
bool PackageSelector::OkButtonHandler( const NCursesEvent&  event )
{
    // FIXME - do a final dependency check
    // FIXME - check diskspace

    NCMIL <<  "OK button pressed - leaving package selection, starting installation" << endl;

    // return false, leave the package selection
    return false;
}

///////////////////////////////////////////////////////////////////
//
// showPackageInformation
//
// Shows the package information
//
bool PackageSelector::showPackageInformation ( PMObjectPtr pkgPtr )
{
    if ( !pkgPtr )
    {
	NCERR << "Package not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }
    
    if ( youMode )
    {
	NCstring text ( ((PMYouPatchPtr)pkgPtr)->shortDescription() );
	
	// show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( text.YCPstr() );
	}
	
	return true;
    }
    NCMIL << "INFO " << visibleInfo->toString() << endl;
    
    if ( visibleInfo->compare( PkgNames::LongDescr() ) == YO_EQUAL )
    {
	// ask the package manager for the description of this package
	list<string> value = pkgPtr->description();
	string descr = createDescrText( value );

        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(descr) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Files() ) == YO_EQUAL )
    {
	// get the file list from the package manager/show the list
	list<string> fileList = ((PMPackagePtr)pkgPtr)->filenames();
	
	// get the widget id 
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );  
	if ( descrInfo  )
	{
	    NCMIL <<  "Size of the file list: " << fileList.size() << endl;
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(createText(fileList, false)) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));
	if ( pkgAvail )
	{
	    fillAvailableList( pkgAvail, pkgPtr );
	}
    }
    else if ( visibleInfo->compare( PkgNames::PkgInfo() ) == YO_EQUAL )
    {
	string instVersion = "";
	string version = "";
	NCstring text ( "" );
	
	if ( pkgPtr->hasSelectable() )
	{
	    text += NCstring( pkgPtr->getSelectable()->name() );
	    text += NCstring( " - " );
	}

	text += NCstring( pkgPtr->summary() );		// the summary
	text += NCstring( "<br>" );

	if ( pkgPtr->hasInstalledObj() && pkgPtr->hasCandidateObj() )
	{
	    instVersion = pkgPtr->getInstalledObj()->version();
	    version = pkgPtr->getCandidateObj()->version();
	}
	else
	{
	    version = pkgPtr->version();
	}

	text += NCstring( PkgNames::Version() );
	text += NCstring( version );
	if ( instVersion != "" )
	{
	    text += NCstring( "  " );
	    text += NCstring( PkgNames::InstVersion() );
	    text += NCstring( instVersion );
	}

	text += NCstring( "  " );
	
	// show the size
	text += NCstring( PkgNames::Size() );
	text += NCstring( pkgPtr->size().asString() );
	
	text += NCstring( "<br>" );

	// show Provides:
	text += NCstring( PkgNames::Provides() );
	list<PkgRelation> provides = pkgPtr->provides();	// PMSolvable
	text += NCstring( createRelLine(provides) );
	text += NCstring( "<br>" );

	// show the authors
	text += NCstring( PkgNames::Authors() );
	list<string> authors = ((PMPackagePtr)pkgPtr)->authors(); // PMPackage	
	text += NCstring( createText( authors, true ) );
	
        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( text.YCPstr() );
	}
    }

    NCDBG <<  "Showing package information: " << visibleInfo->toString() << endl;
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// createDescrText
//
string PackageSelector::createDescrText( list<string> value )
{
    string html_text = "";
    
    bool auto_format = true;
    list<string>::const_iterator it = value.begin();

    while ( it != value.end() )
    {
	string line = (*it);

	if ( line.substr( 0, 7 ) ==  "Authors" )
	{
	    line = "<b>" + line + "</b>";
	    auto_format = false;
	}

	if ( auto_format )
	{
	    if ( line.length() == 0 )	// Empty lines mean new paragraph
		html_text += "</p><p>";
	    else
		html_text += line;
	}
	else
	{
	    html_text += line + "<br>";
	}	
	
	++it;
    }

    html_text += "</p>";

    return html_text;
}

///////////////////////////////////////////////////////////////////
//
// createProvides
//
string PackageSelector::createRelLine( list<PkgRelation> info )
{
    list<PkgRelation>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it).asString();
	if ( i < info.size()-1 )
	{
	    text = text + ", ";
	}
    }

    return text;
}


///////////////////////////////////////////////////////////////////
//
// createText
//
string PackageSelector::createText( list<string> info, bool oneline )
{
    list<string>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it);
	if ( i < info.size()-1 )
	{
	    if ( oneline )
	    {
	    text = text + ", ";
	    }
	    else
	    {
		text = text + "<br>";	
	    }
	}
    }

    return text;
}
