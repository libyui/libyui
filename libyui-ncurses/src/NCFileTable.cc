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

   File:       NCFileTable.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCFileTable.h"
#include "NCTable.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileInfo::NCFileInfo
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCFileInfo::NCFileInfo( string 	fileName,
			struct stat *	statInfo )
{
    _name   = fileName;
    _mode   = statInfo->st_mode;
    _device = statInfo->st_dev;
    _links  = statInfo->st_nlink;
    _size   = statInfo->st_size;
    _mtime  = statInfo->st_mtime;
    
    if ( S_ISLNK(_mode) )
	_tag = " @";
    else
	_tag = "  ";
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileInfo::NCFileInfo
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCFileInfo::NCFileInfo( )
{
    _name   = "";
    _tag    = "";
    _mode   = (mode_t)0;
    _device = (dev_t)0;
    _links  = (nlink_t)0;
    _size   = (off_t)0;
    _mtime  = (time_t)0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTableTag::NCFileTableTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCFileTableTag::NCFileTableTag( const NCFileInfo & info )
   : NCTableCol( NCstring( "  " ), SEPARATOR )
   , fileInfo( info )
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTableTag::DrawAt
//	METHOD TYPE : virtual DrawAt
//
//	DESCRIPTION :
//
void NCFileTableTag::DrawAt( NCursesWindow & w, const wrect at,
			    NCTableStyle & tableStyle,
			    NCTableLine::STATE linestate,
			    unsigned colidx ) const
{
    NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

    w.addch( at.Pos.L, at.Pos.C, fileInfo._tag.c_str()[0] );
    w.addch( at.Pos.L, at.Pos.C +1, fileInfo._tag.c_str()[1] );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::NCFileTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCFileTable::NCFileTable( NCWidget * parent,
			  YWidgetOpt & opt,
			  NCFileTableType type,
			  const YCPString & iniDir )
    : NCTable( parent, opt, vector<string> () )
      , tableType( type )
      , startDir( iniDir->value() )
      , currentDir( iniDir->value() )
{
    fillHeader();
    SetSepChar( ' ' );

    if ( iniDir->value().empty() )
    {
	size_t bufSize	= 5120 * sizeof(char);	
	char wDir[bufSize+1];
    
	// start with working directory
	if ( getcwd( wDir, bufSize ) )
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
    
    WIDDBG << endl;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::~NCFileTable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCFileTable::~NCFileTable()
{
    WIDDBG << endl;
}

//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected line 
//
string  NCFileTable::getCurrentLine( )
{
    int index = getCurrentItem();
    
    if ( index != -1 )
    {
	NCFileInfo info = getFileInfo( index );
	return info._name;
    }
    else
    {
	return "";
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::setCurrentDir
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFileTable::setCurrentDir()
{
    string selected = getCurrentLine();

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
	if ( ( pos = currentDir.find_last_of("/") ) != 0 )
	{
	    currentDir = currentDir.substr( 0, pos );
	}
	else
	{
	    currentDir = "/";
	}
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::addLine
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFileTable::addLine( const vector<string> & elements,
			   NCFileInfo & info )
{
    vector<NCTableCol*> Items( elements.size()+1, 0 );
    
    // fill first column (containing the status information and the package pointer)
    Items[0] = new NCFileTableTag( info );

    for ( unsigned i = 1; i < elements.size()+1; ++i ) {
	// use YCPString to enforce recoding from 'utf8'
	Items[i] = new NCTableCol( YCPString( elements[i-1] ), NCTableCol::PLAIN );
    }
    pad->Append( Items );
    
    // don't call DrawPad(); for every line - is called once after the loop
  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::itemsCleared
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFileTable::itemsCleared()
{
    return NCTable::itemsCleared();  
}

///////////////////////////////////////////////////////////////////
//
// fillHeader
//
// Fillup the column headers of the package table 
//
void NCFileTable::fillHeader( )
{
    vector<string> header;

    switch ( tableType )
    {
	case T_Overview: {
	    header.reserve(2);
	    header.push_back( "L" + string("  ") );
	    header.push_back( "L" + string("Name") );
	    break;
	}
	case T_Detailed: {
	    header.reserve(5);
	    header.push_back( "L" + string("  ") );
	    header.push_back( "L" + string("Name") );
	    header.push_back( "L" + string("Size") );
	    header.push_back( "L" + string("Type") );
	    header.push_back( "L" + string("Permissions") );
	    break;
	}
	default: {
	    header.reserve(2);
	    header.push_back( "L" + string("   ") );
	    header.push_back( "L" + string("Name") );
	    break;
	}
    }
    setHeader( header );
}


///////////////////////////////////////////////////////////////////
//
// createListEntry
//
//
bool NCFileTable::createListEntry ( NCFileInfo fileInfo )
{
    vector<string> data;

    switch ( tableType )
    {
	case T_Overview: {
	    data.reserve(2);
	    data.push_back( fileInfo._name );
	    break;
	}
	case T_Detailed: {
	    data.reserve(5);
	    data.push_back( fileInfo._name );
	    char size_buf[50];
	    sprintf( size_buf, "%d", fileInfo._size);
	    data.push_back( size_buf );
	    if ( fileInfo.isDir() )
		data.push_back( "directory" );
	    else if ( fileInfo.isLink() )
		data.push_back( "link" );
	    else
		data.push_back( "file" );
	    data.push_back( "read only" );	//FIXME
	    break;
	}
	default: {
	    data.reserve(2);
	    data.push_back( " " );
	    data.push_back( " " );
	    break;
	}
    }
	
    addLine( data, fileInfo );

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable::getFileInfo
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCFileInfo NCFileTable::getFileInfo( int index )
{
    // get the tag 
    NCFileTableTag *cc = getTag( index );
    if ( !cc )
	return NCFileInfo( );

    return cc->getFileInfo();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTableTag::getTag
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCFileTableTag * NCFileTable::getTag( const int & index )
{
    // get the table line 
    NCTableLine * cl = pad->ModifyLine( index );
    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    NCFileTableTag * cc = static_cast<NCFileTableTag *>( cl->GetCol( 0 ) );

    return cc;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFileTable:fillDirectoryList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCFileTable::fillDirectoryList ( )
{
    struct stat 	statInfo;
    struct stat		linkInfo;
    struct dirent *	entry;
    list<string>	tmpList;
    list<string>::iterator   it;

    fillHeader();	// create the column headers
	
    DIR * diskDir = opendir( currentDir.c_str() );
    
    if ( ( diskDir = opendir( currentDir.c_str() ) ) )
    {
	itemsCleared();
	while ( ( entry = readdir( diskDir ) ) )
	{
	    string entryName = entry->d_name;
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
	    string fullName = currentDir + "/" + (*it);
	    if ( lstat( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISDIR( statInfo.st_mode ) )
		{
		    if ( ( (*it) == ".." && currentDir != "/" )
			 || (*it) != ".." )
		    {
			createListEntry( NCFileInfo( (*it), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )	

		{
		    if ( stat( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISDIR( linkInfo.st_mode ) )
			{
			    createListEntry( NCFileInfo( (*it), &statInfo ) );
			}
		    }
		}
	    }	
	    ++it;
	}
	drawList();		// draw the list
	startDir = currentDir;	// set start directory
    }
    else
    {
	NCERR << "ERROR opening directory: " << currentDir << " errno: "
	      << strerror( errno ) << endl;
	return false;
    }
    closedir( diskDir );
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//     METHOD NAME : NCFileTable::wHandleInput
//     METHOD TYPE : NCursesEvent
//
//     DESCRIPTION :
//
NCursesEvent NCFileTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    unsigned int old_pos = getCurrentItem();

    // call handleInput of NCPad
    handleInput( key );

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME: 	{
	    if ( old_pos != 0 )
	    {
		setCurrentDir();
		ret = NCursesEvent::SelectionChanged;
		ret.result = YCPString( currentDir );
	    }
	    break;
	}
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END: {
	    	setCurrentDir();
		ret = NCursesEvent::SelectionChanged;
		ret.result = YCPString( currentDir );
		break;
	}
	case KEY_RETURN:
	case KEY_SPACE: {
	    setCurrentDir();
	    ret = NCursesEvent::Activated;
	    ret.result = YCPString( currentDir );
	    break;
	}
	default:
	    ret = NCursesEvent::none;
    }

    NCMIL << "CURRENT: " << currentDir << " START DIR: " << startDir << endl;
    return ret;
}



