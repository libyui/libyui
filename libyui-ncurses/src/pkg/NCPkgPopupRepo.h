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

#include "NCPadWidget.h"
#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCTablePad.h"
#include "NCPackageSelector.h"

class NCPopup;
class NCPushButton;
class NCPackageSelector;

class NCPkgRepoTag : public NCTableCol
{

private:

    ZyppRepo repo;

public:

    NCPkgRepoTag ( ZyppRepo repo);

    virtual ~NCPkgRepoTag() {};

    ZyppRepo getRepo() const		{ return repo; } 	

};

class NCPkgRepoTable : public NCTable
{
private:

    NCPkgRepoTable & operator=( const NCPkgRepoTable & );
    NCPkgRepoTable            ( const NCPkgRepoTable & );

public:

    NCPkgRepoTable (NCWidget *parent, const YWidgetOpt & opt);

    virtual ~NCPkgRepoTable() {};

    virtual void addLine( ZyppRepo r, const vector<string> & cols );

    void fillHeader();

    NCPkgRepoTag * getTag ( const int & index );

    ZyppRepo getRepo( int index );

};

class NCPkgPopupRepo : public NCPopup {

    NCPkgPopupRepo & operator=( const NCPkgPopupRepo & );
    NCPkgPopupRepo	      ( const NCPkgPopupRepo &);

private:
    NCPkgRepoTable *repolist;
    NCPushButton *okButton;
    NCPackageSelector *packager;

protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput ( wint_t ch );

public: 

    NCPkgPopupRepo ( const wpos at, NCPackageSelector *pkger);

    virtual ~NCPkgPopupRepo();

    ZyppProduct findProductForRepo (ZyppRepo repo);

    virtual long nicesize( YUIDimension dim );

    void createLayout( const YCPString & label );    

    bool fillRepoList( );

    NCursesEvent & showRepoPopup();
};
#endif
