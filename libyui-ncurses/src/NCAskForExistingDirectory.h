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
#include "NCFileTable.h"
#include "NCPushButton.h"
#include "NCComboBox.h"


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

    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCComboBox * dirName;
    NCFileTable *dirList;		// directory list

    string currentDir;			// currently selected directory
    string startDir;
    
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

    
    /**
     * Create layout of file selection popup
     * @return bool
     */
    void createLayout( const YCPString & headline );

    /**
     * Fills the list with the directories
     * @return bool
     */
    bool fillDirectoryList ( );

    /**
     * Shows the popup with the list of directories.
     * @return NCursesEvent
     */
    NCursesEvent & showDirPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCAskForExistingDirectory_h
