/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

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
			       const std::string & startDir,
			       const std::string & headline );

    virtual ~NCAskForExistingDirectory();

    virtual int preferredWidth();
    virtual int preferredHeight();

    /**
     * Create layout of file directory selection popup
     */
    void createLayout( const std::string & initialDir,
		       const std::string & headline );

    /**
     * Shows the popup with the std::list of directories.
     */
    NCursesEvent & showDirPopup( );

};


#endif // NCAskForExistingDirectory_h
