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

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPParser.h>
#include <ycp/YCPBlock.h>


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelector::NCPackageSelector
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPackageSelector::NCPackageSelector( Y2NCursesUI *ui, NCWidget * parent,
				      YWidgetOpt & opt, YUIDimension dimension )
    : NCSplit( parent, opt, dimension )
      , widgetRoot( 0 )
      , packager( ui, opt )
      , youMode ( false )
      , updateMode ( false )
{
    // get the mode (the mode is also available in PackageSelector via YWidgetOpt & opt)
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
	widgetRoot = (YContainerWidget *)ui->createWidgetTree( dynamic_cast<YWidget *>(parent),
							       opt, 0, pkgLayout );
    }

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
	    pkgList->setPackager( &packager );

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
    WIDDBG << endl;
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
    return packager.handleEvent( event );
}


///////////////////////////////////////////////////////////////////
//
// readLayoutFile
// 
// Read a layout file (containing a YCPTerm)
//
YCPTerm NCPackageSelector::readLayoutFile( Y2NCursesUI *ui, const char * layoutFilename )
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
    
	YCPParser parser( layoutFile, layoutFilename );
	YCPValue layout = parser.parse();

	if ( layout.isNull() )
	{
	    NCERR << "Error parsing layout file - layout reading aborted" << endl;
	}

        layout = ui->evaluate (layout);

	if ( layout.isNull()
	     || !layout->isTerm() )
	{
	    NCERR << "Error parsing layout file - layout must be a term" << endl;	
	}
	else
	{
	    pkgLayout = layout->asTerm();
	}

	NCDBG <<  "LAYOUT TERM: " << layout->toString() << endl;

	fclose( layoutFile );
    }

    return pkgLayout;
}
