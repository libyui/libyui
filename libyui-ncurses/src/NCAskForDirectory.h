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

/-*/
#ifndef NCAskForExistingDirectory_h
#define NCAskForExistingDirectory_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCFileSelection.h"
#include "NCPushButton.h"
#include "NCComboBox.h"
#include "NCCheckBox.h"



class NCAskForExistingDirectory : public NCPopup {

    NCAskForExistingDirectory & operator=( const NCAskForExistingDirectory & );
    NCAskForExistingDirectory            ( const NCAskForExistingDirectory & );

private:

    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCComboBox * dirName;
    NCDirectoryTable *dirList;		// directory list
    NCCheckBox *detailed;

    bool getCheckBoxValue( NCCheckBox * detailed );
    
protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCAskForExistingDirectory( const wpos at,
			       const string & startDir,
			       const string & headline );

    virtual ~NCAskForExistingDirectory();

    /**
     * Set the default size
     */
    
    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Create layout of file directory selection popup
     * @param string The initial start directory
     * @param string The headline of the popup
     * @return void
     */
    void createLayout( const string & iniDir,
		       const string & headline );

    /**
     * Shows the popup with the list of directories.
     * @return NCursesEvent
     */
    NCursesEvent & showDirPopup( );

};



#endif // NCAskForExistingDirectory_h
