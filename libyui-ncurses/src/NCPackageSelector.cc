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
      , packager( ui )
{
    // FIXME: get mode ONLINE/UPDATE/INSTALL (and pass it to the packager)
    
    // Read the layout file and create the widget tree
    YCPTerm   pkgLayout = readLayoutFile( "/usr/share/YaST2/data/pkg_layout.ycp" );

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
	    // fill table header
	    packager.fillHeader( pkgList );
	    // set the pointer to the packager object
	    pkgList->setPackager( &packager );
	}
	
        // set the label for the default filter
	YWidget * label = ui->widgetWithId( widgetRoot, PkgNames::Filter(), true );
	if ( label )
	{
	    static_cast<NCLabel *>(label)->setLabel( YCPString("default") );
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
	// FIRST, set the package list widget
	packager.setPackageList( pkgList );

	// second, fill the list with packages 
	packager.fillPackageList( YCPString("default"), 0 );
	
        // set the visible info to package description 
	packager.setVisibleInfo ( PkgNames::PkgInfo() );
	
	// show the package description of the current item
	packager.showPackageInformation( pkgList->getDataPointer( pkgList->getCurrentItem() ) );

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
YCPTerm NCPackageSelector::readLayoutFile( const char * layoutFilename )
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
	else if ( ! layout->isTerm() )
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
