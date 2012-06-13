/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    string iniFileName;			// initial file name

protected:

    NCFileTable *fileList;		// the file list
    NCInputField *fileName;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

    virtual string getFileName() = 0;

    string checkIniDir( string startDir );

public:

    NCAskForFile( const wpos at,
		  const string & startDir,
		  const string & filter,
		  const string & headline );

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
    void createLayout( const string & iniDir,
		       const string & filter,
		       const string & headline,
		       bool  editable );

    /**
     * Shows the popup with the list of directories.
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
			  const string & startDir,
			  const string & filter,
			  const string & headline );

    virtual ~NCAskForExistingFile() {}

protected:

    virtual string getFileName();
};


class NCAskForSaveFileName : public NCAskForFile
{

    NCAskForSaveFileName & operator=( const NCAskForFile & );
    NCAskForSaveFileName( const NCAskForFile & );

public:

    NCAskForSaveFileName( const wpos at,
			  const string & startDir,
			  const string & filter,
			  const string & headline );

    virtual ~NCAskForSaveFileName() {}

protected:

    virtual string getFileName();

};



#endif // NCAskForFile_h
