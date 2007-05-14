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

   File:       NCPkgPopupFile.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupFile_h
#define NCPkgPopupFile_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCTextEntry.h"
#include "NCPushButton.h"
#include "NCComboBox.h"


class YCPValue;
class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupFile
//
//	DESCRIPTION :
//
class NCPkgPopupFile : public NCPopup {

    NCPkgPopupFile & operator=( const NCPkgPopupFile & );
    NCPkgPopupFile            ( const NCPkgPopupFile & );

private:

    NCLabel * headline;
    NCRichText * textLabel;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCTextEntry * fileName;
    NCComboBox * comboBox;

    NCPackageSelector * packager;
    
    string pathName;
    bool mountFloppy;
    string floppyDevice;
    
    int hDim;
    int vDim;

    void setDefaultPath();
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPopupFile( const wpos at, string device, NCPackageSelector * pkger );
    
    virtual ~NCPkgPopupFile();

    virtual long nicesize(YUIDimension dim);

    void createLayout( );

    NCursesEvent & showFilePopup( );

    bool mountDevice( string device, string errText );
    void unmount();

    void saveToFile();
    void loadFromFile();
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupFile_h
