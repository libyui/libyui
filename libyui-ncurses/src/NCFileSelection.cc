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

   File:       NCFileSelection.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCFileSelection.h"
#include "NCTable.h"
#include "NCi18n.h"

#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <string.h> // strerror()
#include <sys/types.h>

/*
  Textdomain "ncurses"
*/



NCFileInfo::NCFileInfo( std::string	fileName,
			struct stat64 *	statInfo,
			bool	link )
{
    _name   = fileName;
    _mode   = statInfo->st_mode;
    _device = statInfo->st_dev;
    _links  = statInfo->st_nlink;
    _size   = statInfo->st_size;
    _mtime  = statInfo->st_mtime;

    if ( link )
    {
	char tmpName[PATH_MAX+1];
	// get actual file name
	int len = readlink( fileName.c_str(), tmpName, PATH_MAX );

	if ( len >= 0 )
	{
	    tmpName[len] = '\0';
	    _realName = tmpName;
	}

	_tag = " @";	// set tag
    }
    else if ( S_ISREG( _mode )
	      && ( _mode & S_IXUSR ) )
	_tag = " *";	// user executable files
    else
	_tag = "  ";

    // get user and group name

    struct passwd * pwdInfo  = getpwuid( statInfo->st_uid );

    if ( pwdInfo )
	_user = pwdInfo->pw_name;

    struct group * groupInfo = getgrgid( statInfo->st_gid );

    if ( groupInfo )
	_group = groupInfo->gr_name;

    if ( _mode & S_IRUSR )
	_perm += "r";
    else
	_perm += "-";

    if ( _mode & S_IWUSR )
	_perm += "w";
    else
	_perm += "-";

    if ( _mode & S_IXUSR )
	_perm += "x";
    else
	_perm += "-";

    if ( _mode & S_IRGRP )
	_perm += "r";
    else
	_perm += "-";

    if ( _mode & S_IWGRP )
	_perm += "w";
    else
	_perm += "-";

    if ( _mode & S_IXGRP )
	_perm += "x";
    else
	_perm += "-";

    if ( _mode & S_IROTH )
	_perm += "r";
    else
	_perm += "-";

    if ( _mode & S_IWOTH )
	_perm += "w";
    else
	_perm += "-";

    if ( _mode & S_IXOTH )
	_perm += "x";
    else
	_perm += "-";
}


NCFileInfo::NCFileInfo( )
{
    _name   = "";
    _realName = "";
    _tag    = "";
    _perm   = "";
    _user   = "";
    _group  = "";
    _mode   = ( mode_t )0;
    _device = ( dev_t )0;
    _links  = ( nlink_t )0;
    _size   = ( off64_t )0;
    _mtime  = ( time_t )0;
}


NCFileSelectionTag::NCFileSelectionTag( NCFileInfo * info )
	: YTableCell( "  " )
	, fileInfo( info )
{
    setLabel( fileInfo->_tag );
}

NCFileSelectionTag::~NCFileSelectionTag()
{
    if ( fileInfo )
    {
	delete fileInfo;
    }
}

NCFileSelection::NCFileSelection( YWidget * parent,
				  YTableHeader * tableHeader,
				  NCFileSelectionType type,
				  const std::string & iniDir )
    : NCTable( parent, tableHeader )
    , startDir( iniDir )
    , currentDir( iniDir )
    , tableType( type )
{
    SetSepChar( ' ' );

    struct stat64 statInfo;

    if ( !iniDir.empty() )
    {
	stat64( iniDir.c_str(), &statInfo );
    }

    if ( iniDir.empty()
	 || !S_ISDIR( statInfo.st_mode ) )
    {
	char wDir[PATH_MAX+1];		// <limits.h>

	// start with working directory

	if ( getcwd( wDir, PATH_MAX ) )
	{
	    startDir = wDir;
	    currentDir = wDir;
	}
	else
	{
	    startDir = "/";
	    currentDir = "/";
	}
    }

    yuiDebug() << std::endl;
}


NCFileSelection::~NCFileSelection()
{
    yuiDebug() << std::endl;
}


std::string	NCFileSelection::getCurrentLine( )
{
    int index = getCurrentItem();

    if ( index != -1 )
    {
	NCFileInfo * info = getFileInfo( index );
	return info->_name;
    }
    else
    {
	return "";
    }
}


