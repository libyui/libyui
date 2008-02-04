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

   File:       YNCursesUI.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include "YNCursesUI.h"
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <langinfo.h>

#include "Y2Log.h"
#include <YEvent.h>
#include "YDialog.h"

#include <ycp/y2log.h>
#include <ycp/Parser.h>
#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPMap.h>
#include <yui/YUI.h>

#include "NCPackageSelectorStart.h"
#include "NCstring.h"
#include "NCWidgetFactory.h"
#include "NCOptionalWidgetFactory.h"
#include "NCPackageSelectorPlugin.h"

extern string language2encoding( string lang );

YNCursesUI::YNCursesUI( int argc, char **argv, bool with_threads, const char * macro_file )
    : YUI( with_threads )
{
    y2milestone ("Start YNCursesUI");
    _ui = this;

    if ( getenv( "LANG" ) != NULL )
    {
	string language = getenv( "LANG" );
	string encoding =  nl_langinfo( CODESET );

        // setlocale ( LC_ALL, "" ) is called in WMFInterpreter::WFMInterpreter;

	// Explicitly set LC_CTYPE so that it won't be changed if setenv( LANG ) is called elsewhere.
        // (it's not enough to call setlocale( LC_CTYPE, .. ), set env. variable LC_CTYPE!)

	string locale = setlocale( LC_CTYPE, NULL );
	setenv( "LC_CTYPE", locale.c_str(), 1 );

	NCMIL << "setenv LC_CTYPE: " << locale << " encoding: " << encoding << endl;
	
        // The encoding of a terminal (xterm, konsole usw. ) can never change; the encoding
	// of the "real" console is changed in setConsoleFont(). 
	NCstring::setTerminalEncoding( encoding );
  
	app()->setLanguage( language, encoding );
    }

    try {
	NCurses::init();
    }
    catch ( NCursesError & err ) {
	UIMIL << err << endl;
	::endwin();
	abort();
    }

    if ( macro_file )
	playMacro( macro_file );
    
    topmostConstructorHasFinished();
}

YNCursesUI::~YNCursesUI()
{
    //delete left-over dialogs (if any)
    YDialog::deleteAllDialogs();
    y2milestone ("Stop YNCursesUI");
}


YNCursesUI * YNCursesUI::_ui = 0;

YWidgetFactory *
YNCursesUI::createWidgetFactory()
{
    NCWidgetFactory * factory = new NCWidgetFactory();
    YUI_CHECK_NEW( factory );
    
    return factory;
}



YOptionalWidgetFactory *
YNCursesUI::createOptionalWidgetFactory()
{
    NCOptionalWidgetFactory * factory = new NCOptionalWidgetFactory();
    YUI_CHECK_NEW( factory );
    
    return factory;
}


