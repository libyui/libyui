/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		NCPackageSelectorPlugin.cc

  Author:	Gabriele Mohr <gs@suse.de>


/-*/

#include "NCPackageSelectorPlugin.h"

#include <ycp/y2log.h>
#include "NCPackageSelectorStart.h"
#include "NCPkgTable.h"

#define y2log_component "ncurses-ui"

#define PLUGIN_BASE_NAME "ncurses_pkg"


NCPackageSelectorPlugin::NCPackageSelectorPlugin()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	y2milestone( "Loaded %s plugin successfully from %s",
		     PLUGIN_BASE_NAME, pluginLibFullPath().c_str() );
    }
}


NCPackageSelectorPlugin::~NCPackageSelectorPlugin()
{
    // NOP
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorPlugin::createPackageSelector
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Create NCPackageSelectorStart which reads the layout
//                    term of the package selection dialog, creates the widget
//		      tree and creates the NCPackageSelector.
//
YWidget * NCPackageSelectorPlugin::createPackageSelector( YWidget * parent,
							  YWidgetOpt & opt )
{
    YWidget * w = 0;
    try
    {
	w = new NCPackageSelectorStart ( dynamic_cast<NCWidget *>(parent), opt,
					 YD_HORIZ );
    }
    catch (const std::exception & e)
    {
	UIERR << "Caught a std::exception: " << e.what () << endl;
    }
    catch (...)
    {
	UIERR << "Caught an unspecified exception" << endl;
    }
    return w;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorPluign::createPkgSpecial
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : creates special widgets used for the package selection
//		      dialog (which do not have a corresponding widget in qt-ui)
//
YWidget * NCPackageSelectorPlugin::createPkgSpecial( YWidget *parent,
						     YWidgetOpt &opt,
						     const YCPString &subwidget )
{
    YCPString pkgTable( "pkgTable" );

    YWidget * w = 0;
    if ( subwidget->compare( pkgTable ) == YO_EQUAL )
    {
	NCDBG << "Creating a NCPkgTable" << endl;
	try
	{
	    w = new NCPkgTable( dynamic_cast<NCWidget *>( parent ), opt );
	}
	catch (const std::exception & e)
	{
	    UIERR << "Caught a std::exception: " << e.what () << endl;
	}
	catch (...)
	{
	    UIERR << "Caught an unspecified exception" << endl;
	}
    }
    else
    {
	NCERR <<  "PkgSpecial( "  << subwidget->toString() << " )  not found - take default `Label" << endl;
	w = new NCLabel( dynamic_cast<NCWidget *>( parent ), opt, subwidget );
    }

    return w;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorPLugin::runPkgSelection
//	METHOD TYPE : void
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YCPValue NCPackageSelectorPlugin::runPkgSelection(  YDialog * dialog,
						    YWidget * selector )
{
    NCPackageSelectorStart * ncSelector = 0;

    if ( !dialog )
    {
	UIERR << "ERROR package selection: No dialog existing." << endl;
	return YCPVoid();
    }
    if ( !selector )
    {
	UIERR << "ERROR package selection: No package selector existing." << endl;
	return YCPVoid();
    }
    
    ncSelector = dynamic_cast<NCPackageSelectorStart *>( selector );

    bool result = true;

    // start event loop
    NCursesEvent event = NCursesEvent::cancel;

    if ( ncSelector )
    {
	try
	{
	    ncSelector->showDefaultList();
	    NCDialog * ncd = static_cast<NCDialog *>( dialog );

	    do
	    {
		event = ncd->userInput();
		result = ncSelector->handleEvent( event );
		NCDBG << "Result: " << (result?"true":"false") << endl;
	    }
	    while ( event != NCursesEvent::cancel && result == true );
	}
	catch (const std::exception & e)
	{
	    UIERR << "Caught a std::exception: " << e.what () << endl;
	}
	catch (...)
	{
	    UIERR << "Caught an unspecified exception" << endl;
	}
    }
    else
    {
	UIERR << "No NCPackageSelectorStart existing" << endl;
    }

    if ( !event.result.isNull() )
    {
	UIMIL << "Return value: " << event.result->toString() << endl;
	return event.result;
    }
    else
	return YCPVoid();
}

