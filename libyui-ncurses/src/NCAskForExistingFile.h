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

   File:       NCAskForExistingFile.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCAskForExistingFile_h
#define NCAskForExistingFile_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCFileSelection.h"
#include "NCPushButton.h"
#include "NCComboBox.h"
#include "NCCheckBox.h"
#include "NCTextEntry.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCAskForExistingFile
//
//	DESCRIPTION :
//
class NCAskForExistingFile : public NCPopup {

    NCAskForExistingFile & operator=( const NCAskForExistingFile & );
    NCAskForExistingFile            ( const NCAskForExistingFile & );

private:

    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCComboBox * dirName;
    NCDirectoryTable *dirList;		// directory list
    NCFileTable *fileList;		// file list
    NCCheckBox *detailed;

    NCTextEntry *fileName;
    
    bool getCheckBoxValue( NCCheckBox * detailed );
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCAskForExistingFile( const wpos at,
			  const YCPString & startDir,
			  const YCPString & headline );

    virtual ~NCAskForExistingFile();

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
     * Shows the popup with the list of directories.
     * @return NCursesEvent
     */
    NCursesEvent & showDirPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCAskForExistingFile_h
