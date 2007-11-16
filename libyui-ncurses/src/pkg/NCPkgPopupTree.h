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

   File:       NCPkgPopupTree.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupTree_h
#define NCPkgPopupTree_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCTree.h"
#include "NCLabel.h"
#include "NCRichText.h"

#include <y2util/YRpmGroupsTree.h>

class NCTree;
class NCPackageSelector;
class NCRpmGroupItem;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupTree
//
//	DESCRIPTION :
//
class NCPkgPopupTree : public NCPopup {

    NCPkgPopupTree & operator=( const NCPkgPopupTree & );
    NCPkgPopupTree            ( const NCPkgPopupTree & );

private:

    NCTree * filterTree;		// the YTree
    
    NCPackageSelector * packager;		// connection to the PackageSelector

    // internal use (copies tree items)
    void cloneTree( YStringTreeItem * parentOrig, NCRpmGroupItem * parentClone );

protected:

    void setCurrentItem( int index );

    virtual NCursesEvent wHandleHotkey( wint_t ch );

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPopupTree( const wpos at, NCPackageSelector * packager );
    virtual ~NCPkgPopupTree();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    void createLayout( const string & label );

    void addItem( YTreeItem * newItem );

    NCursesEvent showFilterPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupTree_h
