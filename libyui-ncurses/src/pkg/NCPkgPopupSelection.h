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

   File:       NCPkgPopupSelection.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupSelection_h
#define NCPkgPopupSelection_h

#include <iosfwd>

#include <string>

#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCPackageSelector.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupSelection
//
//	DESCRIPTION :
//
class NCPkgPopupSelection : public NCPopup {

    NCPkgPopupSelection & operator=( const NCPkgPopupSelection & );
    NCPkgPopupSelection            ( const NCPkgPopupSelection & );

private:

    NCPkgTable *sel;			// selection list
    NCPushButton * okButton;

    NCPackageSelector * packager;		// connection to the PackageSelector,

protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:

    enum SelType {
	S_Selection,
	S_Pattern,
	S_Language,
	S_Repo,
	S_Unknown
    };
    
    NCPkgPopupSelection( const wpos at, NCPackageSelector * pkg, SelType type );
    virtual ~NCPkgPopupSelection();

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

private:
    
    SelType type;
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupSelection_h
