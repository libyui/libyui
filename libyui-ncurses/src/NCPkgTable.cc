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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTableTag::NCPkgTableTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgTableTag::NCPkgTableTag( PMObjectPtr pkgPtr, NCPkgStatus stat )
      : NCTableCol( NCstring( "   " ), SEPARATOR )
	, status ( stat )
	, dataPointer( pkgPtr )
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
    w.addch( at.Pos.L, at.Pos.C +1, statusStr.c_str()[0] );
}


string NCPkgTableTag::statusToStr( NCPkgStatus stat ) const
{
     // convert NCPkgStatus to string
    switch ( stat )
    {
	case PkgNoInstall:	// PkgNoInst Is not installed and will not be installed
	    return " ";
	case PkgInstalled: 	// PkgKeepInstalled Is installed - keep this version
	    return "i";
	case PkgToInstall:	// PkgInstall ??Is?? or will be installed
	    return "X";
	case PkgToDelete:	// PkgDel    Will be deleted
	    return "d";
	case PkgToUpdate:	// PkgUpdate Will be updated
	    return "u";
	case PkgToReplace:	// ?????????
	    return "X";
	case PkgAutoInstall:	// PkgAuto   Will be automatically installed
	    return "a";
	case PkgAutoDelete:	// -------------
	    return "-";
	case PkgTaboo:		// PkgTaboo  Never install this 
	    return "#";
    }

    return " ";
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
void NCPkgTable::addLine( NCPkgStatus status, vector<string> elements, int index, PMObjectPtr pkgPtr )
{
    vector<NCTableCol*> Items( elements.size()+1, 0 );

    // fill first column (containing the status information and the package pointer)
    Items[0] = new NCPkgTableTag( pkgPtr, status );
    
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
bool NCPkgTable::changeStatus( int index, NCPkgStatus newstatus )
{
    bool ok = false;

    // get the table line 
    NCTableLine * cl = pad->ModifyLine( index );
    if ( !cl )
	return false;
    
    // get first column (the column containing the status info)
    NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( cl->GetCol( 0 ) );
    
    // set new status
    cc->setStatus( newstatus );
    // redraw the table
    DrawPad();

    // inform the package manager
    ok = packager->setPackageStatus( index, newstatus );

    return ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTable::getCellContents
//	METHOD TYPE : void
//
//	DESCRIPTION : returns the NClabel of the specified column
//
NClabel NCPkgTable::getCellContents( int index, int colnum )
{
    NClabel ret ( "" );
    
    const NCTableLine * cl = pad->GetLine( index );
    if ( !cl )
    {
	NCINT << "No such line: " << wpos( index, colnum ) << endl;
    }
    else
    {
	const NCTableCol * cc = cl->GetCol( colnum );
	if ( !cc )
	{
	    NCINT << "No such colnum: " << wpos( index, colnum ) << endl;
	}
	else
	{
	    ret = cc->Label();
	}
    }

    return ret;
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
	case KEY_SPACE:
	case KEY_RETURN:
	    if ( getNotify() )
		ret = NCursesEvent::button;
	    break;
	case KEY_UP:
	case KEY_DOWN: {
	    // debug only
	    NClabel packageName = getCellContents( citem, 1 );
	    UIDBG << "Package Name: " << packageName << endl;

	    // show the required package info
	    packager->showPackageInformation( getDataPointer(citem) );
	    ret = NCursesEvent::handled;
	    break;
	}
	case KEY_F(4): {
	    // set the new status
	    toggleStatus( getDataPointer(citem) );

	    ret = NCursesEvent::handled;
	    break;	
	}
	default: {
	    // set the new status
	    setNewStatus( keyToStatus(key) );
	    
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
NCPkgStatus NCPkgTable::getStatus( int index )
{
    // get the tag 
    NCPkgTableTag * cc = getTag( index);
    if ( !cc )
	return PkgNoInstall;

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
// NCPkgTable::toggleStatus()
//
// Returns the new status
//
bool NCPkgTable::toggleStatus( PMPackagePtr pkgPtr )
{
    bool ok = false;
    
    int citem = getCurrentItem();
    NCPkgStatus newStatus = getStatus( citem ); 

    switch ( getStatus( citem ) )
    {
	case PkgToDelete:
	    newStatus = PkgInstalled;
	    break;
	case PkgToInstall:
	    newStatus = PkgNoInstall;
	    break;
	case PkgToUpdate:
	    newStatus = PkgToDelete;
	    break;
	case PkgInstalled:
	    if ( pkgPtr->hasCandidateObj() )
	    {
		newStatus = PkgToUpdate;
	    }
	    else
	    {
		newStatus = PkgToDelete;
	    }
	    break;
	case PkgToReplace: 
	    newStatus = PkgToDelete;
	    break;
	case PkgNoInstall:
	    newStatus = PkgToInstall;
	    break;
	case PkgAutoInstall:
	    newStatus = PkgNoInstall;
	    break;
	case PkgAutoDelete:
	    newStatus = PkgInstalled;
	    break;
	case PkgTaboo:
	    newStatus = PkgTaboo;
	    break;
    }

    // show the new status and inform the packagemanager
    ok = changeStatus( citem, newStatus );

    return ok;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::keyToStatus()
//
// Returns the corresponding status
//
NCPkgStatus NCPkgTable::keyToStatus( const int & key )
{
    // get the new status
    switch ( key )
    {
	case '-':
	case 'd':
	case KEY_F(5):
	    return PkgToDelete;
	case 's':
	case '+':
	case 'x':
	case KEY_F(3):
	    return PkgToInstall;
	case 'u':
	    return PkgToUpdate;
	case 'i':
	    return PkgInstalled;
	case 'r':
	    return PkgToReplace;
	case 'l':
	    return PkgNoInstall;
	default:
	    y2warning( "Key not valid - returning current status" );
	    return getStatus( getCurrentItem() );
    }
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::setNewStatus()
//
// Checks whether the new status is valid and - if yes - sets the status 
//
bool NCPkgTable::setNewStatus( const NCPkgStatus & newStatus  )
{
    bool ok = false;
    bool valid = false;
    
    int citem = getCurrentItem();
    PMPackagePtr pkgPtr = getDataPointer( getCurrentItem() );
    
    // check whether the status change is possible
    switch ( getStatus( citem ) )
    {
	case PkgToDelete: {
	    if ( newStatus == PkgInstalled || newStatus == PkgToUpdate ||
		 newStatus == PkgToReplace )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToInstall: {
	    if ( newStatus == PkgNoInstall )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToUpdate: {
	    if ( newStatus == PkgToDelete ||  newStatus == PkgInstalled ||
		 newStatus == PkgToReplace )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgInstalled: {
	    if ( ( newStatus == PkgToDelete ) 	||
		 ( pkgPtr->hasCandidateObj() &&
		   ( newStatus == PkgToUpdate
		     || newStatus == PkgToReplace
		     || newStatus == PkgToDelete ) ) )
	    {
		valid = true;
	    }
	    break;
	}
	case PkgToReplace: {
	   if ( newStatus == PkgToDelete || newStatus == PkgInstalled )
	    {
		valid = true;
	    }
	    break;  
	}
	case PkgNoInstall:
	    if ( newStatus == PkgToInstall )
	    {
		valid = true;
	    }
	    break;
 	case PkgAutoInstall:
	    if ( newStatus == PkgNoInstall )
	    {
		valid = true;
	    }
	    break;
	case PkgAutoDelete:
	    if ( newStatus == PkgInstalled )
	    {
		valid = true;
	    }
	    break;
	case PkgTaboo:
	    if ( newStatus == PkgTaboo )
	    {
		valid = true;
	    }
	    break;
    }

    NCMIL << "Change from: " << getStatus( citem ) << " to "
	  << newStatus << ": " << (valid?"true":"false") << endl;

    if ( valid )
    {
	// show the new status and inform the package manager
	ok = changeStatus( citem, newStatus );
    }
    
    return ok;
}
