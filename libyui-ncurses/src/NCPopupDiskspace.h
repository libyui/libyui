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

   File:       NCPopupDiskspace.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupDiskspace_h
#define NCPopupDiskspace_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"

class NCTable;
class NCPushButton;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupDiskspace
//
//	DESCRIPTION :
//
class NCPopupDiskspace : public NCPopup {

    NCPopupDiskspace & operator=( const NCPopupDiskspace & );
    NCPopupDiskspace            ( const NCPopupDiskspace & );

private:

    NCTable * partitions;
    NCPushButton * okButton;
    int hDim;
    int vDim;

    string usedPercent( FSize used, FSize total );
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPopupDiskspace( const wpos at);
    
    virtual ~NCPopupDiskspace();

    virtual long nicesize(YUIDimension dim);

    void fillPartitionTable();

    string checkDiskSpace();
    
    void createLayout( const YCPString & headline );

    void showInfoPopup( );

    void setNiceSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDiskspace_h