void NCFileSelection::setCurrentDir()
{
    std::string selected = getCurrentLine();
    yuiMilestone() << "Current directory: " << selected << std::endl;

    if ( selected != ".." )
    {
	if ( startDir != "/" )
	{
	    currentDir = startDir + "/" + selected;
	}
	else
	{
	    currentDir = startDir + selected;
	}
    }
    else
    {
	size_t pos;

	if (( pos = currentDir.find_last_of( "/" ) ) != 0 )
	{
	    currentDir = currentDir.substr( 0, pos );
	}
	else
	{
	    currentDir = "/";
	}
    }
}


void NCFileSelection::addLine( const std::vector<std::string> & elements,
			       NCFileInfo * info )
{
     YTableItem *tabItem = new YTableItem();

     tabItem->addCell( new NCFileSelectionTag( info ) );

     for ( unsigned i = 1; i < elements.size()+1; ++i ) {
	tabItem->addCell( elements[i-1] );
    }

    // use all-at-once insertion mode - DrawPad() is called only after the loop
    addItem(tabItem, true);
}


void NCFileSelection::deleteAllItems()
{
    return NCTable::deleteAllItems();
}


bool NCFileTable::createListEntry( NCFileInfo * fileInfo )
{
    std::vector<std::string> data;

    switch ( tableType )
    {
	case T_Overview:
	    {
		data.reserve( 2 );
		data.push_back( fileInfo->_name );
		break;
	    }
	case T_Detailed:
	    {
		data.reserve( 6 );
		data.push_back( fileInfo->_name );
		char size_buf[50];
		sprintf( size_buf, "%lld", ( long long int ) fileInfo->_size );
		data.push_back( size_buf );
		data.push_back( fileInfo->_perm );
		data.push_back( fileInfo->_user );
		data.push_back( fileInfo->_group );
		break;
	    }
	default:
	    {
		data.reserve( 2 );
		data.push_back( " " );
		data.push_back( " " );
		break;
	    }
    }

    addLine( data, fileInfo );

    return true;
}


bool NCDirectoryTable::createListEntry( NCFileInfo * fileInfo )
{
    std::vector<std::string> data;

    switch ( tableType )
    {
	case T_Overview:
	    {
		data.reserve( 2 );
		data.push_back( fileInfo->_name );
		break;
	    }
	case T_Detailed:
	    {
		data.reserve( 4 );
		data.push_back( fileInfo->_name );
		data.push_back( fileInfo->_perm );
		data.push_back( fileInfo->_user );
		data.push_back( fileInfo->_group );
		break;
	    }
	default:
	    {
		data.reserve( 2 );
		data.push_back( " " );
		data.push_back( " " );
		break;
	    }
    }

    addLine( data, fileInfo );

    return true;
}


NCFileInfo * NCFileSelection::getFileInfo( int index )
{
    // get the tag
    NCFileSelectionTag *cc = getTag( index );

    if ( !cc )
	return 0;

    return cc->getFileInfo();
}


NCFileSelectionTag * NCFileSelection::getTag( const int & index )
{
    // get the table line
    NCTableLine * cl = myPad()->ModifyLine( index );

    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    YTableItem *it = dynamic_cast<YTableItem*> (cl->origItem() );
    YTableCell *tcell = it->cell(0);
    NCFileSelectionTag * cc = static_cast<NCFileSelectionTag *>( tcell );

    return cc;
}






NCFileTable::NCFileTable( YWidget * parent,
			  YTableHeader * tableHeader,
			  NCFileSelectionType type,
			  const std::string & filter,
			  const std::string & iniDir )
    : NCFileSelection( parent, tableHeader, type, iniDir )
    , currentFile("")
{
    //fillHeader();

    std::string filterStr = filter;
    const std::string delims( " \t" );
    std::string::size_type begin, end;

    begin = filterStr.find_first_not_of( delims );

    while ( begin != std::string::npos )
    {
	end = filterStr.find_first_of( delims, begin );

	if ( end == std::string::npos )
	    end = filterStr.length();

	pattern.push_back( filterStr.substr( begin, end - begin ) );
	begin = filterStr.find_first_not_of( delims, end );
    }
}


/**
 * Fill the column headers of the file table
 **/
