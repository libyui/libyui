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
#define NCPkgPopupRepo_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPadWidget.h"
#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCTablePad.h"
#include "NCZypp.h"

class NCTable;
class NCPushButton;
class NCPackageSelector;

class NCPkgRepoTag : public YTableCell
{

private:

    ZyppRepo repo;

public:
    /**
      * A helper class to hold a reference to zypp::Repository
      * for each repository table line
      * (actually it's a dummy column of the table)
      * @param repo zypp::Repository reference
      */

    NCPkgRepoTag ( ZyppRepo repo);

    //Nikdy, ale opravdu nikdy nenechavej v odvozene tride virtualni
    //destruktor, kdyz ani v puvodni neni, Bublino!
    ~NCPkgRepoTag() {  };

    /*
     * Get repository reference from the line tag
     * @return ZyppRepo
     */

    ZyppRepo getRepo() const		{ return repo; } 	

};

class NCPkgRepoTable : public NCTable
{
private:

    NCPkgRepoTable & operator=( const NCPkgRepoTable & );
    NCPkgRepoTable            ( const NCPkgRepoTable & );

    NCPackageSelector *packager;
public:

    /**
      * A helper class to hold repository data in a neat table
      * widget
      * @param parent A parent widget
      * @param opt Widget options
      */

    NCPkgRepoTable  ( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg);

    virtual ~NCPkgRepoTable() {};

    /**
      * Add one line (with tag) to the repositories table
      * @param ZyppRepo Reference to zypp::Repository
      * @param cols String vector with repository data (name + URL)
      */
    virtual void addLine( ZyppRepo r, const vector<string> & cols );

    /*
     * Fill header of repositories table (name + URL)
     */
    void fillHeader();

    /**
      * Get tag of repository table line on current index,
      * ( contains repository reference)
      * @param index Index of selected table line
      * @return NCPkgRepoTag* Tag of selected line
      */
    NCPkgRepoTag * getTag ( const int & index );

    /**
     * Get repository reference from selected line's tag 
     * @param index Index of selected table line
     * @return ZyppRepo Associated zypp::Repository reference
     */
    ZyppRepo getRepo( int index );

    string showDescription (ZyppRepo r);

    virtual NCursesEvent wHandleInput ( wint_t ch );

    /**
      * Find single zypp::Product for this repository
      * (null product if multiple products found)
      * @param repo zypp::Repository
      * @return ZyppProduct
      */

    ZyppProduct findProductForRepo (ZyppRepo repo);

   /**
      * Add items to the repository list (assoc.
      * product name, if any, and URL)
      * @return bool (always true ;-) )
      */
    bool fillRepoList( );

    bool showRepoPackages( );

};
#endif
