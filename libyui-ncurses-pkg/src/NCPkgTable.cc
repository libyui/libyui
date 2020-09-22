/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>
#include <YDialog.h>
#include <boost/format.hpp>

#include "NCurses.h"
#include "NCPkgTable.h"
#include "NCTable.h"
#include "NCPopupInfo.h"
#include "NCPkgStrings.h"
#include "NCi18n.h"
#include "NCPkgPopupDiskspace.h"
#include "NCPackageSelector.h"
#include <zypp/ui/Selectable.h>
#include "NCZypp.h"

#define SOURCE_INSTALL_SUPPORTED        0

using std::string;
using std::vector;
using std::endl;

/*
  Textdomain "ncurses-pkg"
*/


NCPkgTableTag::NCPkgTableTag( ZyppObj objPtr, ZyppSel selPtr, ZyppStatus stat )
    : YTableCell( "    " )
    , status ( stat )
    , dataPointer( objPtr )
    , selPointer( selPtr )
{
    setLabel( statusToString(stat) );
}


string NCPkgTableTag::statusToString( ZyppStatus stat ) const
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

	case S_Protected: // Always keep installed version
	    return " -i-";

	default:
	    return "####";
    }

    return "    ";
}


NCPkgTable::NCPkgTable( YWidget * parent, YTableHeader * tableHeader )
    : NCTable( parent, tableHeader )
    , packager( 0 )
    , statusStrategy( new PackageStatStrategy )	// default strategy: packages
    , tableType( T_Packages )                   // default type: packages
    , haveInstalledVersion( false )
    , visibleInfo( I_Technical )
{
    yuiDebug() << "NCPkgTable created" << endl;
}


NCPkgTable::~NCPkgTable()
{
    delete statusStrategy;
    yuiDebug() << endl;
}


void NCPkgTable::addLine( ZyppStatus stat,
			  const vector<string> & elements,
			  ZyppObj objPtr,
			  ZyppSel slbPtr )
{
    YTableItem *tabItem = new YTableItem();

    // fill first column (containing the status information and the package pointers)
    tabItem->addCell( new NCPkgTableTag( objPtr, slbPtr, stat ));

    for ( const string& s: elements )
	tabItem->addCell( s );

    // use all-at-once insertion mode - DrawPad() is called only after the loop
    addItem( tabItem, true );
}


void NCPkgTable::itemsCleared()
{
    return NCTable::deleteAllItems();
}


void NCPkgTable::cellChanged( int index, int colnum, const string & newtext )
{
    return NCTable::cellChanged( index, colnum, newtext );
}


