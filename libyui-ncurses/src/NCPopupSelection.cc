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

  // get the available selections from the PackageSelector
  vector<string> selections;

  if ( packager )
  {
      selections =  packager->selectionNames( );
  }
  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );

  vector<string>::iterator pos;
  
  opt.notifyMode.setValue( false );

  // the multi selection widget
  selectionBox = new NCMultiSelectionBox( split, opt, label );
  split->addChild( selectionBox );
  int i;
  
  for ( i = 0, pos = selections.begin(); pos != selections.end(); ++pos, i++ )
  {
      // FIXME: get the currently selected add-ons from packagemanager
      // FIXME: get the (translated) description
      selectionBox->addItem( (*pos),			// description
			     PkgNames::Treeitem(),	// `id
			     false );			// selected
  }
  
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


    postevent = NCursesEvent();
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
// bool getSelectedItems ( vector<string> & list )
//
//
bool  NCPopupSelection::getSelectedItems ( vector<string> & selItems )
{
    vector<std::string> items;
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
	    // add this category to the list of selected items
	    string origName = packager->selectionId( index );
	    items.push_back( origName );
	    NCINT << "Selected: " << origName << endl;
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

    return ( packager->selectionId(index) );
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
    bool ok = false;
    vector<string> selItems;
    
    if ( !selectionBox
	 || !postevent.widget )
	return false;

    postevent.item = YCPNull();
     
    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    
    if ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
    {
	// if OK is clicked get the current item and the list of selected items
	postevent.detail = selectionBox->getCurrentItem();

	NCMIL << "GOT INDEX: " << selectionBox->getCurrentItem() << endl;
	
	postevent.item =  getCurrentLine(); 
	ok =  getSelectedItems( selItems );
    
	if ( ok )
	{
	    postevent.itemList = selItems;
	}
	// return false means: close the popup
	return false;
    }
    
    if (postevent == NCursesEvent::cancel)
	return false;

    return true;
}

