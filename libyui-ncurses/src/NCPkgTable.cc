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

#include "PackageSelector.h"
#include <y2pm/PMSelectable.h>


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
	    return "----";
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
    DrawPad();
  
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
bool NCPkgTable::changeStatus( PMSelectable::UI_Status newstatus )
{
    if ( !packager )
	return false;
    
    bool ok = false;

    int index  = getCurrentItem();
    
    // inform the package manager
    ok = statusStrategy->setPackageStatus( newstatus,
					   getDataPointer(index) );

    if ( ok )
    {
	// update this list to show the status changes
	updateTable();
	
	if ( tableType == T_Availables )
	{
	    // additionally update the package list
	    packager->updatePackageList();
	}
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

	if ( !cc || !objPtr )
	{
	    ret = false;
	    break;
	}

	// set the new status (if status has changed) - use particular strategy
	PMSelectable::UI_Status newstatus = statusStrategy->getPackageStatus( objPtr);
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
PMSelectable::UI_Status NCPkgTable::getAvailableStatus ( PMObjectPtr objPtr )
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
	case T_Packages:
	case T_Update: {
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
	case T_Patches: {
	    packager->fillPatchList( "all" );	// default: all patches

	    // set the visible info to long description 
	    packager->setVisibleInfo ( PkgNames::PatchDescr() );
	    // show the package description of the current item
	    packager->showPatchInformation( getDataPointer( getCurrentItem() ) );
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
    vector<NCstring> header;

    switch ( tableType )
    {
	case T_Packages:
	case T_Availables:
	case T_Update:	       {
	    int installedPkgs = Y2PM::instTarget().numPackages();
	    if ( installedPkgs > 0 )
	    {
		header.reserve(7);
		header.push_back( PkgNames::PkgStatus() );
		header.push_back( PkgNames::PkgName() );
		header.push_back( PkgNames::PkgVersionNew() );
		header.push_back( PkgNames::PkgVersionInst() );
		header.push_back( PkgNames::PkgSummary() );
		header.push_back( PkgNames::PkgSize() );
	    }
	    else
	    {
		header.reserve(6);
		header.push_back( PkgNames::PkgStatus() );
		header.push_back( PkgNames::PkgName() );
		header.push_back( PkgNames::PkgVersion() );
		header.push_back( PkgNames::PkgSummary() );
		header.push_back( PkgNames::PkgSize() );	
	    }
	    break;
	}
	case T_Patches: {
	    header.reserve(6);
	    header.push_back( PkgNames::PkgStatus() );
	    header.push_back( PkgNames::PkgName() );
	    header.push_back( PkgNames::PatchKind() );
	    header.push_back( PkgNames::PkgSummary() );
	    header.push_back( PkgNames::PkgSize() );
	    break;
	}
	default: {
	    header.reserve(5);
	    header.push_back( PkgNames::PkgStatus() );
	    header.push_back( PkgNames::PkgName() );
	    header.push_back( PkgNames::PkgVersion() );
	    header.push_back( PkgNames::PkgSummary() );
	    header.push_back( PkgNames::PkgSize() );
	    break;
	}
    }
    setHeader( header );
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
    PMSelectable::UI_Status newStat = PMSelectable::S_NoInst;
    
    // call handleInput of NCPad
    handleInput( key );
    
    // get current item AFTER handleInput because it may have changed (ScrlUp/Down) 
    int citem  = getCurrentItem();

    switch ( key )
    {
	case KEY_SPACE:
	case KEY_RETURN:
	    // if ( getNotify() )
	    // ret = NCursesEvent::button;
	    break;
	case KEY_UP:
	case KEY_DOWN: {
	    
	    PMObjectPtr objPtr = getDataPointer(citem);

	    if ( !objPtr || !packager )
		break;
	    
	    switch ( tableType )
	    {
		case T_Packages:
		    // show the required package info
		    packager->showPackageInformation( objPtr );   
		    break;
		case T_Patches:
		    // show the patch info
		    packager->showPatchInformation( objPtr );
		    break;
		case T_Dependency:
		    // show the dependencies of this package
		    packager->showConcretelyDependency( objPtr );
		    break;
		default:
		    break;
	    }
	    
	    ret = NCursesEvent::handled;
	    break;
	}
	case KEY_F(3): {
	    // set the new status
	    toggleObjStatus( );

	    ret = NCursesEvent::handled;
	    break;	
	}
	default: {
	    // set the new status
	    changeObjStatus( key );
#if 0
	    bool ok = statusStrategy->keyToStatus( key, getDataPointer(citem), newStat );
	    if ( ok )
	    {
		changeStatus( newStat );
	    }
#endif
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
    PMSelectable::UI_Status newStatus;
    
    bool ok = statusStrategy->toggleStatus( objPtr, newStatus );

    if ( ok )
    {
	changeStatus( newStatus );	
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
    PMSelectable::UI_Status newStatus;
    bool ok = statusStrategy->keyToStatus( key, objPtr, newStatus );
    
    if ( ok )
    {
	changeStatus( newStatus );
    }
    return true;
}







