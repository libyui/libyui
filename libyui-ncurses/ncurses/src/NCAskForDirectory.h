/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
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



class NCAskForExistingDirectory : public NCPopup
{
private:

    NCAskForExistingDirectory & operator=( const NCAskForExistingDirectory & );
    NCAskForExistingDirectory( const NCAskForExistingDirectory & );

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

    virtual int preferredWidth();
    virtual int preferredHeight();

    /**
     * Create layout of file directory selection popup
     */
    void createLayout( const string & initialDir,
		       const string & headline );

    /**
     * Shows the popup with the list of directories.
     */
    NCursesEvent & showDirPopup( );

};


#endif // NCAskForExistingDirectory_h