void NCFileTable::fillHeader( )
{
    std::vector<std::string> header;
    std::string old_textdomain = textdomain(NULL);
    setTextdomain( "ncurses" );

    switch ( tableType )
    {
	case T_Overview:
	    {
		header.reserve( 2 );
		header.push_back( "L" + std::string( "  " ) );
		// column header name of the file
		header.push_back( "L" + std::string( _( "File name" ) ) );
		break;
	    }

	case T_Detailed:
	    {
		header.reserve( 6 );
		header.push_back( "L" + std::string( "  " ) );
		// column header name of the file
		header.push_back( "L" + std::string( _( "File name" ) ) );
		// column header size of the file
		header.push_back( "L" + std::string( _( "Size" ) ) );
		// column header file permissions
		header.push_back( "L" + std::string( _( "Permissions" ) ) );
		// column header user
		header.push_back( "L" + std::string( _( "User" ) ) );
		// column header group
		header.push_back( "L" + std::string( _( "Group" ) ) );
		break;
	    }

	default:
	    {
		header.reserve( 2 );
		header.push_back( "L" + std::string( "   " ) );
		header.push_back( "L" + std::string( _( "File name" ) ) );
		break;
	    }
    }

    setHeader( header );
    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}


bool NCFileTable::filterMatch( const std::string & fileEntry )
{
    if ( pattern.empty() )
	return true;

    bool match = false;

    std::list<std::string>::iterator it = pattern.begin();

    while ( it != pattern.end() )
    {
	if ( fnmatch(( *it ).c_str(), fileEntry.c_str(), FNM_PATHNAME ) == 0 )
	    match = true;

	++it;
    }

    return match;
}


NCursesEvent NCFileSelection::handleKeyEvents( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    if ( sendKeyEvents() &&
	 ( key == KEY_LEFT || key == KEY_RIGHT ) )
    {
	ret = NCursesEvent::key;

	switch ( key )
	{
	    case KEY_LEFT:
		ret.keySymbol = "CursorLeft";
		break;

	    case KEY_RIGHT:
		ret.keySymbol = "CursorRight";
		break;
	}
    }

    return ret;
}


NCursesEvent NCFileTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = handleKeyEvents( key );

    // return key event

    if ( ret == NCursesEvent::key )
	return ret;

    // call handleInput of NCPad
    handleInput( key );

    currentFile = getCurrentLine();

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END:
	    {
		ret = NCursesEvent::SelectionChanged;
		ret.result = currentFile;
		break;
	    }

	default:
	    ret = NCursesEvent::none;
    }

    yuiDebug() << "CURRENT_FILE: " << currentFile << std::endl;

    return ret;
}


