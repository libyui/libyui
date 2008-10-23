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

   File:       NCPkgPopupDiskspace.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "YMenuButton.h"
#include "YDialog.h"
#include "YTypes.h"

#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCTable.h"

#include "NCZypp.h"

#include "NCPkgPopupDiskspace.h"

#include "NCi18n.h"

using namespace std;

// set values as set in YQPkgDiskUsageList.cc
#define MIN_FREE_MB_WARN	400
#define MIN_FREE_MB_PROXIMITY	700

#define MIN_PERCENT_WARN	90
#define MIN_PERCENT_PROXIMITY	80

#define OVERFLOW_MB_WARN	0
#define OVERFLOW_MB_PROXIMITY	300

/*
  Textdomain "ncurses-pkg"
*/

 ///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::NCPkgDiskspace
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgDiskspace::NCPkgDiskspace( bool testMode )
      : testmode( testMode )
      , popupWin( 0 ) 
{

    if ( testMode )
    {
	yuiMilestone() << "TESTMODE Diskspace" << endl;
	zypp::getZYpp()->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
	testDiskUsage = zypp::getZYpp()->diskUsage();
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::~NCPkgDiskspace
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgDiskspace::~NCPkgDiskspace()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::fillPartitionTable
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgDiskspace::fillPartitionTable()
{
    NCTable * partitions = popupWin->Partitions();
    partitions->deleteAllItems();		// clear table

    YTableItem * newItem;
    int i = 0;

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();
    ZyppDuSetIterator
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

	zypp::ByteCount pkg_used (it->pkg_size * 1024);

	zypp::ByteCount pkg_available ((it->total_size - it->pkg_size) * 1024);

	zypp::ByteCount total (it->total_size * 1024);

	newItem = new YTableItem( it->dir,
				  pkg_used.asString (8),
				  pkg_available.asString (8),
				  total.asString (8),
				  usedPercent( it->pkg_size, it->total_size ) );

        partitions->addItem( newItem );
	
	i++;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkDiskSpace
//	METHOD TYPE : string
//
//	DESCRIPTION : called to check disk space before installation
//		      (after OK button is pressed)
//
string NCPkgDiskspace::checkDiskSpace()
{
    string text = "";

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();
    ZyppDuSetIterator
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
		
	zypp::ByteCount pkg_available = (it->total_size - it->pkg_size) * 1024;
	if ( pkg_available < 0 )
	{
	    text += "\"";
	    text += it->dir;
	    text += "\""; 
	    text += " ";
	    text += NCPkgStrings::MoreText();
	    text += " ";
	    string available = pkg_available.asString();
	    text += available.replace( 0, 1, " " ); // clear the minus sign??
	    text += " ";
	    text += NCPkgStrings::MoreSpaceText();
	    text += "<br>";
	}
    }
    return text;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkRemainingDiskSpace
//	METHOD TYPE : void
//
//	DESCRIPTION : check whether remaining disk space enters
// 		      warning or error range
//
void NCPkgDiskspace::checkRemainingDiskSpace( const ZyppPartitionDu & partition )
{
    FSize usedSize ( partition.pkg_size, FSize::K );
    FSize totalSize ( partition.total_size, FSize::K );

    int percent = 0;

    if ( totalSize != 0 )
	percent = ( 100 * usedSize ) / totalSize;

    int	free	= ( totalSize - usedSize ) / FSize::MB;

    yuiMilestone() <<  "Partition: " << partition.dir << "  Used percent: "
	  << percent << "  Free: " << free << endl;
    
    if ( percent > MIN_PERCENT_WARN )
    {
	// Modern hard disks can be huge, so a warning based on percentage only
	// can be misleading - check the absolute value, too.
	if ( free < MIN_FREE_MB_PROXIMITY )
	{
	    runningOutWarning.enterProximity();
	}
	if ( free < MIN_FREE_MB_WARN )
	{
	    runningOutWarning.enterRange();
	}
    }

    if ( free < MIN_FREE_MB_PROXIMITY )
    {
	if ( percent > MIN_PERCENT_PROXIMITY )
	    runningOutWarning.enterProximity();
    }

    if ( free < OVERFLOW_MB_WARN )
	overflowWarning.enterRange();

    if ( free < OVERFLOW_MB_PROXIMITY )
	overflowWarning.enterProximity();

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::setDiskSpace
//	METHOD TYPE : void
//
//	DESCRIPTION : For testing only; called from NCPkgTable if the PackageSelector
//		      running in testMode
//	TESTDESCRIPTION: Call `PackageSelector with `opt(`testMode) (ycp example).
//		      	 With focus on the package list press '+' or '-' to
//		      	 increase/decrease used diskspace (see y2log).
//			 Use the 'Actions' menu to select/delete a package. 
//
void NCPkgDiskspace::setDiskSpace( wint_t ch )
{
    int percent = 0;
    
    // set diskspace values in ZyppDuSet testDiskSpace
    for ( ZyppDuSetIterator it = testDiskUsage.begin();
	  it != testDiskUsage.end();
	  ++it )
    {
	const ZyppPartitionDu & partitionDu = *it;

	FSize usedSize ( partitionDu.pkg_size, FSize::K );
	FSize totalSize ( partitionDu.total_size, FSize::K );

	if ( totalSize != 0 )
	    percent = ( 100 * usedSize ) / totalSize;

	if ( ch == '+' )
	    percent += 3;
	else if ( ch == '-' )
	    percent -= 3;

	if ( percent < 0   )
	    percent = 0;
		    
	partitionDu.pkg_size = partitionDu.total_size * percent / 100;

	FSize newSize ( partitionDu.pkg_size, FSize::K );

	yuiMilestone() << "Used size (MB): " << newSize / FSize::MB << endl;
	yuiMilestone() << "Total size (MB): " << totalSize / FSize::MB << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkDiskSpaceRange
//	METHOD TYPE : void
//
//	DESCRIPTION : calls checkRemaingDiskspace for every partition
//
void NCPkgDiskspace::checkDiskSpaceRange( )
{
    // see YQPkgDiskUsageList::updateDiskUsage()
    runningOutWarning.clear();
    overflowWarning.clear();
    ZyppDuSet diskUsage;
    
    if ( testmode )
	diskUsage = testDiskUsage;
    else
	diskUsage = zypp::getZYpp()->diskUsage();

    for ( ZyppDuSetIterator it = diskUsage.begin();
	  it != diskUsage.end();
	  ++it )
    {
	//Exclude readonly dirs from the check (#384368)
	if( it->readonly )
	    continue;
	checkRemainingDiskSpace( *it );
    }

     // see YQPkgDiskUsageList::postPendingWarnings()
    if ( overflowWarning.needWarning() )
    {
	showInfoPopup( _( "Error: Out of disk space!" ) );

	overflowWarning.warningPostedNotify();
	runningOutWarning.warningPostedNotify(); // Suppress this ( now redundant ) other warning
    }

    if ( runningOutWarning.needWarning() )
    {
	showInfoPopup( _( "Warning: Disk space is running out!" ) );

	runningOutWarning.warningPostedNotify();
    }

    if ( overflowWarning.leavingProximity() )
	overflowWarning.clearHistory();

    if ( runningOutWarning.leavingProximity() )
	runningOutWarning.clearHistory();

    if ( testmode )
    {
	yuiMilestone() << "Running out Warning:" << endl;
	runningOutWarning.logSettings();

	yuiMilestone() << "Overflow Warning:" << endl; 
	overflowWarning.logSettings();
    }
}

string NCPkgDiskspace::usedPercent( FSize used, FSize total )
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
//	METHOD NAME : NCPkgDiskspace::showInfoPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgDiskspace::showInfoPopup( string headline )
{
    
    popupWin = new NCPkgPopupDiskspace (wpos( (NCurses::lines() - 15)/2, NCurses::cols()/6  ), headline );
    // update values in partition table
    fillPartitionTable();
    popupWin->doit();
    YDialog::deleteTopmostDialog();    
} 

zypp::ByteCount NCPkgDiskspace::calculateDiff()
{
    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();
    ZyppDuSetIterator
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

    zypp::ByteCount diff = 0;
    for (it = b; it != e; ++it)
    {
	diff += (it->pkg_size - it->used_size) * 1024;
    }

    return diff;  
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::NCPkgPopupDiskspace
//	METHOD TYPE : Constructor
//
NCPkgPopupDiskspace::NCPkgPopupDiskspace( const wpos at, string headline )
    : NCPopup( at, false )
    , partitions( 0 )
    , okButton( 0 )
    , head( 0 )
{
    createLayout( headline );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::~NCPkgPopupDiskspace
//	METHOD TYPE : Destructor
//
NCPkgPopupDiskspace::~NCPkgPopupDiskspace()
{
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::createLyout
//	METHOD TYPE : void
//
//	DESCRIPTION : create layout (partition table)
//
void NCPkgPopupDiskspace::createLayout( string headline )
{
    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    head = new NCLabel( split, "", true, false );	// isHeading = true
    head->setLabel( headline );

    YTableHeader * tableHeader = new YTableHeader();
    tableHeader->addColumn( NCPkgStrings::Partition(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::UsedSpace(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::FreeSpace(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::TotalSpace(), YAlignBegin );
    tableHeader->addColumn( "% ", YAlignBegin );

    // add the partition table 
    partitions = new NCTable( split, tableHeader );

    // add the ok button
    okButton = new NCPushButton( split, NCPkgStrings::OKLabel() );
    okButton->setFunctionKey( 10 );
    okButton->setKeyboardFocus();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPopupDiskspace::preferredWidth()
{
    return NCurses::cols()*2/3;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPopupDiskspace::preferredHeight()
{
    if ( NCurses::lines() > 15 )
	return 15;
    else
	return NCurses::lines()-4;
}

void NCPkgPopupDiskspace::doit()
{
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
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDiskspace::wHandleInput( wint_t ch )
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
//	METHOD NAME : NCPkgPopupDiskspace::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupDiskspace::postAgain()
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




NCPkgWarningRangeNotifier::NCPkgWarningRangeNotifier()
{
    clearHistory();
}


void
NCPkgWarningRangeNotifier::clear()
{
    _inRange 		= false;
    _hasBeenClose	= _isClose;
    _isClose 		= false;
}


void
NCPkgWarningRangeNotifier::clearHistory()
{
    clear();
    _hasBeenClose  = false;
    _warningPosted = false;
}


void
NCPkgWarningRangeNotifier::enterRange()
{
    _inRange = true;
    enterProximity();
}


void
NCPkgWarningRangeNotifier::enterProximity()
{
    _isClose      = true;
    _hasBeenClose = true;
}


void
NCPkgWarningRangeNotifier::warningPostedNotify()
{
    _warningPosted = true;
}


bool
NCPkgWarningRangeNotifier::inRange() const
{
    return _inRange;
}


bool
NCPkgWarningRangeNotifier::leavingProximity() const
{
    return ! _isClose && ! _hasBeenClose;
}


bool
NCPkgWarningRangeNotifier::needWarning() const
{
    return _inRange && ! _warningPosted;
}

void
NCPkgWarningRangeNotifier::logSettings() const
{
    yuiMilestone() << "in range: " << (_inRange?"true":"false") << endl;
    yuiMilestone() << "is close: " << (_isClose?"true":"false") << endl;
    yuiMilestone() << "has been close: " << (_hasBeenClose?"true":"false") << endl;
    yuiMilestone() << "warning posted: " << (_warningPosted?"true":"false") << endl;
}

