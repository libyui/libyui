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

   File:       NCPkgTable.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPkgTable.h"
#include "NCTable.h"
#include "NCPopupInfo.h"

#include "PackageSelector.h"
#include <y2pm/PMSelectable.h>
#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstTarget.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTableTag::NCPkgTableTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgTableTag::NCPkgTableTag( PMObjectPtr objPtr, PMSelectable::UI_Status stat )
      : NCTableCol( NCstring( "    " ), SEPARATOR )
	, status ( stat )
	, dataPointer( objPtr )
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTableTag::DrawAt
//	METHOD TYPE : virtual DrawAt
//
//	DESCRIPTION :
//
void NCPkgTableTag::DrawAt( NCursesWindow & w, const wrect at,
			    NCTableStyle & tableStyle,
			    NCTableLine::STATE linestate,
			    unsigned colidx ) const
{
    NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

    string statusStr =  statusToStr( status );
    w.addch( at.Pos.L, at.Pos.C, statusStr.c_str()[0] );
    w.addch( at.Pos.L, at.Pos.C +1, statusStr.c_str()[1] );
    w.addch( at.Pos.L, at.Pos.C +2, statusStr.c_str()[2] );
    w.addch( at.Pos.L, at.Pos.C +3, statusStr.c_str()[3] );
}


