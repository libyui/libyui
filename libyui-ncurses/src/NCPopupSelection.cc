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

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>


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
    , selectionBox( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    // get the available selections
    getSelections();

    createLayout( PkgNames::SelectionLabel() );
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

  // the multi selection widget
  selectionBox = new NCMultiSelectionBox( split, opt, label );
  split->addChild( selectionBox );

  opt.notifyMode.setValue( true );
  
  // add an OK button
  okButton = new NCPushButton( split, opt, PkgNames::OKLabel() );
  okButton->setId( PkgNames::OkButton () );
  
  split->addChild( okButton );

  NCSpacing * sp = new NCSpacing( split, opt, 0.1, false, true );
  split->addChild( sp );
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
NCursesEvent & NCPopupSelection::showSelectionPopup( )
{
    int i;
    vector< pair<PMSelectionPtr, bool> >::iterator it;    

    postevent = NCursesEvent();

    if ( !selectionBox )
	return postevent;
    
    selectionBox->clearItems();

    // fill the selection box
    for ( i = 0, it = selections.begin(); it != selections.end(); ++it, i++ )
    {
	// FIXME: get the (translated) description
	selectionBox->addItem( YCPString( (*it).first->name() ),	// description
			       PkgNames::Treeitem(),	// `id
			       (*it).second );		// selected
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
	    int index = selectionBox->getCurrentItem();
	    PMSelectionPtr selPtr = selections[index].first;
	
	    NCMIL << "Current selection: " << getCurrentLine() << endl;

	    packager->showSelPackages( getCurrentLine(), selPtr );
	}
    }
    
    return postevent;
}

///////////////////////////////////////////////////////////////////
//
// bool getSelectedItems ( vector<int> & list )
//
// ------- UNUSED ---------
//
bool  NCPopupSelection::getSelectedItems ( vector<int> & selItems )
{
    vector<int> items;
    items.reserve(20);
    
    const NCTableCol * tag = 0;

    if ( !selectionBox )
	return false;

    // get the size of the selection box 
    unsigned int size = selectionBox->getNumLines();
    unsigned int index = 0;
    
    while ( index < size )
    {
	// get the line 
	const NCTableLine * line = selectionBox->getLine( index );
	if ( !line )
	{
	    NCINT << "No such line: " << index << endl; 
	}
	else
	{
	    tag = line->GetCol( 0 );
	}
	if ( tag && ( (NCTableTag *)tag )->Selected() )
	{
	    items.push_back( index );
	    NCINT << "Selected line: " << index << endl;
	}
	index++;
    }
    
    selItems = items;
    
    return true; 
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
    if ( !selectionBox )
	return "";
    
    int index = selectionBox->getCurrentItem();

    return ( selections[index].first->name() );
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
    return ( dim == YD_HORIZ ? 30 : 20 );
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

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    NCursesEvent event = NCDialog::wHandleInput( ch );
    
    if ( ch == KEY_SPACE )
    {
	int index = selectionBox->getCurrentItem();
	PMSelectionPtr selPtr = selections[index].first;
	if ( selectionBox && (unsigned)index < selections.size() )
	{
	    if ( selections[index].second )
	    {
		selections[index] = make_pair( selPtr, false );
		NCMIL << "Selection: " << selPtr->name() << " is DEselected" << endl;	
	    }
	    else
	    {
		selections[index] = make_pair( selPtr, true );	
		NCMIL << "Selection: " << selPtr->name() << " is selected" << endl;	
	    }
	}
    }
    
    return event;
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
    bool selected;
    
    for (  it = Y2PM::selectionManager().begin();
	 it != Y2PM::selectionManager().end();
	 ++it )
    {
	selected = false;
	PMSelectionPtr instPtr = (*it)->installedObj();
	PMSelectionPtr candPtr = (*it)->candidateObj();

	if ( instPtr )
	{
	    selPtr = instPtr;
	    selected = true;
	}
	else if ( candPtr )
	{
	    if ( (*it)->status() == PMSelectable::S_Install )
		selected = true;
	    selPtr = candPtr;
	}
	else
	{
	    selPtr = (*it)->theObject(); 
	}
	 // show the available add-ons
	if ( selPtr && selPtr->visible() && !selPtr->isBase() )
	{
	    NCMIL << "Add-on " <<  selPtr->name() << ", selected: " << selected << endl; 
	    selections.push_back( make_pair(selPtr, selected) );
	}
    }
    
    return true;
}
