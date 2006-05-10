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

#include <string>

#include "NCPopup.h"
#include "PackageSelector.h"
#include "NCPushButton.h"


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

protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:

    enum SelType {
	S_Selection,
	S_Pattern,
	S_Unknown
    };
    
    NCPopupSelection( const wpos at, PackageSelector * pkg, SelType type );
    virtual ~NCPopupSelection();

    virtual long nicesize(YUIDimension dim);

    
    void createLayout( const YCPString & label );

    /**
     * Fills the list with the available selections (and the status info)
     * @return bool
     */
    bool fillSelectionList ( NCPkgTable * table, SelType type );
    
    /**
     * Shows the popup with the add ons (package categories).
     * @return NCursesEvent
     */
    NCursesEvent & showSelectionPopup( );

    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupSelection_h
