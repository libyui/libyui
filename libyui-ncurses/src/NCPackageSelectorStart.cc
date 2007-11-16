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

   File:       NCPackageSelectorStart.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPackageSelectorStart.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"
#include "NCLabel.h"
#include "NCPkgNames.h"
#include "NCi18n.h"

#include <libintl.h>

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>

#include "YPackageSelector.h"
#include "YCPDialogParser.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::NCPackageSelectorStart
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPackageSelectorStart::NCPackageSelectorStart( YWidget * parent,
						long modeFlags,
						YUIDimension dimension )
    : NCLayoutBox( parent, dimension )
      , widgetRoot( 0 )
      , packager( 0 )
      , youMode ( false )
      , updateMode ( false )
{
    YNCursesUI * ui = YNCursesUI::ui();
    
    // set the textdomain
    setTextdomain( "packages" );
    
    // get the mode (the mode is also available in PackageSelector via modeFlags)
    if ( modeFlags & YPkg_OnlineUpdateMode )
	youMode = true;

    if ( modeFlags & YPkg_UpdateMode )
	updateMode = true;
    
    // Read the layout file and create the widget tree
    YCPTerm pkgLayout = YCPNull();
    if ( youMode )
    {
	pkgLayout = readLayoutFile( "/usr/share/YaST2/data/you_layout.ycp" );
    }
    else
    { 
	pkgLayout = readLayoutFile( "/usr/share/YaST2/data/pkg_layout.ycp" );
	//pkgLayout = readLayoutFile( "/usr/share/YaST2/data/test_layout.ycp" );
    }

    if ( ! pkgLayout.isNull() && ui )
    {
	// widgetRoot = (YContainerWidget *)ui->createWidgetTree( dynamic_cast<YWidget *>(parent),
	//						       childrenOpt, 0, pkgLayout );

	widgetRoot = YCPDialogParser::parseWidgetTreeTerm( this,
							   pkgLayout );
    }

    // create the PackageSelector (creation with 'new' is required because initialization
    // in the list of member variables causes problems with untranslated messages)

    if ( widgetRoot )
    {
	NCMIL <<  "Widget tree of NCPackageSelector created" << endl;

	packager = new NCPackageSelector( ui, widgetRoot, modeFlags );

	// child handling is done by libyui now
	//this->addChild( widgetRoot );
	//widgetRoot->setParent( this );

	// get the widget ID of the package table
	YWidget * pkg = YCPDialogParser::findWidgetWithId( widgetRoot, NCPkgNames::Packages() );
	pkgList = dynamic_cast<NCPkgTable *>(pkg);

	if ( pkgList )
	{
	    NCMIL << "Set package table type" << endl;
	    // set the type of the table
	    NCPkgStatusStrategy * strategy;
	    if ( youMode )
	    {
		strategy = new PatchStatStrategy();

		pkgList->setTableType( NCPkgTable::T_Patches, strategy );
	    }
	    else if ( updateMode )
	    {
		strategy = new UpdateStatStrategy();
		pkgList->setTableType( NCPkgTable::T_Update, strategy );
	    }
	    else
	    {
		strategy = new PackageStatStrategy();
		pkgList->setTableType( NCPkgTable::T_Packages, strategy );
	    }

            // set the pointer to the packager object
	    pkgList->setPackager( packager );

	    // fill the table header
	    pkgList->fillHeader( );
	}
    }
    else
    {
	NCERR << "ERROR: could not create the NCPackageSelector" << endl;
    }

    WIDDBG << endl;
    wstate = NC::WSdumb;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::~NCPackageSelectorStart
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPackageSelectorStart::~NCPackageSelectorStart()
{
    if ( packager )
    {
	delete packager;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorStart::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPackageSelectorStart::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YLayoutBox::setSize( newwidth, newheight );
}


///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelectorStart::showDefaultList
//      METHOD TYPE : void
//
//      DESCRIPTION : fill up the package table with default data
//		     
//
void NCPackageSelectorStart::showDefaultList()
{
    // fill the package table with packages belonging to the default filter
    if ( pkgList )
    {
	if ( packager )
	{
	    packager->createPopups();
	}
	
	// fill the list with packages (or patches)
	pkgList->fillDefaultList( );

	pkgList->setKeyboardFocus();

	if ( packager )
	{
	    // always do an initial dependency solving
	    packager->showPackageDependencies( true );

	    if ( youMode )
		// show download size
		packager->showDownloadSize();
	    else
	    {
	        // show the required diskspace
		packager->showDiskSpace();
		// show appropriate filter menu entries
		packager->createFilterMenu();
	    }
	}
    }
    else
    {
	NCERR << "Package table does not exist" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelectorStart::handleEvent
//      METHOD TYPE : bool
//
//      DESCRIPTION : passes the event to the handleEvent method
//		      of the member variable PackageSelector packager
//
bool NCPackageSelectorStart::handleEvent ( const NCursesEvent & event )
{
    if ( !packager )
	return false;
    
    return packager->handleEvent( event );
}


///////////////////////////////////////////////////////////////////
//
// readLayoutFile
// 
// Read a layout file (containing a YCPTerm)
//
YCPTerm NCPackageSelectorStart::readLayoutFile( const char * layoutFilename )
{
    YCPTerm   pkgLayout = YCPNull();
    
    FILE * layoutFile = fopen( layoutFilename, "r" );

    if ( ! layoutFile )
    {
	NCERR << "Can't open layout file " << layoutFilename << endl;
    }
    else
    {
	NCMIL <<  "Loading layout file " << layoutFilename << endl;
    
	Parser parser( layoutFile, layoutFilename );
	YCodePtr parsed_code = parser.parse ();
	YCPValue layout = YCPNull ();

	if ( parsed_code != NULL )
	    layout = parsed_code->evaluate ();

	if ( layout.isNull() )
	{
	    NCERR << "Error parsing layout file - layout reading aborted" << endl;
	}
	else
	{
	    if ( !layout->isTerm() )
	    {
		NCERR << "Error evaluating layout file - layout must be a term" << endl;	
	    }
	    else
	    {
		pkgLayout = layout->asTerm();
		NCDBG <<  "LAYOUT TERM: " << layout->toString() << endl; 
	    }
	}
	
	fclose( layoutFile );
    }
    
    return pkgLayout;
}

