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

   File:       NCPopupDeps.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupDeps_h
#define NCPopupDeps_h

#include <iosfwd>

#include <vector>
#include <string>

using namespace std;

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCPushButton.h"

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupDeps
//
//	DESCRIPTION :
//
class NCPopupDeps : public NCPopup {

    NCPopupDeps & operator=( const NCPopupDeps & );
    NCPopupDeps            ( const NCPopupDeps & );

private:

    list<PkgRelation> requires;		// additional required packages
    list<PkgRelation> conflicts;	// package conflicts

    NCPushButton * cancelButton;
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupDeps( const wpos at, PMObjectPtr pkgPtr );
    virtual ~NCPopupDeps();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline );

    YCPString showDependencyPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDeps_h
