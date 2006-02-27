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
#include "NCi18n.h"

#include "PackageSelector.h"
#include <zypp/ui/Selectable.h>
#include "YQZypp.h"
//#include <y2pm/InstTarget.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgTableTag::NCPkgTableTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgTableTag::NCPkgTableTag( ZyppObj objPtr, ZyppSel selPtr,
			      ZyppStatus stat )
      : NCTableCol( NCstring( "    " ), SEPARATOR )
	, status ( stat )
	, dataPointer( objPtr )
	, selPointer( selPtr )
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


string NCPkgTableTag::statusToStr( ZyppStatus stat ) const
{
     // convert ZyppStatus to string
    switch ( stat )
    {
	case S_NoInst:	// Is not installed and will not be installed
	    return "    ";
	case S_KeepInstalled: 	// Is installed - keep this version
	    return "  i ";
	case S_Install:	// Will be installed
	    return "  + ";
	case S_Del:	// Will be deleted
	    return "  - ";
	case S_Update:	// Will be updated
	    return "  > ";
	case S_AutoInstall: // Will be automatically installed
	    return " a+ ";
	case S_AutoDel:	// Will be automatically deleted
	    return " a- ";
	case S_AutoUpdate: // Will be automatically updated
	    return " a> ";    
	case S_Taboo:	// Never install this 
	    return " ---";
	case S_Protected:	// always keep installed version 
	    return " -i-";
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
NCPkgTable::NCPkgTable( NCWidget * parent, const YWidgetOpt & opt )
    : NCTable( parent, opt, vector<string> (), false )
      , packager ( 0 )
      , statusStrategy( new PackageStatStrategy )	// default strategy: packages
      , tableType ( T_Packages )			// default type: packages
      , haveInstalledVersion ( false )
{
    fillHeader();
    
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
void NCPkgTable::addLine( ZyppStatus stat,
			  const vector<string> & elements,
			  ZyppObj objPtr,
			  ZyppSel slbPtr )
{
    vector<NCTableCol*> Items( elements.size()+1, 0 );
    
    // fill first column (containing the status information and the package pointers)
    Items[0] = new NCPkgTableTag( objPtr, slbPtr, stat );

    for ( unsigned i = 1; i < elements.size()+1; ++i ) {
	// use YCPString to enforce recoding from 'utf8'
	Items[i] = new NCTableCol( YCPString( elements[i-1] ) );
    }
    myPad()->Append( Items );
    
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
bool NCPkgTable::changeStatus( ZyppStatus newstatus,
			       const ZyppSel & slbPtr,
    // objPtr is candidatePtr or what the user selected instead of it.
			       ZyppObj objPtr,
			       bool singleChange )
{
    if ( !packager || !slbPtr )
	return false;

    zypp::Text notify;
    zypp::License license;
    bool license_confirmed = true;
    ZyppPkg pkgPtr = NULL;
    YCPString header( "" );
    bool ok = true;

    switch ( newstatus )
    {
	case S_Del:
	case S_NoInst:
	case S_Taboo:
	    if ( objPtr )
	    {
		notify = objPtr->delnotify();
		header = YCPString(PkgNames::WarningLabel());
	    }
	break;
	case S_Install:
	case S_Update:
	    if ( objPtr )
	    {	
		notify = objPtr->insnotify();
		header = YCPString(PkgNames::NotifyLabel());

		// get license (available for packages only)  
		pkgPtr = tryCastToZyppPkg (objPtr);
		if ( pkgPtr )
		{
		    license = pkgPtr->licenseToConfirm();
#ifdef FIXME_LIC
		    license_confirmed = slbPtr->isLicenceConfirmed();
#endif
		}
	    }
	    break;

	default: break;
    }

    string pkgName = slbPtr->name();

    if ( !license.empty() )
    {
	if (!license_confirmed) {
	    NCPopupInfo info( wpos( 1, 1),
			  YCPString(_("End User License Agreement") ),
			  YCPString( "<i>" + pkgName + "</i><br><br>" + packager->createDescrText( license ) ),
			  PkgNames::AcceptLabel(),
			  PkgNames::CancelLabel() );
	    license_confirmed = info.showInfoPopup( ) != NCursesEvent::cancel;
	}

	if ( !license_confirmed )
	{
	    // make sure the package won't be installed
	    switch ( newstatus )
	    {
		case S_Install:
		    newstatus = S_Taboo;
		    break;
		    
		case S_Update:
		    newstatus = S_Protected;
		    break;

		default:
		    break;
	    }
	    
	    ok = false;
	} else {
#ifdef FIXME_LIC
	    slbPtr->setLicenseConfirmed (true);
#endif
	}
    }

    if ( ok && !notify.empty() )
    {
	NCPopupInfo info( wpos( 1, 1),
			  header,
			  YCPString( "<i>" + pkgName + "</i><br><br>" + packager->createDescrText( notify ) ) );
	info.showInfoPopup( );
    }
    
    // inform the package manager
    ok = statusStrategy->setObjectStatus( newstatus, slbPtr, objPtr );
    
    if ( ok && singleChange )
    {
	switch ( tableType )
	{
	    case T_Packages:
	    case T_Update:
	    case T_Availables:
		// check/show dependencies of packages
		packager->showPackageDependencies( false );	// only check if automatic check is ON
		// show the required diskspace
		packager->showDiskSpace();
		break;
		
	    case T_Selections:
		// check/show dependencies of selections 
		packager->showSelectionDependencies();
		break;
	    
	    case T_Patches:
#ifdef FIXME
		// show the download size for all selected patches
		packager->showDownloadSize();
#endif
		break;
		
	    default:
		break;
	}
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
	NCTableLine * cl = myPad()->ModifyLine( index );
	if ( !cl )
	{
	    ret = false;
	    break;
	}

        // get first column (the column containing the status info)
	NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( cl->GetCol( 0 ) );
	// get the object pointer
	ZyppSel slbPtr = getSelPointer( index );
	ZyppObj objPtr = getDataPointer( index );

	if ( !cc )
	{
	    ret = false;
	    break;
	}
	
	ZyppStatus newstatus = S_NoInst;
	if ( slbPtr && objPtr)
	{
	    // get the new status - use particular strategy
	    newstatus = statusStrategy->getPackageStatus( slbPtr, objPtr );
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
ZyppStatus NCPkgTable::getAvailableStatus ( const ZyppSel & slbPtr,
					    const ZyppObj & objPtr )
{
    return ( statusStrategy->getPackageStatus( slbPtr, objPtr) );
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
#ifdef FIXME
	case T_Patches: {
	    packager->fillPatchList( "installable" );	// default: installable patches

	    // set the visible info to long description 
	    packager->setVisibleInfo ( PkgNames::PatchDescr() );
	    // show the package description of the current item
	    showInformation ();
	    break;
	}
#endif
	case T_Update: {
#ifdef FIXME
	    if ( !Y2PM::packageManager().updateEmpty() )
#endif
	    {
		packager->fillUpdateList();
		// set the visible info to package description 
		packager->setVisibleInfo ( PkgNames::PkgInfo() );
		// show the package description of the current item
		showInformation ();
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
		showInformation ();
	    }
	    else
	    {
		NCERR << "No default RPM group available" << endl;
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
// slbHasInstalledObj
//
// a helper to call a method
//
static bool slbHasInstalledObj (const ZyppSel & slb)
{
    return slb->hasInstalledObj ();
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
	case T_Update: {
	    bool haveInstalledPkgs = find_if (zyppPkgBegin (), zyppPkgEnd (),
					      slbHasInstalledObj) != zyppPkgEnd ();

	    header.reserve(7);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::PkgName() );
	    if ( haveInstalledPkgs > 0 )
	    {
		header.push_back( "L" + PkgNames::PkgVersionNew() );
		header.push_back( "L" + PkgNames::PkgVersionInst() );
		haveInstalledVersion = true;
	    }
	    else
	    {
		header.push_back( "L" + PkgNames::PkgVersion() );
	    }
	    header.push_back( "L" + PkgNames::PkgSummary() );
	    header.push_back( "L" + PkgNames::PkgSize() );
	    header.push_back( "L" + PkgNames::PkgSource() );	
	    break;
	}
	case T_PatchPkgs: {
	    header.reserve(7);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::PkgName() );
	    header.push_back( "L" + PkgNames::PkgVersionNew() );
	    header.push_back( "L" + PkgNames::PkgVersionInst() );
	    header.push_back( "L" + PkgNames::PkgSummary() );
	    header.push_back( "L" + PkgNames::PkgSize() );
	    break;
	}
	case T_Patches: {
	    header.reserve(6);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::PkgName() );
	    header.push_back( "L" + PkgNames::PatchKind() );
	    header.push_back( "L" + PkgNames::PkgSummary() );
	    header.push_back( "L" + PkgNames::PkgSize() );
	    break;
	}
	case T_Selections: {
	    header.reserve(3);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::SelectionName() );
	    break;
	}
	case T_Availables: {
	    header.reserve(6);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::PkgName() );
	    header.push_back( "L" + PkgNames::PkgVersion() );
	    header.push_back( "L" + PkgNames::PkgInstSource() );
	    header.push_back( "L" + PkgNames::PkgSize() );
	    break;
	}
	default: {
	    header.reserve(4);
	    header.push_back( "L" + PkgNames::PkgStatus() );
	    header.push_back( "L" + PkgNames::PkgName() );
	    header.push_back( "L" + PkgNames::PkgSummary() );
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
bool NCPkgTable::createListEntry ( ZyppPkg pkgPtr, ZyppSel slbPtr )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);

    if ( !pkgPtr || !slbPtr )
    {
	NCERR << "No valid package available" << endl;
	return false;
    }
    
    // add the package name
    pkgLine.push_back( slbPtr->name() );	    

    string instVersion = "";
    string version = "";
    ZyppStatus status;
    
    switch( tableType )
    {
#ifdef FIXME
	case T_PatchPkgs: {
	    if ( pkgPtr->hasInstalledObj() )
	    {
		instVersion = pkgPtr->getInstalledObj()->edition().asString();
	    }
	    
	    // in case of YOU patches: show the version of the package which
	    // is contained in the patch
	    version = pkgPtr->edition().asString();
   	    pkgLine.push_back( version );

	    // if ( Y2PM::instTarget().numPackages() > 0 )
	    // doesn't make sense for YOU mode because there are always installed packages
	    // -> show installed version or empty column
	    pkgLine.push_back( instVersion );

   	    pkgLine.push_back( pkgPtr->summary() );  	// short description
	    
	    status = pkgPtr->getSelectable()->status(); // the package status
	    
	    FSize size = pkgPtr->size();     	// installed size
	    pkgLine.push_back( size.form( 8 ) );  // format size

	    break;
	}
#endif
	case T_Availables: {
	    version = pkgPtr->edition().asString();
	    pkgLine.push_back( version );
	    // is alias the right string? id?
	    pkgLine.push_back( pkgPtr->source().alias() ); // show the installation source

	    status = getAvailableStatus( slbPtr, pkgPtr ); // the status of this certain package
	    
	    zypp::ByteCount size = pkgPtr->size();     	// installed size
	    pkgLine.push_back( size.asString( 8 ) );  // format size

	    break;
	}
	default: {
	    // version() was edition.version. but what about edition.release?

	    // if the package is installed, get the installed version 
	    if ( slbPtr->hasInstalledObj() )
	    {
		instVersion = slbPtr->installedObj()->edition().version();

                // if a candidate is available, get the candidate version
		if ( slbPtr->hasCandidateObj() )
		{
		    version = slbPtr->candidateObj()->edition().version();
		}
	    }
	    else
	    {
		version = pkgPtr->edition().version();
	    }
	    pkgLine.push_back( version );	// the available version (the candidate)

	    if ( haveInstalledVersion )
	    {
		pkgLine.push_back( instVersion );	// installed version
	    }
	    pkgLine.push_back( pkgPtr->summary() );  	// short description
	    
	    status = slbPtr->status(); // the package status

	    zypp::ByteCount size = pkgPtr->size(); // installed size
	    pkgLine.push_back( size.asString( 8 ) );  	// format size

// Selectable does not have source_install
#ifdef FIXME
	    if ( slbPtr->source_install() )
	    {
		pkgLine.push_back( " x " );	
	    }
	    else
#endif
	    {
		pkgLine.push_back( "   " );	
	    }
	}
    }
    
    addLine( status,	// the package status
	     pkgLine, 	// the package data
	     pkgPtr,	// the corresponding package pointer
	     slbPtr
	);

    return true;
}

///////////////////////////////////////////////////////////////////
//
// createInfoEntry
//
//
bool NCPkgTable::createInfoEntry ( string text )
{
    vector<string> pkgLine;
    pkgLine.reserve(2);

    pkgLine.push_back( text );
    addLine( S_NoInst,	// use status NoInst
	     pkgLine,
	     ZyppObj(),
	     ZyppSel());	// null pointer
    
    return true;
}

#ifdef FIXME
///////////////////////////////////////////////////////////////////
//
// createPatchEntry
//
//
bool NCPkgTable::createPatchEntry ( ZyppPatch patchPtr )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);
    
    if ( !patchPtr || !patchPtr->hasSelectable() )
    {
	NCERR << "No valid patch available" << endl;
	return false;
    }

    pkgLine.push_back( patchPtr->getSelectable()->name() );	 // name
    pkgLine.push_back( patchPtr->kindLabel() ); // patch kind
    pkgLine.push_back( patchPtr->summary() );  	// short description
    FSize size = patchPtr->size();     		// installed size
    pkgLine.push_back( size.form( 8 ) );
    
    addLine( patchPtr->getSelectable()->status(), //  get the status
	     pkgLine,
	     patchPtr );	// the corresponding pointer

    return true;
}
#endif

///////////////////////////////////////////////////////////////////
//
// showInformation
//
//
bool NCPkgTable::showInformation ( )
{
    ZyppObj objPtr = getDataPointer( getCurrentItem() );
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );

    if ( !packager )
	return false;
	    
    switch ( tableType )
    {
	case T_Packages:
	case T_Update:
	    // show the required package info
	    packager->showPackageInformation( objPtr, slbPtr );
	    break;
	case T_Patches:
#ifdef FIXME
	    // show the patch info
	    packager->showPatchInformation( objPtr, slbPtr );
#endif
	    break;
	default:
	    break;
    }
    
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
NCursesEvent NCPkgTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;
    
    // call handleInput of NCPad
    handleInput( key );
    
    switch ( key )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME: {
	    // show the corresponding information
	    showInformation( );
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

    return ret;
}

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::getStatus()
//
// Gets the status of the package of selected line
//
ZyppStatus NCPkgTable::getStatus( int index )
{
    // get the tag 
    NCPkgTableTag * cc = getTag( index);
    if ( !cc )
	return S_NoInst;

    return cc->getStatus();
}

ZyppObj NCPkgTable::getDataPointer( int index )
{
    // get the tag 
    NCPkgTableTag *cc = getTag( index );
    if ( !cc )
	return ZyppObj( );

    return cc->getDataPointer();
}

ZyppSel NCPkgTable::getSelPointer( int index )
{
    // get the tag 
    NCPkgTableTag *cc = getTag( index );
    if ( !cc )
	return ZyppSel( );

    return cc->getSelPointer();
}

NCPkgTableTag * NCPkgTable::getTag( const int & index )
{
    // get the table line 
    NCTableLine * cl = myPad()->ModifyLine( index );
    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( cl->GetCol( 0 ) );

    return cc;
}

#ifdef FIXME
///////////////////////////////////////////////////////////////////
//
// NCPkgTable::SourceInstall()
//
//
bool NCPkgTable::SourceInstall( bool install )
{
    int index =  getCurrentItem();
    ZyppObj objPtr = getDataPointer( index );
    bool ok;
    
    if ( !objPtr )
    {
	NCERR << "Invalid Pointer" << endl;
	return false;
    }
    ZyppSel selPtr = objPtr->getSelectable();
    NCTableLine * currentLine = myPad()->ModifyLine( index );
    
    if ( !selPtr  || !currentLine )
    {
	NCERR << "Invalid Selectable" << endl;
	return false;
    }

    NCTableCol * currentCol = currentLine->GetCol( currentLine->Cols()-1 );    

    if ( install && selPtr->providesSources() )
    {
	ok = selPtr->set_source_install( true );
	NCMIL << "Set source install returns: " << (ok?"true":"false") << endl;
	if ( currentCol )
	    currentCol->SetLabel( NClabel( " x " ) );
    }
    else if ( !install && selPtr->source_install() )
    {
	ok = selPtr->set_source_install( false );
	NCMIL << "ReSet source install returns: " << (ok?"true":"false") << endl;
	if ( currentCol )
	    currentCol->SetLabel( NClabel( "   " ) );
    }
	
    return true;
}
#endif

///////////////////////////////////////////////////////////////////
//
// NCPkgTable::toggleObjStatus()
//
//
bool NCPkgTable::toggleObjStatus( )
{
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );
    ZyppObj objPtr = getDataPointer( getCurrentItem() );

    if ( !slbPtr )
	return false;
    
    ZyppStatus newStatus;
    
    bool ok = statusStrategy->toggleStatus( slbPtr, objPtr, newStatus );

    if ( ok )
    {
	changeStatus( newStatus, slbPtr, objPtr, true );
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
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );
    ZyppObj objPtr = getDataPointer( getCurrentItem() );

    if ( !slbPtr )
    {
	return false; 
    }
    ZyppStatus newStatus;

    bool ok = statusStrategy->keyToStatus( key, slbPtr, objPtr, newStatus );
    
    if ( ok )
    {
	changeStatus( newStatus, slbPtr, objPtr, true );
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
    ZyppStatus newStatus;
    unsigned int size = getNumLines();
    unsigned int index = 0;

    while ( index < size )
    {
	// get the object pointer
	ZyppSel slbPtr = getSelPointer( index );
	ZyppObj objPtr = getDataPointer( index );
	bool ok = false;
	
	if ( slbPtr )
	{
	    switch ( type ) {
		case A_Install: {
		    if ( slbPtr->status() == S_NoInst ) 
			ok = statusStrategy->keyToStatus( '+', slbPtr, objPtr, newStatus );
		    break;
		}
		case A_DontInstall: {
		    if ( slbPtr->status() == S_Install
			 ||  slbPtr->status() == S_AutoInstall )
			ok = statusStrategy->keyToStatus( '-', slbPtr, objPtr, newStatus );
		    break;
		}
		case A_Delete: {
		    if ( slbPtr->status() == S_KeepInstalled )
			ok = statusStrategy->keyToStatus( '-', slbPtr, objPtr, newStatus );
		    break;
		}
		case A_DontDelete: {
		    if ( slbPtr->status() == S_Del
			 || slbPtr->status() == S_AutoDel )
			ok = statusStrategy->keyToStatus( '+', slbPtr, objPtr, newStatus );
		    break;
		}
		case A_Update: {
		    if ( slbPtr->status() == S_KeepInstalled )
			ok = statusStrategy->keyToStatus( '>', slbPtr, objPtr, newStatus );
		    break;
		}
		case A_DontUpdate: {
		    if ( slbPtr->status() == S_Update
			 || slbPtr->status() == S_AutoUpdate )
			ok = statusStrategy->keyToStatus( '-', slbPtr, objPtr, newStatus );
		    break;
		}
		default:
		    NCERR << "Unknown list action" << endl;
	    }

	    if ( ok )
	    {
		changeStatus( newStatus,
			      slbPtr,
			      objPtr,
			      false );	// do not do the updates with every change
	    }
	}

	index++;
    }

    // do the updates now
    packager->showPackageDependencies( false );
    packager->showDiskSpace();

    updateTable();

    return true;
}
