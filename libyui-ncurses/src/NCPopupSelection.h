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

   File:       NCPopupSelection.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupSelection_h
#define NCPopupSelection_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCTree.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCMultiSelectionBox.h"
#include "PackageSelector.h"
#include "NCPushButton.h"

#include <y2pm/PMError.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupSelection
//
//	DESCRIPTION :
//
class NCPopupSelection : public NCPopup {

    NCPopupSelection & operator=( const NCPopupSelection & );
    NCPopupSelection            ( const NCPopupSelection & );

private:

    NCPkgTable *sel;			// selection list
    NCPushButton * okButton;

    PackageSelector * packager;		// connection to the PackageSelector,

    //std::vector< std::pair<PMSelectionPtr, bool> > selections;
    std::vector<PMSelectionPtr> selections;	// the available selections
    
    // Get selections from the instsource manager
    bool getSelections( );
    
protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupSelection( const wpos at, PackageSelector * pkg );
    virtual ~NCPopupSelection();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & label );

    bool getSelectedItems( std::vector<int> & list );

    /**
     * Shows the popup with the add ons (package categories).
     * The selected items all stored in NCursesEvent which ir
     * returned if the user exits the popup with OK.
     * @return NCursesEvent
     */
    NCursesEvent & showSelectionPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupSelection_h