//
// Set the new status in the first column of the package table and in libzypp
//
bool NCPkgTable::changeStatus( ZyppStatus newstatus,
			       const ZyppSel & slbPtr,
			       ZyppObj objPtr, // this is candidatePtr or what the user selected instead of it
			       bool singleChange )
{
    if ( !packager || !slbPtr )
	return false;

    yuiMilestone() << "Changing status of " << slbPtr->name() << endl;

    string notify;
    string license;
    bool confirmed = true;
    ZyppPkg pkgPtr = NULL;
    string header;
    bool ok = true;

    switch ( newstatus )
    {
	case S_Del:
	case S_NoInst:
	case S_Taboo:
	    if ( objPtr )
	    {
		notify = objPtr->delnotify();
		yuiMilestone() << "DELETE message: " << notify << endl;
		header = NCPkgStrings::WarningLabel();
	    }
	break;

        // display notify msg only if we mark pkg for installation
        // disregard update, to be consistent with Qt (#308410)
	case S_Install:
	    if ( objPtr )
	    {
		notify = objPtr->insnotify();
		yuiMilestone() << "NOTIFY message: " << notify << endl;
		header = NCPkgStrings::NotifyLabel();
	    }
            // FALLTHROUGH
	case S_Update:
	case S_AutoInstall:
	case S_AutoUpdate:
	    if ( objPtr )
	    {
                if ( objPtr->isRetracted() )
                    confirmed = confirmRetracted( objPtr, slbPtr );

                if ( confirmed )
                    license = objPtr->licenseToConfirm();
	    }
	    break;

	default: break;
    }

    string pkgName = slbPtr->name();

    if ( ! license.empty() )
    {
        if ( ! slbPtr->hasLicenceConfirmed() )
	    confirmed = packager->showLicensePopup( pkgName, license);

        if ( confirmed )
        {
	    yuiMilestone() << "User confirmed license agreement for " << pkgName << endl;
	    slbPtr->setLicenceConfirmed (true);
        }
    }

    if ( ! confirmed )
    {
        // make sure the package won't be installed
        switch ( newstatus )
        {
            case S_Install:
            case S_AutoInstall:
                newstatus = S_Taboo;
                break;

            case S_Update:
            case S_AutoUpdate:
                newstatus = S_Protected;
                break;

            default:
                break;
        }

        ok = false;
    }

    if ( ok && ! notify.empty() )
    {
        int cols  = NCurses::cols();
        int lines = NCurses::lines();

        string html_text = packager->InfoText()->createHtmlText( notify );
	NCPopupInfo * info = new NCPopupInfo( wpos( (lines * 35)/100, (cols * 25)/100),
					      header,
					      "<i>" + pkgName + "</i><br><br>" + html_text );
	info->setPreferredSize( (cols * 50)/100, (lines * 30)/100);
	info->showInfoPopup();

	YDialog::deleteTopmostDialog();
    }

    // inform the package manager
    ok = statusStrategy->setObjectStatus( newstatus, slbPtr, objPtr );

    if ( ok && singleChange )
    {
	switch ( tableType )
	{
	    case T_Packages:
	    case T_PatchPkgs:
	    case T_Update:
		// check/show dependencies of packages
		packager->showPackageDependencies( false );	// only check if automatic check is ON
		// show the required diskspace
		packager->showDiskSpace();
		break;

	    case T_Availables:
		// check/show dependencies of packages
		packager->showPackageDependencies( false );
		// don't show diskspace (type T_Availables is also used in YOU mode)
		break;

	    case T_Selections:
		// check/show dependencies of selections
		packager->showSelectionDependencies();
		packager->showDiskSpace();
		break;

	    case T_Patches:
		// show the download size for all selected patches
		packager->showDownloadSize();
		packager->showPackageDependencies( false );
		break;

	    default:
		break;
	}

        // update this list to show the status changes
	updateTable();

	if ( tableType == T_Availables || tableType == T_MultiVersion )
	{
	    // additionally update the package list
	    packager->updatePackageList();
	}
    }

    return ok;
}


//
// Set the new status info if status has changed
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
        YTableItem *it = dynamic_cast<YTableItem*> (cl->origItem() );
        YTableCell *tcell = it->cell(0);
	NCPkgTableTag * cc = static_cast<NCPkgTableTag*>( tcell );
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
	    if ( tableType == T_Availables && !slbPtr->multiversionInstall() )
	    {
		string isCandidate = "   ";
		if ( objPtr == slbPtr->candidateObj() )
		    isCandidate = " x ";

		cl->AddCol( 2, new NCTableCol( isCandidate ) );
	    }
	    else
	    {
		// get the new status and replace old status
		newstatus = statusStrategy->getPackageStatus( slbPtr, objPtr );

		// set new status (if status has changed)
		if ( getStatus(index) != newstatus )
		{
		    cc->setStatus( newstatus );
    		    cellChanged( index, 0, cc->statusToString (newstatus) );
		}
	    }
	}
	index++;
    }

    DrawPad();

    return ret;
}


static bool slbHasInstalledObj (const ZyppSel & slb)
{
    return ! slb->installedEmpty();
}