bool NCFileTable::fillList()
{

    struct stat64	statInfo;
    struct stat64	linkInfo;
    struct dirent *	entry;
    std::list<std::string>	tmpList;
    std::list<std::string>::iterator   it;

    fillHeader();	// create the column headers

    DIR * diskDir = opendir( currentDir.c_str() );

    if ( diskDir )
    {
	deleteAllItems();

	while (( entry = readdir( diskDir ) ) )
	{
	    std::string entryName = entry->d_name;

	    if ( entryName != "."
		 && filterMatch( entryName ) )
	    {
		tmpList.push_back( entryName );
	    }
	}

	// sort the list and fill the table widget with file entries
	tmpList.sort( );
	it = tmpList.begin();

	while ( it != tmpList.end() )
	{
	    std::string fullName = currentDir + "/" + ( *it );

	    if ( lstat64( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISREG( statInfo.st_mode ) || S_ISBLK( statInfo.st_mode ) )
		{
		    if ((( *it ) == ".." && currentDir != "/" )
			|| ( *it ) != ".." )
		    {
			createListEntry( new NCFileInfo(( *it ), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )
		{
		    if ( stat64( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISREG( linkInfo.st_mode ) || S_ISBLK( linkInfo.st_mode ) )
			{
			    createListEntry( new NCFileInfo(( *it ), &linkInfo, true ) );
			}
		    }
		}
	    }

	    ++it;
	}

	drawList();		// draw the list

	if ( getNumLines() > 0 )
	{
	    setCurrentItem( 0 );	// set focus to the first list entry
	    currentFile = getCurrentLine();
	}
	else
	{
	    currentFile = "";
	}

	closedir( diskDir );
    }
    else
    {
	yuiError() << "ERROR opening directory: " << currentDir << " errno: "
		   << strerror( errno ) << std::endl;
	return false;
    }

    return true;
}


NCDirectoryTable::NCDirectoryTable( YWidget * parent,
				    YTableHeader * tableHeader,
				    NCFileSelectionType type,
				    const std::string & iniDir )
    : NCFileSelection( parent, tableHeader, type, iniDir )
{
    //fillHeader();
}


/**
 * Fill the column headers of the table
 **/
void NCDirectoryTable::fillHeader()
{
    std::vector<std::string> header;
    std::string old_textdomain = textdomain(NULL);
    setTextdomain( "ncurses" );

    switch ( tableType )
    {
	case T_Overview:
	    {
		header.reserve( 2 );
		header.push_back( "L" + std::string( "  " ) );
		// column header name of diretcory
		header.push_back( "L" + std::string( _( "Directory Name" ) ) );
		break;
	    }

	case T_Detailed:
	    {
		header.reserve( 5 );
		header.push_back( "L" + std::string( "  " ) );
		// column header name of diretcory
		header.push_back( "L" + std::string( _( "Directory Name" ) ) );
		header.push_back( "L" + std::string( _( "Permissions" ) ) );
		header.push_back( "L" + std::string( _( "User" ) ) );
		header.push_back( "L" + std::string( _( "Group" ) ) );
		break;
	    }

	default:
	    {
		header.reserve( 2 );
		header.push_back( "L" + std::string( "   " ) );
		header.push_back( "L" + std::string( _( "Directory Name" ) ) );
		break;
	    }
    }

    setHeader( header );
    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}


bool NCDirectoryTable::fillList()
{
    struct stat64	statInfo;
    struct stat64	linkInfo;
    struct dirent *	entry;
    std::list<std::string>	tmpList;
    std::list<std::string>::iterator   it;

    fillHeader();	// create the column headers

    DIR * diskDir = opendir( currentDir.c_str() );

    if ( diskDir )
    {
	deleteAllItems();

	while (( entry = readdir( diskDir ) ) )
	{
	    std::string entryName = entry->d_name;

	    if ( entryName != "." )
	    {
		tmpList.push_back( entryName );
	    }
	}

	// sort the list and fill the table widget with directory entries
	tmpList.sort( );

	it = tmpList.begin();

	while ( it != tmpList.end() )
	{
	    std::string fullName = currentDir + "/" + ( *it );

	    if ( lstat64( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISDIR( statInfo.st_mode ) )
		{
		    if ((( *it ) == ".." && currentDir != "/" )
			|| ( *it ) != ".." )
		    {
			createListEntry( new NCFileInfo(( *it ), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )
		{
		    if ( stat64( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISDIR( linkInfo.st_mode ) )
			{
			    createListEntry( new NCFileInfo(( *it ), &linkInfo, true ) );
			}
		    }
		}
	    }

	    ++it;
	}

	drawList();		// draw the list
	startDir = currentDir;	// set start directory

	if ( getNumLines() > 0 )
	    setCurrentItem( 0 );	// set focus to the first list entry

	closedir( diskDir );
    }
    else
    {
	yuiError() << "ERROR opening directory: " << currentDir << " errno: "
	<< strerror( errno ) << std::endl;

	return false;
    }

    return true;
}


NCursesEvent NCDirectoryTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = handleKeyEvents( key );

    // return key event

    if ( ret == NCursesEvent::key )
	return ret;

    unsigned int old_pos = getCurrentItem();

    // call handleInput of NCPad
    handleInput( key );

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME:
	    {
		if ( old_pos != 0 )
		{
		    setCurrentDir();
		    ret = NCursesEvent::SelectionChanged;
		    ret.result = currentDir;
		}

		break;
	    }

	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END:
	    {
		setCurrentDir();
		ret = NCursesEvent::SelectionChanged;
		ret.result = currentDir;
		break;
	    }

	case KEY_RETURN:
	case KEY_SPACE:
	    {
		setCurrentDir();
		ret = NCursesEvent::Activated;
		ret.result = currentDir;
		break;
	    }

	default:
	    ret = NCursesEvent::none;
    }

    yuiDebug() << "CURRENT: " << currentDir << " START DIR: " << startDir << std::endl;

    return ret;
}



