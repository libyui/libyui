/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

  File:		NCPackageSelectorPluginImpl.cc

  Author:	Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#include "NCPackageSelectorPluginImpl.h"
#include "NCPackageSelectorStart.h"

#include <YTableHeader.h>

#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

using std::endl;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorPluginStub::createPackageSelector
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Create NCPackageSelectorStart which reads the layout
//                    term of the package selection dialog, creates the widget
//		      tree and creates the NCPackageSelector.
//
NCPackageSelectorPluginImpl PSP;

YPackageSelector *
NCPackageSelectorPluginImpl::createPackageSelector( YWidget * parent,
                                                    long      modeFlags )
{
    YWidget * w = 0;

    try
    {
	w = new NCPackageSelectorStart ( parent, modeFlags, YD_HORIZ );
    }
    catch (const std::exception & e)
    {
	yuiError() << "Caught a std::exception: " << e.what() << endl;
    }
    catch (...)
    {
	yuiError() << "Caught an unspecified exception" << endl;
    }

    yuiMilestone() << "Package selector created: " << w << endl;

    return (YPackageSelector *)(w);
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
YWidget *
NCPackageSelectorPluginImpl::createPkgSpecial( YWidget *parent, const std::string &subwidget )
{
    YWidget * w = 0;
    YTableHeader * tableHeader = new YTableHeader();

    if ( subwidget == "pkgTable" )
    {
	yuiDebug() << "Creating a NCPkgTable" << endl;
	try
	{
	    // yuiError() << "Tady taky nic neni " << endl;
	    w = new NCPkgTable( parent, tableHeader );
	}
	catch (const std::exception & e)
	{
	    yuiError() << "Caught a std::exception: " << e.what() << endl;
	}
	catch (...)
	{
	    yuiError() << "Caught an unspecified exception" << endl;
	}
    }
    else
    {
	yuiError() <<  "PkgSpecial( "  << subwidget << " )  not found - take default `Label" << endl;
	w = new NCLabel( parent, subwidget, false, false );
    }

    return w;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPackageSelectorPLugin::runPkgSelection
//	METHOD TYPE : YEvent *
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YEvent *
NCPackageSelectorPluginImpl::runPkgSelection(  YDialog * dialog,
                                               YWidget * selector )
{
    NCPackageSelectorStart * ncSelector = 0;

    yuiMilestone() << "Calling runPkgSelection()" << endl;

    if ( !dialog )
    {
	yuiError() << "ERROR package selection: No dialog existing." << endl;
	return 0;
    }
    if ( !selector )
    {
	yuiError() << "ERROR package selection: No package selector existing." << endl;
	return 0;
    }

    ncSelector = dynamic_cast<NCPackageSelectorStart *>( selector );

    bool result = true;

    // start event loop
    NCursesEvent event = NCursesEvent::cancel;
    NCDialog * ncd = static_cast<NCDialog *>( dialog );

    if ( ncSelector && ncd )
    {
	try
	{
	    ncSelector->showDefaultList();
            ncd->setStatusLine();       // show function keys
	    yuiMilestone() << "NCDialog: " << ncd << endl;
	    do
	    {
		event = ncd->userInput();
		result = ncSelector->handleEvent( event );
                // reset to function keys of the main dialog
                ncd->setStatusLine();
		// yuiMilestone() << "Result of handleEvent: " << (result?"true":"false") << endl;
	    }
	    while ( event != NCursesEvent::cancel && result == true );
	}
	catch (const std::exception & e)
	{
	    yuiError() << "Caught a std::exception: " << e.what() << endl;
	}
	catch (...)
	{
	    yuiError() << "Caught an unspecified exception" << endl;
	}
    }
    else
    {
	yuiError() << "No NCPackageSelectorStart existing" << endl;
    }

    if ( event.result != "" )
    {
        // Before returning some value to the YCP client,
	// we must delete (==close) any leftover dialogs,
	// Wizard will not do it for us (#354712)
        while( YDialog::topmostDialog() != dialog )
            YDialog::deleteTopmostDialog();

	yuiMilestone() << "Return value: " << event.result << endl;
	return new YMenuEvent( event.result );
    }
    else
	return new YCancelEvent();
}

