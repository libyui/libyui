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

   File:       NCAskForFile.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCAskForFile_h
#define NCAskForFile_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include "NCFileSelection.h"
#include "NCPushButton.h"
#include "NCComboBox.h"
#include "NCCheckBox.h"
#include "NCInputField.h"



class NCAskForFile : public NCPopup
{
private:

    NCAskForFile & operator=( const NCAskForFile & );
    NCAskForFile( const NCAskForFile & );


    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCComboBox * dirName;		// the selected directory
    NCDirectoryTable *dirList;		// the directory list
    NCCheckBox *detailed;		// the 'Details' checkbox

    bool getCheckBoxValue( NCCheckBox * detailed );

    std::string iniFileName;			// initial file name

protected:

    NCFileTable *fileList;		// the file list
    NCInputField *fileName;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

    virtual std::string getFileName() = 0;

    std::string checkIniDir( std::string startDir );

public:

    NCAskForFile( const wpos at,
		  const std::string & startDir,
		  const std::string & filter,
		  const std::string & headline );

    virtual ~NCAskForFile() = 0;

    /**
     * Set the default size
     */
    virtual int preferredWidth();
    virtual int preferredHeight();

    /**
     * Create layout of file selection popup
     * iniDir:	 The initial start directory
     * filter:	 pattern what files to show
     * headline: popup headline
     * editable: file name field editable?
     */
    void createLayout( const std::string & iniDir,
		       const std::string & filter,
		       const std::string & headline,
		       bool  editable );

    /**
     * Shows the popup with the std::list of directories.
     */
    NCursesEvent & showDirPopup( );

    /**
     * Show new file information
     */
    void updateFileList();

};


class NCAskForExistingFile : public NCAskForFile
{

    NCAskForExistingFile & operator=( const NCAskForFile & );
    NCAskForExistingFile( const NCAskForFile & );

public:

    NCAskForExistingFile( const wpos at,
			  const std::string & startDir,
			  const std::string & filter,
			  const std::string & headline );

    virtual ~NCAskForExistingFile() {}

protected:

    virtual std::string getFileName();
};


class NCAskForSaveFileName : public NCAskForFile
{

    NCAskForSaveFileName & operator=( const NCAskForFile & );
    NCAskForSaveFileName( const NCAskForFile & );

public:

    NCAskForSaveFileName( const wpos at,
			  const std::string & startDir,
			  const std::string & filter,
			  const std::string & headline );

    virtual ~NCAskForSaveFileName() {}

protected:

    virtual std::string getFileName();

};



#endif // NCAskForFile_h
