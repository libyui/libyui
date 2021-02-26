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

   File:       NCPkgFilterPattern.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgFilterPattern_h
#define NCPkgFilterPattern_h

#include <iosfwd>

#include <string>
#include <set>

#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCPackageSelector.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterPattern
//
//	DESCRIPTION :
//
class NCPkgFilterPattern : public NCPkgTable {

    NCPkgFilterPattern & operator=( const NCPkgFilterPattern & );
    NCPkgFilterPattern            ( const NCPkgFilterPattern & );

private:

    NCPackageSelector * packager;		// connection to the PackageSelector,

protected:

    std::string getCurrentLine();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
        
    NCPkgFilterPattern( YWidget *parent, YTableHeader *header, NCPackageSelector * pkg );
    virtual ~NCPkgFilterPattern();
    
    void createLayout( YWidget *parent );

    /**
     * Fills the list with the available selections (and the status info)
     * @return bool
     */
    bool fillPatternList ( );

    string showDescription( ZyppObj objPtr );	
   
    /**
     * Shows the popup with the add ons (package categories).
     * @return NCursesEvent
     */
    void showPatternPackages( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterPattern_h
