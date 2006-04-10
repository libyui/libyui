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

   File:       NCPackageSelector.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPackageSelector.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"
#include "NCLabel.h"
#include "PkgNames.h"
#include "NCi18n.h"

#include <libintl.h>

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelector::NCPackageSelector
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPackageSelector::NCPackageSelector( YNCursesUI *ui,
				      NCWidget * parent,
				      const YWidgetOpt & opt, YUIDimension dimension,
				      string floppyDevice )
    : NCSplit( parent, opt, dimension )
      , widgetRoot( 0 )
      , packager( 0 )
      , youMode ( false )
      , updateMode ( false )
{
    // set the textdomain
    setTextdomain( "packages" );
    
    // get the mode (the mode is also available in PackageSelector via const YWidgetOpt & opt)
    if ( opt.youMode.value() )
	youMode = true;

    if ( opt.updateMode.value() )
	updateMode = true;
    
    // Read the layout file and create the widget tree
    YCPTerm pkgLayout = YCPNull();
    if ( youMode )
    {
	pkgLayout = readLayoutFile( ui, "/usr/share/YaST2/data/you_layout.ycp" );
    }
    else
    { 
	pkgLayout = readLayoutFile( ui, "/usr/share/YaST2/data/pkg_layout.ycp" );
    }

    if ( ! pkgLayout.isNull() )
    {
	YWidgetOpt childrenOpt( opt );
	widgetRoot = (YContainerWidget *)ui->createWidgetTree( dynamic_cast<YWidget *>(parent),
							       childrenOpt, 0, pkgLayout );
    }

    // create the PackageSelector (creation with 'new' is required because initialization
    // in the list of member variables causes problems with untranslated messages)
    packager = new PackageSelector( ui, opt, floppyDevice );

    if ( widgetRoot )
    {
	this->addChild( widgetRoot );
	widgetRoot->setParent( this );
	
	NCDBG <<  "Widget tree of NCPackageSelector created" << endl;

	// get the widget ID of the package table
	YWidget * pkg = ui->widgetWithId( widgetRoot, PkgNames::Packages(), true );
	pkgList = dynamic_cast<NCPkgTable *>(pkg);

	if ( pkgList )
	{
	    // set the type of the table
	    ObjectStatStrategy * strategy;
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
//	METHOD NAME : NCPackageSelector::~NCPackageSelector
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPackageSelector::~NCPackageSelector()
{
    if ( packager )
    {
	delete packager;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelector::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPackageSelector::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  NCSplit::setSize( newwidth, newheight );
}


///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelector::showDefaultList
//      METHOD TYPE : void
//
//      DESCRIPTION : fill up the package table with default data
//		     
//
void NCPackageSelector::showDefaultList()
{
    // fill the package table with packages belonging to the default filter
    if ( pkgList )
    {
	// fill the list with packages (or patches)
	pkgList->fillDefaultList( );

	pkgList->setKeyboardFocus();

	if ( !youMode && packager )
	{
	    // do an initial dependency solving in 'normal' and 'update' mode
	    packager->showPackageDependencies( true );
	    // show the required diskspace
	    packager->showDiskSpace();	    
	}
	if ( youMode && packager )
	{
	    packager->showDownloadSize();
	}
    }
    else
    {
	NCERR << "Package table does not exist" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//      METHOD NAME : NCPackageSelector::handleEvent
//      METHOD TYPE : bool
//
//      DESCRIPTION : passes the event to the handleEvent method
//		      of the member variable PackageSelector packager
//
bool NCPackageSelector::handleEvent ( const NCursesEvent & event )
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
YCPTerm NCPackageSelector::readLayoutFile( YNCursesUI *ui,
					   const char * layoutFilename )
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

