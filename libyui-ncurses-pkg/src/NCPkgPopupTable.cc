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

   File:       NCPkgPopupTable.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "YMenuButton.h"
#include "YDialog.h"

#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCPackageSelector.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"

#include "NCi18n.h"

#include <zypp/ui/Selectable.h>
#include <zypp/ui/UserWantedPackages.h>

#include "NCPkgPopupTable.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::NCPkgPopupTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupTable::NCPkgPopupTable( const wpos at, NCPackageSelector * pkger,
                                  std::string headline,
                                  std::string label1,
                                  std::string label2,
                                  bool add_cancel )
    : NCPopup( at, false )
      , pkgTable( 0 )
      , okButton( 0 )
      , cancelButton( 0 )
      , packager( pkger )
{
    createLayout( headline, label1, label2, add_cancel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::~NCPkgPopupTable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupTable::~NCPkgPopupTable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupTable::createLayout( std::string headline,
                                    std::string label1,
                                    std::string label2,
                                    bool add_cancel )
{
    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    new NCSpacing( split, YD_VERT, false, 0.6 );	// stretchable = false

    // the headline of the popup containing a list with packages with status changes
    new NCLabel( split, headline, true, false );	// isHeading = true

    new NCSpacing( split, YD_VERT, false, 0.6 );

    if ( label1 != "" )
    {
        // text part 1
        new NCLabel( split, label1, false, false );
    }
    if ( label2 != "" )
    {
        // text part 2
        new NCLabel( split, label2, false, false );
    }
    YTableHeader * tableHeader = new YTableHeader();
    // add the package table (use default type T_Packages)
    pkgTable = new NCPkgTable( split, tableHeader );
    pkgTable->setPackager( packager );
    pkgTable->fillHeader();

    // HBox for the buttons
    NCLayoutBox * hSplit = new NCLayoutBox( split, YD_HORIZ );
    new NCSpacing( hSplit, YD_HORIZ, true, 0.2 );	// stretchable = true

    // add the OK button
    okButton = new NCPushButton( hSplit, NCPkgStrings::OKLabel() );
    okButton->setFunctionKey( 10 );
    okButton->setKeyboardFocus();

    if ( add_cancel )
    {
        new NCSpacing( hSplit, YD_HORIZ, true, 0.4 );

        // add the Cancel button
        cancelButton = new NCPushButton( hSplit, NCPkgStrings::CancelLabel() );
        cancelButton->setFunctionKey( 9 );
    }
    new NCSpacing( hSplit, YD_HORIZ, true, 0.2 );

    new NCSpacing( split, YD_VERT, false, 0.6 );
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::fillAutoChanges
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupTable::fillAutoChanges( NCPkgTable * pkgTable )
{
    if ( !pkgTable )
	return false;

    pkgTable->itemsCleared();		// clear the table

    std::set<std::string> ignoredNames;
    std::set<std::string> userWantedNames = zypp::ui::userWantedPackageNames();
    //these are the packages already selected for autoinstallation in previous 'verify system' run
    std::set<std::string> verifiedNames = packager->getVerifiedPkgs();

    //initialize storage for the new set
    std::insert_iterator< std::set<std::string> > result (ignoredNames, ignoredNames.begin());

    if(!verifiedNames.empty())
    {
	//if we have some leftovers from previous run, do the union of the sets
	set_union(userWantedNames.begin(), userWantedNames.end(),
	           verifiedNames.begin(), verifiedNames.end(), result );
    }
    else
	//else just take userWanted stuff
	ignoredNames = userWantedNames;

    for ( std::set<std::string>::iterator it = ignoredNames.begin(); it != ignoredNames.end(); ++it )
	yuiMilestone() << "Ignoring: " << *it << endl;

    ZyppPoolIterator
	b = zyppPkgBegin(),
	e = zyppPkgEnd(),
	it;

    for (it = b; it != e; ++it)
    {
	ZyppSel slb = *it;

	// show all packages which are automatically selected for installation
	if ( slb->toModify() && slb->modifiedBy () != zypp::ResStatus::USER )
	{
	    if ( ! inContainer( ignoredNames, slb->name() ) )
	    {
		ZyppPkg pkgPtr = tryCastToZyppPkg (slb->theObj());
		if ( pkgPtr )
		{
		    yuiMilestone() << "The status of " << pkgPtr->name() << " has automatically changed" << endl;
		    pkgTable->createListEntry( pkgPtr, slb );
		    //also add to 'already verified' set
		    packager->insertVerifiedPkg( pkgPtr->name() );
		}
	    }
	}
    }

    pkgTable->drawList();

    if ( pkgTable->getNumLines() > 0 )
    {
	return true;
    }
    else
    {
	return false;
    }
}

bool NCPkgPopupTable::fillAvailables( NCPkgTable * pkgTable, ZyppSel sel )
{
  if ( !pkgTable )
    return false;

  pkgTable->itemsCleared();		// clear the table
  pkgTable->fillAvailableList( sel );

  pkgTable->drawList();

  return true;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::showInfoPopup
//	METHOD TYPE : NCursesEvent event
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupTable::showInfoPopup( )
{
    postevent = NCursesEvent();

    if ( !fillAutoChanges( pkgTable ) )
    {
	postevent = NCursesEvent::button;
	return postevent;
    }

    do {
	// show the popup
	popupDialog( );
    } while ( postAgain() );

    popdownDialog();

    return postevent;
}

NCursesEvent NCPkgPopupTable::showAvailablesPopup( ZyppSel sel )
{
    postevent = NCursesEvent();

    if ( !fillAvailables( pkgTable, sel ) )
    {
	postevent = NCursesEvent::button;
	return postevent;
    }

    do {
	// show the popup
	popupDialog( );
    } while ( postAgain() );

    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPopupTable::preferredWidth()
{
    return NCurses::cols()-15;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupTable::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPopupTable::preferredHeight()
{
    return NCurses::lines()-5;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupTable::wHandleInput( wint_t ch )
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
//	METHOD NAME : NCPkgPopupTable::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupTable::postAgain()
{
    if ( ! postevent.widget )
	return false;

    if ( postevent.widget == cancelButton )
    {
	//user hit cancel - discard set of changes (if not empty)
	packager->clearVerifiedPkgs();

	// close the dialog
	postevent = NCursesEvent::cancel;
    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

