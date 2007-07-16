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

   File:       NCPkgPopupRepo.h

   Author:     Bubli <kmachalkova@suse.cz> 

/-*/

#ifndef NCPkgPopupRepo_h
#define NCPKGPopupRepo_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCPackageSelector.h"

class NCPopup;
class NCPushButton;
class NCPackageSelector;

class NCPkgPopupRepo : public NCPopup {

    NCPkgPopupRepo & operator=( const NCPkgPopupRepo & );
    NCPkgPopupRepo	      ( const NCPkgPopupRepo &);

private:
    NCTable *repolist;
    NCPushButton *okButton;
    NCPackageSelector *packager;

protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput ( wint_t ch );

public: 

    NCPkgPopupRepo ( const wpos at, NCPackageSelector *pkger);

    virtual ~NCPkgPopupRepo();

    virtual long nicesize( YUIDimension dim );

    void createLayout( const YCPString & label );    

//    bool fillRepoList( );

    void fillHeader();

    NCursesEvent & showRepoPopup();
};
#endif
