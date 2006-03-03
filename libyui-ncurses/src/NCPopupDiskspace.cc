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

#include "YQZypp.h"

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
    partitions = new NCTable( split, opt, header, false );

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
    partitions->itemsCleared();		// clear table
    
    vector<string> pkgLine;
    pkgLine.reserve(5);
    int i = 0;

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    zypp::DiskUsageCounter::MountPointSet du = z->diskUsage ();
    zypp::DiskUsageCounter::MountPointSet::iterator
	b = du.begin (),
	e = du.end (),
	it;
    if (b == e)
    {
	// retry after detecting from the target
	z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
	du = z->diskUsage();
	b = du.begin ();
	e = du.end ();
    }

    for (it = b; it != e; ++it)
    {
	if (it->readonly)
	    continue;

	pkgLine.clear();
	pkgLine.push_back (it->dir);

	zypp::ByteCount pkg_used (it->pkg_size * 1024);
	pkgLine.push_back (pkg_used.asString (8));

	zypp::ByteCount pkg_available ((it->total_size - it->pkg_size) * 1024);
	pkgLine.push_back (pkg_available.asString (8));

	zypp::ByteCount total (it->total_size * 1024);
	pkgLine.push_back (total.asString (8));

	pkgLine.push_back( usedPercent( it->pkg_size, it->total_size ) );
	partitions->itemAdded( pkgLine, i );
	
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

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    zypp::DiskUsageCounter::MountPointSet du = z->diskUsage ();
    zypp::DiskUsageCounter::MountPointSet::iterator
	b = du.begin (),
	e = du.end (),
	it;
    if (b == e)
    {
	// retry after detecting from the target
	z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
	du = z->diskUsage();
	b = du.begin ();
	e = du.end ();
    }

    for (it = b; it != e; ++it)
    {
	zypp::ByteCount pkg_available = (it->total_size - it->pkg_size) * 1024;
	if ( pkg_available < 0 )
	{
	    text += "\"";
	    text += it->dir;
	    text += "\""; 
	    text += " ";
	    text += PkgNames::MoreText();
	    text += " ";
	    string available = pkg_available.asString();
	    text += available.replace( 0, 1, " " ); // clear the minus sign??
	    text += " ";
	    text += PkgNames::MoreSpaceText();
	    text += "<br>";
	}
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
    long vdim;
    if ( NCurses::lines() > 15 )
	vdim = 15;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
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

