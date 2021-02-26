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

   File:       NCFileSelection.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCFileSelection_h
#define NCFileSelection_h

#include <iosfwd>

#include "NCPadWidget.h"
#include "NCTablePad.h"
#include "NCTable.h"

#include <map>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>


struct NCFileInfo
{
    /**
     * Constructor from a stat buffer (i.e. based on an lstat64() call).
     **/
    NCFileInfo( std::string	fileName,
		struct stat64	* statInfo,
		bool link	= false );

    NCFileInfo();

    ~NCFileInfo() {};

    // Data members.

    std::string		_name;		// the file name (without path!)
    std::string		_realName;	// actual file name
    std::string		_tag;		// short label
    std::string		_perm;		// permission std::string
    std::string		_user;		// user name
    std::string		_group;		// group name
    dev_t		_device;	// device this object resides on
    mode_t		_mode;		// file permissions + object type
    nlink_t		_links;		// number of links
    off64_t		_size;		// size in bytes
    time_t		_mtime;		// modification time

    bool isDir()  { return (( S_ISDIR( _mode ) ) ? true : false ); }

    bool isLink() { return (( S_ISLNK( _mode ) ) ? true : false ); }

    bool isFile() { return (( S_ISREG( _mode ) ) ? true : false ); }
};


/**
 * This class is used for the first column of the file table.
 * Contains the file data.
 **/
class NCFileSelectionTag : public YTableCell
{

private:

    NCFileInfo	* fileInfo;

public:

    NCFileSelectionTag( NCFileInfo 	* info );

    ~NCFileSelectionTag();

    NCFileInfo	* getFileInfo() const		{ return fileInfo; }
};


/**
 * The class which provides methods to handle a std::list of files or directories.
 **/
class NCFileSelection : public NCTable
{
public:
    enum NCFileSelectionType
    {
	T_Overview,
	T_Detailed,
	T_Unknown
    };

private:

    NCFileSelection & operator=( const NCFileSelection & );
    NCFileSelection( const NCFileSelection & );

    // returns the first column of line with 'index' (the tag)
    NCFileSelectionTag * getTag( const int & index );


protected:

    std::string startDir;
    std::string currentDir;
    NCFileSelectionType tableType;	// T_Overview or T_Detailed

    void	setCurrentDir( );
    std::string	getCurrentLine( );

    NCursesEvent handleKeyEvents( wint_t key );

public:

    /**
     * Constructor
     */
    NCFileSelection( YWidget * parent,
		     YTableHeader * tableHeader,
		     NCFileSelectionType type,
		     const std::string & iniDir );

    virtual ~NCFileSelection();

    /**
     * Get the file info.
     * index: The std::list index
     * return: fileInfo Information about the file (directory)
     */
    NCFileInfo * getFileInfo( int index );

    /**
     * Set the type of the table widget
     * type:  Possible values: NCFileSelection::T_Overview, NCFileSelection::T_Detailed
     */
    void setTableType( NCFileSelectionType type ) { tableType = type; };

    virtual void addLine( const std::vector<std::string> & elements,
			  NCFileInfo * fileInfo );

    /**
     * Get number of lines ( std::list entries )
     */
    unsigned int getNumLines( ) { return myPad()->Lines(); }

    /**
     * Draws the file std::list (has to be called after the loop with
     * addLine() calls)
     */
    void drawList( ) { return DrawPad(); }

    /**
     * Clears the package std::list
     */
    virtual void deleteAllItems();

    /**
     * Fills the header of the table
     */
    virtual void fillHeader() = 0;

    /**
     * Creates a line in the package table.
     */
    virtual bool createListEntry( NCFileInfo * fileInfo ) = 0;

    /**
     * Get the current directory
     * return: The currently selected directory
     */
    std::string getCurrentDir() { return currentDir; }

    /**
     * Fill the std::list of diretcories or files
     * Returns 'true' on success.
     */
    virtual bool fillList( ) = 0;

    /**
     * Set the start directory
     */
    void setStartDir( const std::string & start )
    {
	currentDir = start;
	startDir = start;
    }

};


class NCFileTable : public NCFileSelection
{
private:

    std::list<std::string> pattern;	// files must match this pattern
    std::string currentFile;		// currently selected file

public:

    /**
     * Constructor
     */
    NCFileTable( YWidget * parent,
		 YTableHeader * tableHeader,
		 NCFileSelectionType type,
		 const std::string & filter,
		 const std::string & iniDir );

    virtual ~NCFileTable() {}

    void setCurrentFile( const std::string & file )
    {
	currentFile = file;
    }

    bool filterMatch( const std::string & fileName );

    std::string getCurrentFile() { return currentFile; }

    virtual void fillHeader();

    virtual bool createListEntry( NCFileInfo * fileInfo );

    /**
     * Fill the std::list of files
     * Returns 'true' on success.
     */
    virtual bool fillList( );

    virtual NCursesEvent wHandleInput( wint_t key );
};


class NCDirectoryTable : public NCFileSelection
{
public:
    NCDirectoryTable( YWidget * parent,
		      YTableHeader * tableHeader,
		      NCFileSelectionType type,
		      const std::string & iniDir );

    virtual ~NCDirectoryTable() {}

    virtual void fillHeader();

    virtual bool createListEntry( NCFileInfo * fileInfo );

    /**
     * Fill the std::list of directories.
     * Returns 'true' on success.
     */
    virtual bool fillList( );

    virtual NCursesEvent wHandleInput( wint_t key );
};



#endif // NCFileSelection_h