//
// Fill the column headers of the package table
//
void NCPkgTable::fillHeader()
{
    vector<string> header;

    switch ( tableType )
    {
	case T_Packages:
	case T_Update:
        {
	    bool haveInstalledPkgs = find_if (zyppPkgBegin(), zyppPkgEnd(),
					      slbHasInstalledObj) != zyppPkgEnd();

	    header.reserve(7);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PkgSummary() );

	    if ( haveInstalledPkgs > 0 )
	    {
		header.push_back( "L" + NCPkgStrings::PkgVersionNew() );
		header.push_back( "L" + NCPkgStrings::PkgVersionInst() );
		haveInstalledVersion = true;
	    }
	    else
	    {
		header.push_back( "L" + NCPkgStrings::PkgVersion() );
	    }

	    header.push_back( "L" + NCPkgStrings::PkgSize() );

#if SOURCE_INSTALL_SUPPORTED
	    header.push_back( "L" + NCPkgStrings::PkgSource() );
#endif
	    break;
	}
	case T_PatchPkgs:
        {
	    header.reserve(7);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PkgVersionNew() );
	    header.push_back( "L" + NCPkgStrings::PkgVersionInst() );
	    header.push_back( "L" + NCPkgStrings::PkgSummary() );
	    header.push_back( "L" + NCPkgStrings::PkgSize() );
	    break;
	}
	case T_Patches:
        {
	    header.reserve(6);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PatchKind() );
	    header.push_back( "L" + NCPkgStrings::PkgSummary() );
	    header.push_back( "L" + NCPkgStrings::PkgVersion() );
	    // header.push_back( "L" + NCPkgStrings::PkgSize() );
	    break;
	}
	case T_Selections:
        {
	    header.reserve(3);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PatternsLabel() );
	    break;
	}
	case T_Languages:
        {
	    header.reserve(4);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::LangCode() );
	    header.push_back( "L" + NCPkgStrings::LangName() );
	    break;
	}
	case T_Availables:
        {
	    header.reserve(6);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgVersion() );
	    header.push_back( "L" + NCPkgStrings::PkgInstSource() );
	    header.push_back( "L" + NCPkgStrings::PkgSize() );
	    header.push_back( "L" + NCPkgStrings::PkgArch() );
	    break;
	}
        case T_MultiVersion:
        {
	    header.reserve(5);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PkgVersion() );
	    header.push_back( "L" + NCPkgStrings::PkgInstSource() );
	    header.push_back( "L" + NCPkgStrings::PkgSize() );
	    header.push_back( "L" + NCPkgStrings::PkgArch() );
            break;
        }
	default: {
	    header.reserve(4);
	    header.push_back( "L" + NCPkgStrings::PkgStatus() );
	    header.push_back( "L" + NCPkgStrings::PkgName() );
	    header.push_back( "L" + NCPkgStrings::PkgSummary() );
	    break;
	}
    }
    setHeader( header );
}