YApplication *
YNCursesUI::createApplication()
{
    NCApplication * app = new NCApplication();
    YUI_CHECK_NEW( app );

    return app;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::idleLoop
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void YNCursesUI::idleLoop( int fd_ycp )
{

  int    timeout = 5;
  struct timeval tv;
  fd_set fdset;
  int    retval;

  do {
    tv.tv_sec  = timeout;
    tv.tv_usec = 0;

    FD_ZERO( &fdset );
    FD_SET( 0,      &fdset );
    FD_SET( fd_ycp, &fdset );

    retval = select( fd_ycp+1, &fdset, 0, 0, &tv );

    if ( retval < 0 ) {
      if ( errno != EINTR )
	UIINT << "idleLoop error in select() (" << errno << ')' << endl;
    } else if ( retval != 0 ) {
      //do not throw here, as current dialog may not necessarily exist yet
      //if we have threads
      YDialog *currentDialog = YDialog::currentDialog( false );

      if (currentDialog) {
        NCDialog * ncd = static_cast<NCDialog *>( currentDialog );
        if( ncd ) {
	  extern NCBusyIndicator* NCBusyIndicatorObject;
	  if (NCBusyIndicatorObject)
  	    NCBusyIndicatorObject->handler(0);

	  ncd->idleInput();
        }
      }
    } // else no input within timeout sec.
  } while ( !FD_ISSET( fd_ycp, &fdset ) );
}

///////////////////////////////////////////////////////////////////


#define ONCREATE WIDDBG << endl
//#define ONCREATE


///////////////////////////////////////////////////////////////////
//
// package selection
//
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::packageSelectorPlugin()
//	METHOD TYPE : NCPackageSelectorPlugin
//
//	DESCRIPTION : Create the package selector plugin
//
NCPackageSelectorPlugin * YNCursesUI::packageSelectorPlugin()
{
    static NCPackageSelectorPlugin * plugin = 0;

    if ( ! plugin )
    {
	plugin = new NCPackageSelectorPlugin();

	// This is a deliberate memory leak: If an application requires a
	// PackageSelector, it is a package selection application by
	// definition. In this case, the ncurses_pkg plugin is intentionally
	// kept open to avoid repeated start-up cost of the plugin and libzypp.
    }

    return plugin;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::runPkgSelection
//	METHOD TYPE : YEvent *
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YEvent * YNCursesUI::runPkgSelection( YWidget * selector )
{
    YEvent * event = 0;
    
    YDialog *dialog = YDialog::currentDialog();
    NCPackageSelectorPlugin * plugin = packageSelectorPlugin();
    
    if ( !dialog )
    {
	UIERR << "ERROR package selection: No dialog rexisting." << endl;
	return 0;
    }
    if ( !selector )
    {
	UIERR << "ERROR package selection: No package selector existing." << endl;
	return 0;
    }
    // FIXME - remove debug logging
    NCMIL << "Dump current dialog in YNCursesUI::runPkgSelection" << endl;
    // debug: dump the widget tree
    dialog->dumpDialogWidgetTree();	

    if ( plugin )
    {
	event = plugin->runPkgSelection( dialog, selector );
    }

    return event;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::setKeyboard
//	METHOD TYPE : YCPValue
//
//
YCPValue YNCursesUI::setKeyboard()
{
    string cmd = "/bin/dumpkeys | /bin/loadkeys --unicode";
    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	int ret = system( (cmd + " >/dev/null 2>&1").c_str() );
	if ( ret != 0 )
	{
	    NCERR << "ERROR: /bin/dumpkeys | /bin/loadkeys --unicode returned: "<< ret << endl;
	}
    }

    return YCPVoid();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::setConsoleFont
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION : UI::setConsoleFont() is called in Console.ycp.
//		      The terminal encoding must be set correctly.
//
YCPValue YNCursesUI::setConsoleFont( const YCPString & console_magic,
				      const YCPString & font,
				      const YCPString & screen_map,
				      const YCPString & unicode_map,
				      const YCPString & lang )
{
  string cmd( "setfont" );
  cmd += " -C " + myTerm;
  cmd += " " + font->value();
  if ( !screen_map->value().empty() )
    cmd += " -m " + screen_map->value();
  if ( !unicode_map->value().empty() )
    cmd += " -u " + unicode_map->value();

  UIMIL << cmd << endl;
  int ret = system( (cmd + " >/dev/null 2>&1").c_str() );

  // setfont returns error if called e.g. on a xterm -> return YCPVoid()
  if ( ret ) {
    UIERR << cmd.c_str() << " returned " << ret << endl;
    Refresh();
    return YCPVoid();
  }
  // go on in case of a "real" console
  cmd = "(echo -en \"\\033";
  if ( console_magic->value().length() )
    cmd += console_magic->value();
  else
    cmd += "(B";
  cmd += "\" >" + myTerm + ")";
  UIMIL << cmd << endl;
  ret = system( (cmd + " >/dev/null 2>&1").c_str() );
  if ( ret ) {
    UIERR << cmd.c_str() << " returned " << ret << endl;
  }

  // set terminal encoding for console
  // (setConsoleFont() in Console.ycp has passed the encoding as last argument
  // but this encoding was not correct; now Console.ycp passes the language)

  // if the encoding is NOT UTF-8 set the console encoding according to the language
  if ( NCstring::terminalEncoding() != "UTF-8" )
  {
      string language = lang->value();
      string::size_type pos = language.find( '.' );

      if ( pos != string::npos )
      {
	  language.erase( pos );
      }
      pos = language.find( '_' );
      if ( pos != string::npos )
      {
	  language.erase( pos );
      }

      string code = language2encoding( language );

      NCMIL << "setConsoleFont( ENCODING:  " << code << " )" << endl;
    
      if ( NCstring::setTerminalEncoding( code ) )
      {
	  Redraw();
      }
      else
      {    
	  Refresh();
      }
  }
  else
  {
      Refresh();
  }

  return YCPVoid();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::init_title
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void YNCursesUI::init_title()
{
    string title_ti( "YaST" );
    char hostname_ti[256];
    hostname_ti[0] = hostname_ti[255] = '\0';

    // check for valid hostname, suppress "(none)" as hostname
    if ( gethostname( hostname_ti, 255 ) != -1
	 && hostname_ti[0]
	 && hostname_ti[0] != '(' )
    {
	title_ti += " @ ";
	title_ti += hostname_ti;
    }
    NCurses::SetTitle( title_ti );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::want_colors()
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool YNCursesUI::want_colors()
{
  if ( getenv( "Y2NCURSES_BW" ) != NULL ) {
    UIMIL << "Y2NCURSES_BW is set - won't use colors" << endl;
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::beep()
//	METHOD TYPE : void
//
//	DESCRIPTION : beeps the system bell
//
void YNCursesUI::beep()
{
  ::beep();
}


