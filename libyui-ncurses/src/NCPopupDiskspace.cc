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

   File:       NCPopupDiskspace.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"


#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCTable.h"

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PkgDu.h>

#include "NCPopupDiskspace.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::NCPopupDiskspace
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupDiskspace::NCPopupDiskspace( const wpos at )
    : NCPopup( at, false )
      , partitions( 0 )
      , okButton( 0 )
      , hDim( 55 )
      , vDim( 15 )
{
    createLayout( YCPString(PkgNames::DiskspaceLabel()) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::~NCPopupDiskspace
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupDiskspace::~NCPopupDiskspace()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDiskspace::createLayout( const YCPString & headline )
{

    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );
  
    // add the headline
    opt.isHeading.setValue( true );
    NCLabel * head = new NCLabel( split, opt, headline );
    split->addChild( head );

    vector<string> header;
    header.reserve(5);
    header.push_back( "L" + PkgNames::Partition() );
    header.push_back( "L" + PkgNames::UsedSpace() );
    header.push_back( "L" + PkgNames::FreeSpace() );
    header.push_back( "L" + PkgNames::TotalSpace() );
    header.push_back( "L%   ");
    
    // add the partition table 
    partitions = new NCTable( split, opt, header );

    split->addChild( partitions );

    // add the ok button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, opt, YCPString(PkgNames::OKLabel()) );
    okButton->setId( PkgNames::OkButton () );
  
    split->addChild( okButton );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::fillPartitionTable
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDiskspace::fillPartitionTable()
{
    vector<string> pkgLine;
    pkgLine.reserve(5);
    int i = 0;

    const PkgDuMaster & duMaster =  Y2PM::packageManager().updateDu();        

    std::set<PkgDuMaster::MountPoint>::iterator it = duMaster.mountpoints().begin();

    partitions->itemsCleared();		// clear table
    
    while ( it != duMaster.mountpoints().end() )
    {
	pkgLine.clear();
	pkgLine.push_back( (*it).mountpoint() );

	pkgLine.push_back( (*it).pkg_used().form(8) );
	pkgLine.push_back( (*it).pkg_available().form(8) );
	pkgLine.push_back( (*it).total().form(8) );	
	pkgLine.push_back( usedPercent( (*it).pkg_used(), (*it).total() ) );
	partitions->itemAdded( pkgLine, i );
	
	++it;
	i++;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::checkDiskSpace
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string NCPopupDiskspace::checkDiskSpace()
{
    string text = "";
    
    const PkgDuMaster & duMaster =  Y2PM::packageManager().updateDu();        

    std::set<PkgDuMaster::MountPoint>::iterator it = duMaster.mountpoints().begin();

    while ( it != duMaster.mountpoints().end() )
    {
	if ( (*it).pkg_available() < (FSize)0 )
	{
	    text += "\"";
	    text += (*it).mountpoint();
	    text += "\""; 
	    text += " ";
	    text += PkgNames::MoreText();
	    text += " ";
	    string available = (*it).pkg_available().asString();
	    text += available.replace( 0, 1, " " );
	    text += " ";
	    text += PkgNames::MoreSpaceText();
	    text += "<br>";
	}
	++it;
    }
    return text;
}

string NCPopupDiskspace::usedPercent( FSize used, FSize total )
{
    int percent = 0;
    char percentStr[10];
    
    if ( total != 0 )
	percent = ( 100 * used ) / total;

    sprintf( percentStr, "%d%%", percent );
     
    return percentStr;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::showInfoPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDiskspace::showInfoPopup( )
{
    // update values in partition table
    fillPartitionTable();
    
    postevent = NCursesEvent();
    do {
	// show the popup
	popupDialog( );
    } while ( postAgain() );
    
    popdownDialog();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupDiskspace::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? hDim : vDim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupDiskspace::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;
    
    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDiskspace::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupDiskspace::postAgain()
{
    if ( ! postevent.widget )
	return false;
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

