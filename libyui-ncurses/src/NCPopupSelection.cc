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

   File:       NCPopupSelection.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupSelection.h"

#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCPkgTable.h"
#include "ObjectStatStrategy.h"

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMSelectionManager.h>


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::NCPopupSelection
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupSelection::NCPopupSelection( const wpos at,  PackageSelector * pkg  )
    : NCPopup( at, false )
    , sel( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    // get the available selections
    getSelections();

    createLayout( YCPString(PkgNames::SelectionLabel().str()) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::~NCPopupSelection
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupSelection::~NCPopupSelection()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupSelection::createLayout( const YCPString & label )
{

  YWidgetOpt opt;

  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );

  opt.notifyMode.setValue( false );

  //the headline
  opt.isHeading.setValue( true );

  NCLabel * head = new NCLabel( split, opt, PkgNames::SelectionLabel().str() );
  split->addChild( head );
  
  // add the selection list
  sel = new NCPkgTable( split, opt );
  sel->setPackager( packager );
  // set status strategy
  ObjectStatStrategy * strat = new PackageStatStrategy();
  sel->setTableType( NCPkgTable::T_Selections, strat );
  split->addChild( sel );
  
  opt.notifyMode.setValue( true );

  NCLabel * help = new NCLabel( split, opt, YCPString(PkgNames::DepsHelpLine().str()) );
  split->addChild( help );

  NCSpacing * sp1 = new NCSpacing( split, opt, 0.4, false, true );
  split->addChild( sp1 );
  
  // add an OK button
  opt.key_Fxx.setValue( 10 );
  okButton = new NCPushButton( split, opt, YCPString(PkgNames::OKLabel().str()) );
  okButton->setId( PkgNames::OkButton () );
  
  split->addChild( okButton );

  NCSpacing * sp2 = new NCSpacing( split, opt, 0.4, false, true );
  split->addChild( sp2 );
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
NCursesEvent & NCPopupSelection::showSelectionPopup( )
{
    int i;
    vector<PMSelectionPtr>::iterator it;    
    vector<string> pkgLine;
    pkgLine.reserve(4);
    
    postevent = NCursesEvent();

    if ( !sel )
	return postevent;
    
    sel->itemsCleared();

    // fill the selection box
    for ( i = 0, it = selections.begin(); it != selections.end(); ++it, i++ )
    {
	pkgLine.clear();
	pkgLine.push_back( (*it)->summary(Y2PM::getPreferredLocale()) );
	
	sel->addLine( (*it)->getSelectable()->status(),	// the status 
		      pkgLine,
		      i,
		      (*it) );		// PMSelectionPtr
    }

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    if ( !packager )
	return postevent;
    
    // if OK is clicked get the current item and show the package list
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	if ( !selections.empty() )
	{
	    int index = sel->getCurrentItem();
	    PMSelectionPtr selPtr = selections[index];
	
	    NCMIL << "Current selection: " << getCurrentLine() << endl;
	    // activate the package solving
	    Y2PM::selectionManager().activate( Y2PM::packageManager() );
	    // show the package list
	    packager->showSelPackages( getCurrentLine(), selPtr );
	    packager->showDiskSpace();
	}
    }
    
    return postevent;
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected list item (may be "really" selected
// or not)
//
string  NCPopupSelection::getCurrentLine( )
{
    if ( !sel )
	return "";
    
    int index = sel->getCurrentItem();

    return ( selections[index]->summary(Y2PM::getPreferredLocale()) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupSelection::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? NCurses::cols()-20 : NCurses::lines()-5 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupSelection::wHandleInput( int ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupSelection::postAgain( )
{
    if( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;
     
    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    
    if ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
    {
	postevent.detail = NCursesEvent::USERDEF ;
	// return false means: close the popup
	return false;
    }
    
    if (postevent == NCursesEvent::cancel)
	return false;

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PackageSelector::getSelections
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
bool NCPopupSelection::getSelections( )
{

    // get selections
    PMSelectionPtr selPtr;
    PMManager::PMSelectableVec::const_iterator it;
    
    for (  it = Y2PM::selectionManager().begin();
	 it != Y2PM::selectionManager().end();
	 ++it )
    {
	PMSelectionPtr instPtr = (*it)->installedObj();
	PMSelectionPtr candPtr = (*it)->candidateObj();

	if ( instPtr )
	{
	    selPtr = instPtr;
	}
	else if ( candPtr )
	{
	    selPtr = candPtr;
	}
	else
	{
	    selPtr = (*it)->theObject();
	}

	// get the available add-ons
	if ( selPtr && selPtr->visible() && !selPtr->isBase() )
	{
	    NCMIL << "Add-on selection: " <<  selPtr->name() << ", status: " << selPtr->getSelectable()->status() << endl; 
	    selections.push_back( selPtr );
	}
    }
    
    return true;
}
