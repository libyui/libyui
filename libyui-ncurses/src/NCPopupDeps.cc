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

   File:       NCPopupDeps.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupDeps.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::NCPopupDeps
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupDeps::NCPopupDeps( const wpos at, PMObjectPtr pkgPtr )
    : NCPopup( at, false )
      , cancelButton( 0 )
{
    
    if ( pkgPtr )
    {
	PMSelectablePtr selPtr = pkgPtr->getSelectable();
	string pkgName = selPtr->name();

	// get the dependencies for the certain package
	requires = pkgPtr->requires();
	conflicts = pkgPtr->conflicts();  
    }
    
    createLayout( PkgNames::PackageDeps() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::~NCPopupDeps
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupDeps::~NCPopupDeps()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDeps::createLayout( const YCPString & headline )
{

  YWidgetOpt opt;
 
  // horizontal split is the (only) child of the dialog
  NCSplit * hSplit = new NCSplit( this, opt, YD_HORIZ );
  addChild( hSplit );

  opt.hWeight.setValue( 40 );

  // the help text is the first child of the horizontal split
  
  opt.hWeight.setValue( 60 );
  opt.notifyMode.setValue( false );
  
  // second child is a vertical split
  NCSplit * vSplit = new NCSplit( hSplit, opt, YD_VERT );
  
  hSplit->addChild( vSplit );

  NCSpacing * sp0 = new NCSpacing( vSplit, opt, 0.2, false, true );
  vSplit->addChild( sp0 );
  
  // add the headline
  opt.isHeading.setValue( true );
  NCLabel * head = new NCLabel( vSplit, opt, headline );
  vSplit->addChild( head );

  NCSpacing * sp = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp );
  
  // add the input field (a editable combo box) 
  opt.isEditable.setValue( true );
  opt.isHStretchable.setValue( true );

  NCSpacing * sp1 = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp1 );
  
  // add the check boxes
  opt.isHStretchable.setValue( true );
  NCSpacing * sp2 = new NCSpacing( vSplit, opt, 0.4, false, true );
  NCSpacing * sp3 = new NCSpacing( vSplit, opt, 0.8, false, true );

  vSplit->addChild( sp2 );
  vSplit->addChild( sp3 );
  
  // add the cancel button
  opt.isHStretchable.setValue( false );
  cancelButton = new NCPushButton( vSplit, opt, YCPString( "Cancel" ) );
  cancelButton->setId( PkgNames::Cancel () );
  
  vSplit->addChild( cancelButton );
  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::createDependenyLayout
//	METHOD TYPE : void
//
//	DESCRIPTION : create the dependeny layout for a single package
//
/*
void NCPopupDeps::createDependenyLayout( NCWidget * parent, string pkgName )
{

    
}
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::showDependencyPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
YCPString NCPopupDeps::showDependencyPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    return YCPString(postevent.item);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupDeps::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 50 : 15 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupDeps::wHandleInput( int ch )
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
//	METHOD NAME : NCPopupDeps::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupDeps::postAgain()
{
    if ( ! postevent.widget )
	return false;
    
    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    
    if ( currentId->compare( PkgNames::Cancel () ) == YO_EQUAL )
    {
	// dummy
    }
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

