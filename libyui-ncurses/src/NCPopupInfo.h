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

   File:       NCPopupInfo.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupInfo_h
#define NCPopupInfo_h

#include <iosfwd>

#include <vector>
#include <string>
using namespace std;

#include "NCPopup.h"
#include "NCComboBox.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCPushButton.h"

class YCPValue;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupInfo
//
//	DESCRIPTION :
//
class NCPopupInfo : public NCPopup {

    NCPopupInfo & operator=( const NCPopupInfo & );
    NCPopupInfo            ( const NCPopupInfo & );

private:

    NCRichText * helpText;
    NCPushButton * okButton;
    
protected:

    virtual NCursesEvent wHandleHotkey( int ch );

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupInfo( const wpos at, const YCPString & headline, const YCPString & text );
    virtual ~NCPopupInfo();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline, const YCPString & text );

    void showInfoPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupInfo_h