bool NCPkgTable::createListEntry( ZyppPkg pkgPtr, ZyppSel slbPtr )
{
    vector<string> pkgLine;
    pkgLine.reserve(6);

    if ( !pkgPtr || !slbPtr )
    {
	yuiError() << "No valid package available" << endl;
	return false;
    }

    // add the package name
    pkgLine.push_back( slbPtr->name() );

    string instVersion = "";
    string version = "";
    ZyppStatus status;

    switch ( tableType )
    {
	case T_PatchPkgs:
        {
    	    // if the package is installed, get the installed version
	    if ( ! slbPtr->installedEmpty() )
	    {
		instVersion = slbPtr->installedObj()->edition().asString();
	    }

            // if a candidate is available, get the candidate version
	    if ( slbPtr->hasCandidateObj() )
	    {
		version = slbPtr->candidateObj()->edition().asString();
	    }
	    else
	    {
		version = pkgPtr->edition().asString();
	    }
   	    pkgLine.push_back( version );

	    // in case of YOU there are always installed packages
	    // => always add installed version (or empty column)
	    pkgLine.push_back( instVersion );

   	    pkgLine.push_back( pkgPtr->summary() );  	// short description

	    status = slbPtr->status(); // the package status
	    yuiMilestone() << "Status of " << slbPtr->name() << ": " << status << endl;
	    FSize size(zypp::ByteCount::SizeType(pkgPtr->installSize()));  // installed size
	    pkgLine.push_back( size.form( 8 ) );  // format size

	    break;
	}

	case T_Availables:
        {
            string isCandidate = "   ";
            if ( pkgPtr == slbPtr->candidateObj() )
                isCandidate = " x ";
            pkgLine.push_back( isCandidate );

            version = pkgPtr->edition().asString();

            if ( pkgPtr->isRetracted() )
                version += " " + NCPkgStrings::RetractedLabel();

            pkgLine.push_back( version );
            // show the name of the repository (the installation source)
            pkgLine.push_back( pkgPtr->repository().info().name() );

            // set package status either to S_NoInst or S_KeepInstalled
            status = S_NoInst;
            zypp::ui::Selectable::installed_iterator it = slbPtr->installedBegin();

            while ( it != slbPtr->installedEnd() )
            {
                if ( pkgPtr->edition() == (*it)->edition() &&
                     pkgPtr->arch() == (*it)->arch()	   &&
                     pkgPtr->vendor() == (*it)->vendor() )
                {
                    status = S_KeepInstalled;
                }
                ++it;
            }

	    FSize size(zypp::ByteCount::SizeType(pkgPtr->installSize()));  // installed size
	    pkgLine.push_back( size.form( 8 ) );  // format size
	    pkgLine.push_back( pkgPtr->arch().asString()); // architecture

	    break;
	}

        case T_MultiVersion:
        {
            version = pkgPtr->edition().asString();

            if ( pkgPtr->isRetracted() )
                version += " " + NCPkgStrings::RetractedLabel();

            pkgLine.push_back( version );
            // show the name of the repository (the installation source)
            pkgLine.push_back( pkgPtr->repository().info().name() );

            zypp::PoolItem itemPtr( pkgPtr->satSolvable() );
            status = slbPtr->pickStatus( itemPtr );
            yuiMilestone() << "Multi version: status of " << version << ": " << status << endl;

            FSize size(zypp::ByteCount::SizeType(pkgPtr->installSize()));     	// installed size
	    pkgLine.push_back( size.form( 8 ) );  // format size
	    pkgLine.push_back( pkgPtr->arch().asString()); // architecture
            break;
        }

	default:
        {
	    // if the package is installed, get the installed version
	    pkgLine.push_back( pkgPtr->summary() );  	// short description

	    if ( ! slbPtr->installedEmpty() )
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

	    status = slbPtr->status(); // the package status

	    FSize size(zypp::ByteCount::SizeType(pkgPtr->installSize()));  // installed size
	    pkgLine.push_back( size.form( 8 ) );  	// format size

#if SOURCE_INSTALL_SUPPORTED
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
	     slbPtr );

    return true;
}


bool NCPkgTable::createInfoEntry ( string text )
{
    vector<string> pkgLine;
    pkgLine.reserve(2);

    pkgLine.push_back( text );
    addLine( S_NoInst,	// use status NoInst
	     pkgLine,
	     ZyppObj(),
	     ZyppSel() ); // null pointer

    return true;
}


bool NCPkgTable::createPatchEntry ( ZyppPatch patchPtr, ZyppSel	slb )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);

    if ( !patchPtr || !slb )
    {
	yuiError() << "No valid patch available" << endl;
	return false;
    }

    pkgLine.push_back( slb->name() );	// show the patch name

    pkgLine.push_back( patchPtr->category() );  // patch kind

    if ( !patchPtr->summary().empty() )
	pkgLine.push_back( patchPtr->summary() );  	// short description
    else
	pkgLine.push_back( slb->name() );	// name

    pkgLine.push_back( patchPtr->edition().asString() ); // patch version

    // zypp::ByteCount size = patchPtr->size();
    // pkgLine.push_back( size.asString( 8 ) );


    addLine( slb->status(), //  get the status
	     pkgLine,
	     patchPtr,
	     slb );	// the corresponding pointer

    return true;
}


