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

   File:       NCAskForExistingDirectory.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCAskForExistingDirectory_h
#define NCAskForExistingDirectory_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCSelectionBox.h"
#include "NCPushButton.h"
#include "NCTextEntry.h"

#include <y2pm/PMError.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCAskForExistingDirectory
//
//	DESCRIPTION :
//
class NCAskForExistingDirectory : public NCPopup {

    NCAskForExistingDirectory & operator=( const NCAskForExistingDirectory & );
    NCAskForExistingDirectory            ( const NCAskForExistingDirectory & );

private:

    NCSelectionBox *dir;		// directory list
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCTextEntry * dirName;
    
protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCAskForExistingDirectory( const wpos at,
			       const YCPString & startDir,
			       const YCPString & headline );
    virtual ~NCAskForExistingDirectory();

    virtual long nicesize(YUIDimension dim);

    
    void createLayout( const YCPString & startDir, const YCPString & headline );

    /**
     * Fills the list with the directories
     * @return bool
     */
    bool fillDirectoryList ( NCSelectionBox * dir, const YCPString & startDir );
	
    /**
     * Shows the popup with the list of directories.
     * @return NCursesEvent
     */
    NCursesEvent & showDirPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCAskForExistingDirectory_h
