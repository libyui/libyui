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

   File:       NCFileTable.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCFileTable_h
#define NCFileTable_h

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
      * Constructor from a stat buffer (i.e. based on an lstat() call).
      **/
     NCFileInfo( string 	fileName,
		 struct stat *	statInfo );

     NCFileInfo ();
     
     ~NCFileInfo() {};

     // Data members.

     string		_name;		// the file name (without path!)

     string		_tag;		// short label 
     dev_t		_device;	// device this object resides on
     mode_t		_mode;		// file permissions + object type
     nlink_t		_links;		// number of links
     off_t		_size;		// size in bytes
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
class NCFileTableTag : public NCTableCol {

  private:

    NCFileInfo	 fileInfo;

public:

    NCFileTableTag( const NCFileInfo	& info);

    virtual ~NCFileTableTag() {}

    virtual void SetLabel( const NCstring & ) { /*NOOP*/; }

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
class NCFileTable : public NCTable {

public:
    enum NCFileTableType {
	T_Overview,
	T_Detailed,
	T_Unknown
    };

private:

    NCFileTable & operator=( const NCFileTable & );
    NCFileTable            ( const NCFileTable & );

    NCFileTableType tableType;	// the type

    // returns the first column of line with 'index' (the tag)
    NCFileTableTag * getTag ( const int & index );

protected:


public:

   /**
    * Constructor
    */
    NCFileTable( NCWidget * parent, YWidgetOpt & opt, NCFileTableType type );

    virtual ~NCFileTable();

    /**
     * Get the file info.
     * @param index The list index
     * @return fileInfo Information about the file (directory)
     */
    NCFileInfo  getFileInfo( int index );

   /**
    * This method is called to add a line to the file list.
    * @param status The file type (first column of the table)
    * @param elements A vector<string> containing the package data
    * @param objPtr The pointer to the packagemanager object
    * @return void
    */
    virtual void addLine( const vector<string> & elements,
			  NCFileInfo & fileInfo );

   /**
     * Draws the file list (has to be called after the loop with
     * addLine() calls)
     */ 
   void drawList( ) { return DrawPad(); }
    
   /**
    * Clears the package list
    */
    virtual void itemsCleared();

    /**
     * Handles the events concerning the list (e.g. scroll the list,
     * get the current list entry ...)
     * @param key The key which is pressed
     * @return NCursesEvent
     */
    virtual NCursesEvent wHandleInput( wint_t key );

   /**
     * Returns the number of lines in the table (the table size)
     * @return unsigned int
     */ 
    unsigned int getNumLines( ) { return pad->Lines(); }

    /**
     * Fills the header of the table
     * @return void
     */  
    void fillHeader( );

    /**
     * Creates a line in the package table.
     * @param pkgPtr The package pointer 
     * @return bool
     */  
   bool createListEntry ( NCFileInfo fileInfo );

 
};

///////////////////////////////////////////////////////////////////

#endif // NCFileTable_h