bool NCPkgTable::showInformation()
{
    ZyppObj objPtr = getDataPointer( getCurrentItem() );
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );

    if ( !packager || !objPtr || !slbPtr )
	return false;

    yuiMilestone() << "show information for " << slbPtr->name() << endl;

    switch ( tableType )
    {
	case T_Packages:
	case T_Update:
	    // show the required package info
            updateInfo( objPtr, slbPtr, VisibleInfo() );
            packager->PackageLabel()->setLabel( slbPtr->name() );
	    break;

	case T_Patches:
	    // show the patch info
            updateInfo( objPtr, slbPtr, VisibleInfo() );
	    break;

	default:
	    break;
    }

    return true;
}


NCursesEvent NCPkgTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    // call handleInput of NCPad
    handleInput( key );

    if ( packager->isTestMode() )
    {
	if ( packager->diskSpacePopup() )
	    packager->diskSpacePopup()->setDiskSpace( key );
	return ret;
    }

    switch ( key )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME:
	    showInformation();
	    break;

	case KEY_SPACE:
	case KEY_RETURN:
	    cycleObjStatus();
	    break;

        // Inherited from the parent class to enable sorting
	case CTRL('o'):
	    NCTable::wHandleInput( key);
	    break;

        case '-':
        case '+':
        case '>':
        case '<':
        case '!':
        case '*':
	    // set the new status
	    changeObjStatus( key );

	default:
	    break;
    }

    NCDialog * currentDialog = static_cast<NCDialog *>(YDialog::topmostDialog());
    if ( currentDialog )
        currentDialog->setStatusLine();

    return  NCursesEvent::handled;
}


//
// Get the status of the package of the selected line
//
ZyppStatus NCPkgTable::getStatus( int index )
{
    NCPkgTableTag * cc = getTag( index);
    if ( !cc )
	return S_NoInst;

    return cc->getStatus();
}


ZyppObj NCPkgTable::getDataPointer( int index )
{
    NCPkgTableTag *cc = getTag( index );
    if ( !cc )
	return ZyppObj();

    return cc->getDataPointer();
}


ZyppSel NCPkgTable::getSelPointer( int index )
{
    NCPkgTableTag *cc = getTag( index );
    if ( !cc )
	return ZyppSel();

    return cc->getSelPointer();
}


NCPkgTableTag * NCPkgTable::getTag( const int & index )
{
    NCTableLine * cl = myPad()->ModifyLine( index );
    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    YTableItem *it = dynamic_cast<YTableItem*> (cl->origItem() );
    YTableCell *tcell = it->cell(0);
    NCPkgTableTag * cc = static_cast<NCPkgTableTag *>( tcell );

    return cc;
}


#if SOURCE_INSTALL_SUPPORTED

bool NCPkgTable::SourceInstall( bool install )
{
    int index =  getCurrentItem();
    ZyppObj objPtr = getDataPointer( index );
    bool ok;

    if ( !objPtr )
    {
	yuiError() << "Invalid Pointer" << endl;
	return false;
    }
    ZyppSel selPtr = objPtr->getSelectable();
    NCTableLine * currentLine = myPad()->ModifyLine( index );

    if ( !selPtr  || !currentLine )
    {
	yuiError() << "Invalid Selectable" << endl;
	return false;
    }

    NCTableCol * currentCol = currentLine->GetCol( currentLine->Cols()-1 );

    if ( install && selPtr->providesSources() )
    {
	ok = selPtr->set_source_install( true );
	yuiMilestone() << "Set source install returns: " << ( ok ? "true" : "false" ) << endl;
	if ( currentCol )
	    currentCol->SetLabel( NClabel( " x " ) );
    }
    else if ( !install && selPtr->source_install() )
    {
	ok = selPtr->set_source_install( false );
	yuiMilestone() << "ReSet source install returns: " << ( ok ? "true" : "false" ) << endl;
	if ( currentCol )
	    currentCol->SetLabel( NClabel( "   " ) );
    }

    return true;
}
#endif


