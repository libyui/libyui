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

   File:       NCPopupTree.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupTree_h
#define NCPopupTree_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCTree.h"
#include "NCLabel.h"
#include "NCRichText.h"

#include <y2util/YRpmGroupsTree.h>

class NCTree;
class YCPValue;
class PackageSelector;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupTree
//
//	DESCRIPTION :
//
class NCPopupTree : public NCPopup {

    NCPopupTree & operator=( const NCPopupTree & );
    NCPopupTree            ( const NCPopupTree & );

private:

    NCTree * filterTree;		// the YTree
    
    PackageSelector * packager;		// connection to the PackageSelector
    
    // internal use (copies tree items got from YPkgRpmGroupTagsFilterView)
    void cloneTree( YStringTreeItem * parentOrig, YTreeItem * parentClone );

protected:

    void setCurrentItem( int index );

    virtual NCursesEvent wHandleHotkey( int ch );

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupTree( const wpos at, PackageSelector * packager );
    virtual ~NCPopupTree();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & label );

    YTreeItem * addItem( YTreeItem * 		parentItem,
			 const YCPString & 	text,
			 void * 		data,
			 bool  			open );

    NCursesEvent showFilterPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupTree_h
