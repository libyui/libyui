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
#include "NCCheckBox.h"


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
    NCCheckBox *detailed;

    bool getCheckBoxValue( NCCheckBox * detailed );
    
protected:

    std::string getCurrentLine();

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCAskForExistingDirectory( const wpos at,
			       const YCPString & startDir,
			       const YCPString & headline );
    virtual ~NCAskForExistingDirectory();

    /**
     * Set the default size
     */
    virtual long nicesize(YUIDimension dim);
    
    /**
     * Create layout of file directory selection popup
     * @param YCPString The initial start directory
     * @param YCPString The headline of the popup
     * @return void
     */
    void createLayout( const YCPString & iniDir,
		       const YCPString & headline );

    /**
     * Fills the list with the directories
     * @return bool Returns true on success
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