string NCPkgTableTag::statusToStr( PMSelectable::UI_Status stat ) const
{
     // convert PMSelectable::UI_Status to string
    switch ( stat )
    {
	case PMSelectable::S_NoInst:	// Is not installed and will not be installed
	    return "    ";
	case PMSelectable::S_KeepInstalled: 	// Is installed - keep this version
	    return "  i ";
	case PMSelectable::S_Install:	// Will be installed
	    return "  + ";
	case PMSelectable::S_Del:	// Will be deleted
	    return "  - ";
	case PMSelectable::S_Update:	// Will be updated
	    return "  > ";
	case PMSelectable::S_AutoInstall: // Will be automatically installed
	    return " a+ ";
	case PMSelectable::S_AutoDel:	// Will be automatically deleted
	    return " a- ";
	case PMSelectable::S_AutoUpdate: // Will be automatically updated
	    return " a> ";    
	case PMSelectable::S_Taboo:	// Never install this 
	    return " ---";
	default:
	    return "####";
    }

    return "    ";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::NCPkgTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgTable::NCPkgTable( NCWidget * parent, YWidgetOpt & opt )
    : NCTable( parent, opt, vector<string> () )
      , packager ( 0 )
      , statusStrategy( new PackageStatStrategy )	// default strategy: packages
      , tableType ( T_Packages )			// default type: packages
{
    WIDDBG << endl;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::~NCPkgTable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgTable::~NCPkgTable()
{
    delete statusStrategy;
    WIDDBG << endl;
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::addLine
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgTable::addLine( PMSelectable::UI_Status stat,
			  const vector<string> & elements,
			  int index,
			  PMObjectPtr objPtr )
{
    vector<NCTableCol*> Items( elements.size()+1, 0 );
    
    // fill first column (containing the status information and the package pointer)
    Items[0] = new NCPkgTableTag( objPtr, stat );

    for ( unsigned i = 1; i < elements.size()+1; ++i ) {
	// use YCPString to enforce recoding from 'utf8'
	Items[i] = new NCTableCol( YCPString( elements[i-1] ) );
    }
    pad->Append( Items );
    
    // don't call DrawPad(); for every line - is called once after the loop
  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::itemsCleared
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgTable::itemsCleared()
{
    return NCTable::itemsCleared();  
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::cellChanged
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgTable::cellChanged( int index, int colnum, const YCPString & newtext )
{
    return NCTable::cellChanged( index, colnum, newtext );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::changeStatus
//	METHOD TYPE : bool
//
//	DESCRIPTION : sets the new status in first column of the package table
//		      and informs the package manager
//
bool NCPkgTable::changeStatus( PMSelectable::UI_Status newstatus,
			       const PMObjectPtr & objPtr,
			       bool singleChange )
{
    if ( !packager || !objPtr )
	return false;

    list<string> notify;
    YCPString header( "" );
    
    bool ok = false;

    if ( newstatus == PMSelectable::S_Del
	 || newstatus == PMSelectable::S_NoInst
	 || newstatus == PMSelectable::S_Taboo )
    {
	if ( objPtr->hasCandidateObj() )
	{
	    notify = objPtr->getCandidateObj()->delnotify();
	    header = YCPString(PkgNames::WarningLabel().str());
	}
    }
    if ( newstatus == PMSelectable::S_Install )
    {
	if ( objPtr->hasCandidateObj() )
	{	
	    notify = objPtr->getCandidateObj()->insnotify();
	    header = YCPString(PkgNames::NotifyLabel().str());
	}
    }
    
    if ( !notify.empty() )
    {
	NCPopupInfo info( wpos(3, 3),
			  header,
			  YCPString( packager->createText( notify, false ) ) );
	info.showInfoPopup( );
    }
    
    // inform the package manager
    ok = statusStrategy->setPackageStatus( newstatus,
					   objPtr );

    if ( ok && singleChange )
    {
	if ( tableType == T_Packages
	     || tableType == T_Update
	     || tableType == T_Availables )
	{
	    // check/show dependencies of packages
	    packager->showPackageDependencies( false );	// only check if automatic check is ON
	}
        else if ( tableType == T_Selections )
	{
	    // check/show dependencies of selections 
	    packager->showSelectionDependencies();
	}
	
	// update this list to show the status changes
	updateTable();
	
	if ( tableType == T_Availables
	     || tableType == T_Dependency
	     || tableType == T_DepsPackages )
	{
	    // additionally update the package list
	    packager->updatePackageList();
	}

	// show the required diskspace
	packager->showDiskSpace();
    }

    return ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::updateTable
//	METHOD TYPE : bool
//
//	DESCRIPTION : set the new status info if status has changed
//
bool NCPkgTable::updateTable()
{
    unsigned int size = getNumLines();
    unsigned int index = 0;
    bool ret = true;

    while ( index < size )
    {
    	// get the table line 
	NCTableLine * cl = pad->ModifyLine( index );
	if ( !cl )
	{
	    ret = false;
	    break;
	}

        // get first column (the column containing the status info)
	NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( cl->GetCol( 0 ) );
	// get the object pointer
	PMObjectPtr objPtr = getDataPointer( index );

	if ( !cc )
	{
	    ret = false;
	    break;
	}
	
	PMSelectable::UI_Status newstatus = PMSelectable::S_NoInst;
	if ( objPtr )
	{
	    // get the new status - use particular strategy
	    newstatus = statusStrategy->getPackageStatus( objPtr);
	}
	// set new status (if status has changed)
	if ( getStatus(index) != newstatus )
	{
	    cc->setStatus( newstatus );
	}
	index++;
    }

    DrawPad();

    return ret;
}

///////////////////////////////////////////////////////////////////
//
// get status of a certain package in list of available packages
//
PMSelectable::UI_Status NCPkgTable::getAvailableStatus ( const PMObjectPtr & objPtr )
{
    return ( statusStrategy->getPackageStatus( objPtr) );
};


///////////////////////////////////////////////////////////////////
//
// fillDefaultList
//
// Fills the package table with the list of default rpm group
// or show the complete patch list in YOU mode
//
bool NCPkgTable::fillDefaultList( )
{
    switch ( tableType )
    {
	case T_Patches: {
	    packager->fillPatchList( "all" );	// default: all patches

	    // set the visible info to long description 
	    packager->setVisibleInfo ( PkgNames::PatchDescr() );
	    // show the package description of the current item
	    packager->showPatchInformation( getDataPointer( getCurrentItem() ) );
	    break;
	}
	case T_Update: {
	    if ( !Y2PM::packageManager().updateEmpty() )
	    {
		packager->fillUpdateList();
		// set the visible info to package description 
		packager->setVisibleInfo ( PkgNames::PkgInfo() );
		// show the package description of the current item
		packager->showPackageInformation( getDataPointer( getCurrentItem() ) );	
		break;
	    }
	}
	case T_Packages: {
	    YStringTreeItem * defaultGroup =  packager->getDefaultRpmGroup();

	    if ( defaultGroup )
	    {
		packager->fillPackageList ( YCPString( defaultGroup->value().translation()),
					    defaultGroup );
		
		// set the visible info to package description 
		packager->setVisibleInfo ( PkgNames::PkgInfo() );
		// show the package description of the current item
		packager->showPackageInformation( getDataPointer( getCurrentItem() ) );
	    }
	    else
	    {
		NCERR << "No default RPM group availbale" << endl;
	    }
	    break;
	}
	default:
	    break;
    }
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillHeader
//
// Fillup the column headers of the package table 
//
void NCPkgTable::fillHeader( )
{
    vector<string> header;

    switch ( tableType )
    {
	case T_Packages:
	case T_Update:
	case T_PatchPkgs:    {
	    int installedPkgs = Y2PM::instTarget().numPackages();
	    if ( installedPkgs > 0 )
	    {
		header.reserve(7);
		header.push_back( PkgNames::PkgStatus().str() );
		header.push_back( PkgNames::PkgName().str() );
		header.push_back( PkgNames::PkgVersionNew().str() );
		header.push_back( PkgNames::PkgVersionInst().str() );
		header.push_back( PkgNames::PkgSummary().str() );
		header.push_back( PkgNames::PkgSize().str() );
	    }
	    else
	    {
		header.reserve(6);
		header.push_back( PkgNames::PkgStatus().str() );
		header.push_back( PkgNames::PkgName().str() );
		header.push_back( PkgNames::PkgVersion().str() );
		header.push_back( PkgNames::PkgSummary().str() );
		header.push_back( PkgNames::PkgSize().str() );	
	    }
	    break;
	}
	case T_Patches: {
	    header.reserve(6);
	    header.push_back( PkgNames::PkgStatus().str() );
	    header.push_back( PkgNames::PkgName().str() );
	    header.push_back( PkgNames::PatchKind().str() );
	    header.push_back( PkgNames::PkgSummary().str() );
	    header.push_back( PkgNames::PkgSize().str() );
	    break;
	}
	default: {
	    header.reserve(5);
	    header.push_back( PkgNames::PkgStatus().str() );
	    header.push_back( PkgNames::PkgName().str() );
	    header.push_back( PkgNames::PkgVersion().str() );
	    header.push_back( PkgNames::PkgSummary().str() );
	    header.push_back( PkgNames::PkgSize().str() );
	    break;
	}
    }
    setHeader( header );
}

///////////////////////////////////////////////////////////////////
//
// createListEntry
//
//
bool NCPkgTable::createListEntry ( PMPackagePtr pkgPtr,
				   unsigned int index )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);

    if ( !pkgPtr || !pkgPtr->hasSelectable() )
    {
	NCERR << "No valid package available" << endl;
	return false;
    }
    
    // add the package name
    pkgLine.push_back( pkgPtr->getSelectable()->name() );	    

    string instVersion = "";
    string version = "";
    PMSelectable::UI_Status status;
    
    switch( tableType )
    {
	case T_PatchPkgs: {
	    if ( pkgPtr->hasInstalledObj() )
	    {
		instVersion = pkgPtr->getInstalledObj()->edition().asString();
	    }
	    
	    // in case of YOU patches: always show the version of the package
	    // which is contained in the patch
	    version = pkgPtr->edition().asString();
   	    pkgLine.push_back( version );

	    if ( Y2PM::instTarget().numPackages() > 0 )
	    {
		pkgLine.push_back( instVersion );	// installed version
	    }
   	    pkgLine.push_back( pkgPtr->summary() );  	// short description
	    
	    status = pkgPtr->getSelectable()->status(); // the package status

	    break;
	}
	case T_Availables: {
	    version = pkgPtr->edition().asString();
	    pkgLine.push_back( version );
	    pkgLine.push_back( pkgPtr->instSrcLabel() ); // show the installation source

	    status = getAvailableStatus( pkgPtr ); // the status of this certain
	                                           // available package
	    break;
	}
	default: {   
	    // if the package is installed, get the installed version 
	    if ( pkgPtr->hasInstalledObj() )
	    {
		instVersion = pkgPtr->getInstalledObj()->version();

                // if a candidate is available, get the candidate version
		if ( pkgPtr->hasCandidateObj() )
		{
		    version = pkgPtr->getCandidateObj()->version();
		}
	    }
	    else
	    {
		version = pkgPtr->version();
	    }
	    pkgLine.push_back( version );	// the available version (the candidate)

	    if ( Y2PM::instTarget().numPackages() > 0 )
	    {
		pkgLine.push_back( instVersion );	// installed version
	    }
	    pkgLine.push_back( pkgPtr->summary() );  	// short description

	    status =  status = pkgPtr->getSelectable()->status(); // the package status
	}
    }

    FSize size = pkgPtr->size();     	// installed size
    pkgLine.push_back( size.asString() );	
    
    addLine( status,	// get the package status
	     pkgLine, 	// the package data
	     index,	// the index
	     pkgPtr );	// the corresponding package pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// createPatchEntry
//
//
bool NCPkgTable::createPatchEntry ( PMYouPatchPtr patchPtr,
				    unsigned int index )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);
    
    if ( !patchPtr )
    {
	NCERR << "No valid patch available" << endl;
	return false;
    }

    pkgLine.push_back( patchPtr->getSelectable()->name() );	 // name
    pkgLine.push_back( patchPtr->kindLabel(patchPtr->kind()) );  // patch kind
    pkgLine.push_back( patchPtr->summary() );  	// short description
    FSize size = patchPtr->size();     		// installed size
    pkgLine.push_back( size.asString() );
    
    addLine( patchPtr->getSelectable()->status(), //  get the status
	     pkgLine,
	     index,		// the index
	     patchPtr );	// the corresponding pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgTable::wHandleInput( int key )
{
    NCursesEvent ret = NCursesEvent::none;
    
    // call handleInput of NCPad
    handleInput( key );
    
    // get current item AFTER handleInput because it may have changed (ScrlUp/Down) 
    int citem  = getCurrentItem();

    switch ( key )
    {
	case KEY_UP:
	case KEY_DOWN: {
	    
	    PMObjectPtr objPtr = getDataPointer(citem);

	    if ( !packager )
		break;
	    
	    switch ( tableType )
	    {
		case T_Packages:
		case T_Update:
		    // show the required package info
		    packager->showPackageInformation( objPtr );   
		    break;
		case T_Patches:
		    // show the patch info
		    packager->showPatchInformation( objPtr );
		    break;
		case T_Dependency:
		    // show the dependencies of this package
		    NCDBG << "GET current item line: " <<  getCurrentItem() << endl;
		    packager->showConcretelyPkgDependency( getCurrentItem() );
		    break;
		case T_SelDependency:
		    // show the dependencies of this selection
		    NCDBG << "GET current item line: " <<  getCurrentItem() << endl;
		    packager->showConcretelySelDependency( getCurrentItem() );
		    break; 
		default:
		    break;
	    }
	    
	    ret = NCursesEvent::handled;
	    break;
	}
	case KEY_SPACE:
	case KEY_RETURN: {
	    // toggle status
	    toggleObjStatus( );

	    ret = NCursesEvent::handled;
	    break;	
	}
	default: {
	    // set the new status
	    changeObjStatus( key );

	    ret = NCursesEvent::handled;
	    break;
	}
    }

    // FIXME: copied from NCTable - needed here ???
    if ( getNotify() && isImmediate() && citem != getCurrentItem() )
	ret = NCursesEvent::button;

    return ret;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::getStatus()
//
// Gets the status of the package of selected line
//
PMSelectable::UI_Status NCPkgTable::getStatus( int index )
{
    // get the tag 
    NCPkgTableTag * cc = getTag( index);
    if ( !cc )
	return PMSelectable::S_NoInst;

    return cc->getStatus();
}

PMObjectPtr NCPkgTable::getDataPointer( int index )
{
    // get the tag 
    NCPkgTableTag *cc = getTag( index );
    if ( !cc )
	return PMObjectPtr( );

    return cc->getDataPointer();
}

NCPkgTableTag * NCPkgTable::getTag( const int & index )
{
    // get the table line 
    NCTableLine * cl = pad->ModifyLine( index );
    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( cl->GetCol( 0 ) );

    return cc;
}
///////////////////////////////////////////////////////////////////
//
// NCPkgTable::toggleSourceStatus()
//
//
bool NCPkgTable::toggleSourceStatus( )
{
    PMObjectPtr objPtr = getDataPointer( getCurrentItem() );
    return true;
}
			       
///////////////////////////////////////////////////////////////////
//
// NCPkgTable::toggleObjStatus()
//
//
bool NCPkgTable::toggleObjStatus( )
{
    PMObjectPtr objPtr = getDataPointer( getCurrentItem() );

    if ( !objPtr )
	return false;
    
    PMSelectable::UI_Status newStatus;
    
    bool ok = statusStrategy->toggleStatus( objPtr, newStatus );

    if ( ok )
    {
	changeStatus( newStatus, objPtr, true );	
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::changeObjStatus()
//
//
bool NCPkgTable::changeObjStatus( int key )
{
    PMObjectPtr objPtr = getDataPointer( getCurrentItem() );

    if ( !objPtr )
    {
	return false; 
    }
    PMSelectable::UI_Status newStatus;

    bool ok = statusStrategy->keyToStatus( key, objPtr, newStatus );
    
    if ( ok )
    {
	changeStatus( newStatus, objPtr, true );
    }
    return true;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::changeListObjStatus()
//
//
bool NCPkgTable::changeListObjStatus( NCPkgTableListAction type )
{
    PMSelectable::UI_Status newStatus;
    PMObjectPtr objPtr;
    unsigned int size = getNumLines();
    unsigned int index = 0;

    while ( index < size )
    {
	// get the object pointer
	objPtr = getDataPointer( index );
	bool ok = false;
	
	if ( objPtr )
	{
	    switch ( type ) {
		case A_Install: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_NoInst ) 
			ok = statusStrategy->keyToStatus( '+', objPtr, newStatus );
		    break;
		}
		case A_DontInstall: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_Install
			 ||  objPtr->getSelectable()->status() == PMSelectable::S_AutoInstall )
			ok = statusStrategy->keyToStatus( '-', objPtr, newStatus );
		    break;
		}
		case A_Delete: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_KeepInstalled )
			ok = statusStrategy->keyToStatus( '-', objPtr, newStatus );
		    break;
		}
		case A_DontDelete: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_Del
			 || objPtr->getSelectable()->status() == PMSelectable::S_AutoDel )
			ok = statusStrategy->keyToStatus( '+', objPtr, newStatus );
		    break;
		}
		case A_Update: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_KeepInstalled )
			ok = statusStrategy->keyToStatus( '>', objPtr, newStatus );
		    break;
		}
		case A_DontUpdate: {
		    if ( objPtr->getSelectable()->status() == PMSelectable::S_Update
			 || objPtr->getSelectable()->status() == PMSelectable::S_AutoUpdate )
			ok = statusStrategy->keyToStatus( '-', objPtr, newStatus );
		    break;
		}
		default:
		    NCERR << "Unknown list action" << endl;
	    }

	    if ( ok )
	    {
		changeStatus( newStatus,
			      objPtr,
			      false );	// do not do the updates with every change
	    }
	}

	index++;
    }

    // do the updates now
    updateTable();
    packager->showPackageDependencies( false );
    packager->showDiskSpace();

    return true;
}





