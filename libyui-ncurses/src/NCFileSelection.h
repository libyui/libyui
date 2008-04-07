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
     NCFileInfo( string 	fileName,
		 struct stat64	* statInfo,
		 bool link 	= false );

     NCFileInfo ();
     
     ~NCFileInfo() {};

     // Data members.

     string		_name;		// the file name (without path!)
     string		_realName; 	// actual file name
     string		_tag;		// short label 
     string		_perm;		// permission string
     string 		_user;		// user name
     string 		_group;		// group name
     dev_t		_device;	// device this object resides on
     mode_t		_mode;		// file permissions + object type
     nlink_t		_links;		// number of links
     off64_t		_size;		// size in bytes
     time_t		_mtime;		// modification time

     bool isDir()  { return ( (S_ISDIR(_mode)) ? true : false ); }
     bool isLink() { return ( (S_ISLNK(_mode)) ? true : false ); }
     bool isFile() { return ( (S_ISREG(_mode)) ? true : false ); }
};


/**
 * This class is used for the first column of the file table.
 * Contains the file data.
 *
 **/
class NCFileSelectionTag : public NCTableCol {

  private:

    NCFileInfo	 fileInfo;

  public:

    NCFileSelectionTag( const NCFileInfo	& info);

    virtual ~NCFileSelectionTag() {}

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const; 

    NCFileInfo  getFileInfo() const		{ return fileInfo; }
};

/**
 * The class which provides methods to handle a list of files or directories.
 *
 **/
class NCFileSelection : public NCTable {

public:
    enum NCFileSelectionType {
	T_Overview,
	T_Detailed,
	T_Unknown
    };

private:

    NCFileSelection & operator=( const NCFileSelection & );
    NCFileSelection            ( const NCFileSelection & );

    // returns the first column of line with 'index' (the tag)
    NCFileSelectionTag * getTag ( const int & index );

    
protected:

    string startDir;
    string currentDir;
    NCFileSelectionType tableType;	// T_Overview or T_Detailed 

    void 	setCurrentDir( );
    string 	getCurrentLine( );

    NCursesEvent handleKeyEvents( wint_t key );
    
public:

   /**
    * Constructor
    */
    NCFileSelection( YWidget * parent,
		     YTableHeader * tableHeader,
		     NCFileSelectionType type,
		     const string & iniDir );

    virtual ~NCFileSelection();

    /**
     * Get the file info.
     * @param index The list index
     * @return fileInfo Information about the file (directory)
     */
    NCFileInfo  getFileInfo( int index );

    /**
     * Set the type of the table widget
     * @param type Possible values: NCFileSelection::T_Overview, NCFileSelection::T_Detailed
     */
    void setTableType( NCFileSelectionType type ) { tableType = type; };
    
   /**
    * This method is called to add a line to the file list.
    * @param status The file type (first column of the table)
    * @param elements A vector<string> containing the package data
    * @param objPtr The pointer to the packagemanager object
    * @return void
    */
    virtual void addLine( const vector<string> & elements,
			  const NCFileInfo & fileInfo );

    /**
     * Get number of lines ( list entries )
     */ 
    unsigned int getNumLines( ) { return myPad()->Lines(); }

   /**
     * Draws the file list (has to be called after the loop with
     * addLine() calls)
     */ 
   void drawList( ) { return DrawPad(); }
    
   /**
    * Clears the package list
    */
    virtual void deleteAllItems();

    /**
     * Fills the header of the table
     * @return void
     */  
    virtual void fillHeader( ) = 0;

    /**
     * Creates a line in the package table.
     * @param pkgPtr The package pointer 
     * @return bool
     */  
    virtual bool createListEntry ( const NCFileInfo & fileInfo ) = 0;

    /**
     * Get the current directory
     * @return string The currently selected directory
     */ 
    string getCurrentDir() { return currentDir; }

    /**
     * Fill the list of diretcories or files
     * @return bool List successfully filled
     */  
    virtual bool fillList ( ) = 0;

    /**
     * Set the start directory
     */
    void setStartDir( const string & start ) {
	currentDir = start;
	startDir = start;
    }
	
};

class NCFileTable : public NCFileSelection {

private:

    list<string> pattern;	// files must match this pattern
    string currentFile;		// currently selected file

public:

  /**
    * Constructor
    */
    NCFileTable( YWidget * parent,
		 YTableHeader * tableHeader,
		 NCFileSelectionType type,
		 const string & filter,
		 const string & iniDir );

    virtual ~NCFileTable(){}

    void setCurrentFile( const string & file ) {
	currentFile = file;
    }

    bool filterMatch( const string & fileName );
    
    string getCurrentFile() { return currentFile; }
    
    virtual void fillHeader();

    virtual bool createListEntry( const NCFileInfo & fileInfo );
    
    /**
     * Fill the list of files
     * @return bool List successfully filled
     */  
    virtual bool fillList ( );

    virtual NCursesEvent wHandleInput( wint_t key ); 
};


class NCDirectoryTable : public NCFileSelection {

public:
  /**
    * Constructor
    */
    NCDirectoryTable( YWidget * parent,
		      YTableHeader * tableHeader,
		      NCFileSelectionType type,
		      const string & iniDir );

    virtual ~NCDirectoryTable(){}

    virtual void fillHeader();

    virtual bool createListEntry( const NCFileInfo & fileInfo );

   /**
     * Fill the list of directories
     * @return bool List successfully filled
     */  
    virtual bool fillList ( );

    virtual NCursesEvent wHandleInput( wint_t key );

};



#endif // NCFileSelection_h