bool NCPkgTable::cycleObjStatus()
{
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );
    ZyppObj objPtr = getDataPointer( getCurrentItem() );

    if ( !slbPtr )
	return false;

    ZyppStatus newStatus;

    bool ok = statusStrategy->cycleStatus( slbPtr, objPtr, newStatus );

    if ( ok )
    {
	changeStatus( newStatus, slbPtr, objPtr, true );
    }

    return true;
}


bool NCPkgTable::changeObjStatus( int key )
{
    ZyppSel slbPtr = getSelPointer( getCurrentItem() );
    ZyppObj objPtr = getDataPointer( getCurrentItem() );

    if ( !slbPtr )
	return false;

    ZyppStatus newStatus;

    bool ok = statusStrategy->keyToStatus( key, slbPtr, objPtr, newStatus );

    if ( ok )
	changeStatus( newStatus, slbPtr, objPtr, true );

    return true;
}


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
	    switch ( type )
            {
		case A_Install:
                {
		    if ( slbPtr->status() == S_NoInst )
			ok = statusStrategy->keyToStatus( '+', slbPtr, objPtr, newStatus );
		    break;
		}

		case A_Delete:
                {
		    if ( slbPtr->installedObj() && slbPtr->status() != S_Protected )
			ok = statusStrategy->keyToStatus( '-', slbPtr, objPtr, newStatus );
		    break;
		}

		case A_UpdateNewer:
                {
                    // set status to update respecting "vendor change" settings
                    if ( slbPtr->installedObj() && slbPtr->status() != S_Protected && slbPtr->updateCandidateObj() )
                    {
                        slbPtr->setOnSystem( slbPtr->updateCandidateObj() );
                        ok = statusStrategy->keyToStatus( '>', slbPtr, objPtr, newStatus );
                    }
		    break;
		}

		case A_Update:
                {
		    if ( slbPtr->installedObj() && slbPtr->status() != S_Protected )
			ok = statusStrategy->keyToStatus( '>', slbPtr, objPtr, newStatus );
		    break;
		}

		case A_Keep:
                {
		    if ( slbPtr->status() == S_Install
			 ||  slbPtr->status() == S_AutoInstall
		         ||  slbPtr->status() == S_Update
			 ||  slbPtr->status() == S_AutoUpdate )
			ok = statusStrategy->keyToStatus( '<', slbPtr, objPtr, newStatus );
		    else if ( slbPtr->status() == S_Del
			      || slbPtr->status() == S_AutoDel )
			ok = statusStrategy->keyToStatus( '+', slbPtr, objPtr, newStatus );
		    break;
		}

		default:
		    yuiError() << "Unknown list action" << endl;
                    break;
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


bool NCPkgTable::fillAvailableList( ZyppSel slb )
{
    if ( !slb )
    {
	yuiError() << "Package pointer not valid" << endl;
	return false;
    }

    // clear the package table
    itemsCleared();

    NCPkgStatusStrategy * strategy;
    NCPkgTableType type;

    if ( slb->multiversionInstall() || slb->installedSize() > 1 )
    {
        // Either the selectable has at least one multiversion
        // package or there are more than one installed package
        // instances.
        // The last case can also happens by an previous package
        // installation error.
        type = T_MultiVersion;
        strategy = new MultiVersionStatStrategy();
        yuiMilestone() << "Multi version package " << slb->name() << endl;
    }
    else
    {
        type = T_Availables;
        strategy = new AvailableStatStrategy();
    }

    setTableType( type, strategy );
    this->fillHeader();

    // pick list contains installed and available packages (valid for single and multi version)
    zypp::ui::Selectable::picklist_iterator it = slb->picklistBegin();
    while ( it != slb->picklistEnd() )
    {
        createListEntry( tryCastToZyppPkg(*it), slb );
        ++it;
    }

    // show the package list
    drawList();

    if ( getNumLines() > 0 )
	setCurrentItem( 0 );

    return true;

}


bool NCPkgTable::fillSummaryList( NCPkgTable::NCPkgTableListType type )
{
    // clear the package table
    itemsCleared();

    // get the package list and sort it
    std::list<ZyppSel> pkgList( zyppPkgBegin(), zyppPkgEnd() );
    pkgList.sort( sortByName );

    // fill the package table
    std::list<ZyppSel>::iterator listIt;
    ZyppPkg pkgPtr;

    // If the dependency check is off, the dependencies will not be solved for
    // the installation summary.
    // This is not necessary because the dependencies will be solved and the
    // "Automatic Changes" list will be shown if the OK button is pressed.
    //
    //   if ( !autoCheck )
    //   {
    //       showPackageDependencies( true );
    //   }

    for ( listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt )
    {
	ZyppSel selectable = *listIt;
	ZyppPkg pkg = tryCastToZyppPkg( selectable->theObj() );
	// show all matching packages
	switch ( type )
	{
	    case NCPkgTable::L_Changes:
		if ( selectable->status() != S_NoInst
		     && selectable->status() != S_KeepInstalled )
		{
		    createListEntry( pkg, selectable );
		}
		break;

	    case NCPkgTable::L_Installed:
		if ( selectable->status() == S_KeepInstalled )
		{
		    createListEntry( pkg, selectable );
		}
		break;

	    default:
		break;
	}
    }

    // show the package list
    drawList();

    return true;
}


void NCPkgTable::updateInfo( ZyppObj pkgPtr, ZyppSel slbPtr, NCPkgTableInfoType mode )
{
    switch ( mode )
    {
	case I_Descr:
	    if ( packager->InfoText() )
		packager->InfoText()->longDescription( pkgPtr );
	    break;

	case I_Technical:
	    if ( packager->InfoText() )
		packager->InfoText()->technicalData( pkgPtr, slbPtr );
	    break;

	case I_Files:
	    if ( packager->InfoText() )
		packager->InfoText()->fileList( slbPtr );
	    break;

	case I_Deps:
	    if ( packager->InfoText() )
		packager->InfoText()->dependencyList( pkgPtr, slbPtr );
	    break;

	case I_Versions:
	    if ( packager->VersionsList() )
		packager->VersionsList()->fillAvailableList( slbPtr );
	    break;

	case I_PatchDescr:
	    if ( packager->InfoText() )
		packager->InfoText()->patchDescription( pkgPtr, slbPtr );
	    break;

	case I_PatchPkgs:
	    if ( packager->PatchPkgs() )
		packager->fillPatchPackages( packager->PatchPkgs(), pkgPtr );
	    break;

	// Intentionally omitting 'default' branch so the compiler can
	// catch unhandled enum states
    }
}


bool NCPkgTable::confirmRetracted( ZyppObj pkg, ZyppSel sel )
{
    yuiMilestone() << "Retracted object " << sel->name() << " " << pkg->edition() << endl;

    // Headline of confirmation popup
    string heading = _( "Please confirm" );

    std::ostringstream msg;
    //  %s is a package name
    msg << boost::format( _( "<p>Really install a retracted version of \"%s\" ?</p>" ) ) % sel->name();
    int width = msg.str().size() + 2;
    int height = 7;

    NCPopupInfo * info = new NCPopupInfo( wpos( ( NCurses::lines() - height ) / 2,
                                                ( NCurses::cols()  - width  ) / 2 ),
                                          heading,
                                          msg.str(),
                                          NCPkgStrings::YesLabel(),
                                          NCPkgStrings::NoLabel() );
    info->setPreferredSize( width, height );
    NCursesEvent event = info->showInfoPopup();
    YDialog::deleteTopmostDialog();
    bool confirmed = ( event != NCursesEvent::cancel );

    yuiMilestone() << "User " << ( confirmed ? "confirmed" : "rejected" )
                   << " retracted object " << sel->name() << " " << pkg->edition()
                   << endl;

    return confirmed;
}
