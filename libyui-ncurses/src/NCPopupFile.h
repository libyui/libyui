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

   File:       NCPopupFile.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupFile_h
#define NCPopupFile_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCTextEntry.h"
#include "NCPushButton.h"
#include "PkgNames.h"

class YCPValue;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupFile
//
//	DESCRIPTION :
//
class NCPopupFile : public NCPopup {

    NCPopupFile & operator=( const NCPopupFile & );
    NCPopupFile            ( const NCPopupFile & );

private:

    NCLabel * textLabel;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCTextEntry * fileName;
    
    int hDim;
    int vDim;
    bool visible;
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( int ch );
    
public:
    
    NCPopupFile( const wpos at,
		 const YCPString & headline,
		 const YCPString & text );
    
    virtual ~NCPopupFile();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline,
		       const YCPString & text );

    NCursesEvent & showInfoPopup( );

    bool isVisible( ) { return visible; }
    
    void setNiceSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupFile_h
