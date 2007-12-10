/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      NCApplication.cc

  Author:     Gabriele Mohr <gs@suse.de>

/-*/


#include "Y2Log.h"
#include "NCurses.h"
#include "NCApplication.h"
#include "NCAskForDirectory.h"
#include "NCAskForFile.h"


NCApplication::NCApplication()
{
    
}


NCApplication::~NCApplication()
{
    
}


void
NCApplication::setLanguage( const string & language,
			    const string & encoding )
{
    YApplication::setLanguage( language, encoding );
    NCurses::Refresh();
    
    NCDBG << "Language: " << language << " Encoding: " << ((encoding!="")?encoding:"NOT SET") << endl;
  
}


string
NCApplication::askForSaveFileName( const string & startDir,
				   const string & filter,
				   const string & headline )
{
    NCAskForSaveFileName * filePopup = new NCAskForSaveFileName( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );
    
    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    NCMIL << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}


string
NCApplication::askForExistingFile( const string & startDir,
				   const string & filter,
				   const string & headline )
{
    NCAskForExistingFile * filePopup = new NCAskForExistingFile( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );
    
    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    NCMIL << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}


string
NCApplication::askForExistingDirectory( const string & startDir,
					const string & headline )
{
    NCAskForExistingDirectory * dirPopup = new NCAskForExistingDirectory( wpos( 1, 1 ), startDir, headline );
    YUI_CHECK_NEW( dirPopup );
    
    NCursesEvent retEvent = dirPopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    NCMIL << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}

