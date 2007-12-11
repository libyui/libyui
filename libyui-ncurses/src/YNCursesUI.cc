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

#include "Y2Log.h"
#include <YEvent.h>

#include <ycp/y2log.h>
#include <ycp/Parser.h>
#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPMap.h>
#include <yui/YUI.h>

#include "NCDialog.h"
#include "NCAlignment.h"
#include "NCCheckBox.h"
#include "NCEmpty.h"
#include "NCSpacing.h"
#include "NCFrame.h"
#include "NCCheckBoxFrame.h"
#include "NCImage.h"
#include "NCLabel.h"
#include "NCProgressBar.h"
#include "NCPushButton.h"
#include "NCMenuButton.h"
#include "NCRadioButton.h"
#include "NCRadioButtonGroup.h"
#include "NCReplacePoint.h"
#include "NCRichText.h"
#include "NCSelectionBox.h"
#include "NCMultiSelectionBox.h"
#include "NCSplit.h"
#include "NCSquash.h"
#include "NCTable.h"
#include "NCTextEntry.h"
#include "NCIntField.h"
#include "NCComboBox.h"
#include "NCTree.h"
#include "NCLogView.h"
#include "NCMultiLineEdit.h"
#include "NCPackageSelector.h"
#include "NCPkgTable.h"
#include "NCAskForDirectory.h"
#include "NCAskForFile.h"
#include "NCstring.h"

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
	// of "real" console is changed in setConsoleFont(). 
	NCstring::setTerminalEncoding( encoding );
  
	setLanguage( language );
    }

    try {
	NCurses::init();
    }
    catch ( NCursesError & err ) {
	UIINT << err << endl;
	::endwin();
	abort();
    }

    if ( macro_file )
	playMacro( macro_file );
    
    topmostConstructorHasFinished();
}

YNCursesUI::~YNCursesUI()
{
    y2milestone ("Stop YNCursesUI");
}


YNCursesUI * YNCursesUI::_ui = 0;

/******************************************************************
**
**
**	FUNCTION NAME : Recode
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int Recode( const string & str, const string & from,
	    const string & to, string & outstr )
{
  return YUI::Recode( str, from, to, outstr );
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
  NCDialog * ncd = static_cast<NCDialog *>( currentDialog() );

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
      if( ncd ) {
	ncd->idleInput();
      }
    } // else no input within timeout sec.
  } while ( !FD_ISSET( fd_ycp, &fdset ) );
}

///////////////////////////////////////////////////////////////////

struct NCtoY2Event : public NCursesEvent {

  friend ostream & operator<<( std::ostream & STREAM, const NCtoY2Event & OBJ ) {
    STREAM << static_cast<const NCursesEvent &>(OBJ);
    if ( OBJ.selection.isNull() ) {
      STREAM << "(-)";
    } else {
      STREAM << "(" << OBJ.selection->valuetype() << ")";
    }
    return STREAM << " for " << OBJ.widget;
  }

  NCtoY2Event() {}
  NCtoY2Event( const NCursesEvent & ncev ) : NCursesEvent( ncev ) {}

  NCtoY2Event & operator=( const NCursesEvent & ncev ) {
    if ( ncev.isInternalEvent() )
      NCursesEvent::operator=( none );
    else
      NCursesEvent::operator=( ncev );
    return *this;
  }

  YEvent * propagate()
  {
      switch ( type )
      {
	  // Note: libyui assumes ownership of YEvents, so they need to be
	  // created on the heap with 'new'. libyui takes care of deleting them.
	  
	  case button:
	      if ( widget->isValid() )
		  return new YWidgetEvent( dynamic_cast<YWidget *> (widget), reason );
	      else
		  return 0;
	      
	  case menu:
	      return new YMenuEvent( selection );
	    
	  case cancel:
	      return new YCancelEvent();
	      
	  case timeout:
	      return new YTimeoutEvent();

	  case key:
	      if ( widget->isValid() )
		  return new YKeyEvent( keySymbol, dynamic_cast<YWidget *> (widget) );
	      else
		  return 0;
	      
	  case none:
	  case handled:
	      return 0;

	  // Intentionally omitting 'default' branch so the compiler can
	  // detect unhandled enums 
      }

      // If we get this far, there must be an error.
    
      UIINT << "Can't propagate through (EventType*)0" << endl;
      UIDBG << *this << endl;
      return 0;
  }
};


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::userInput
//	METHOD TYPE : YEvent *
//
//	DESCRIPTION :
//
YEvent * YNCursesUI::userInput( unsigned long timeout_millisec )
{
  NCDialog * ncd = static_cast<NCDialog *>( currentDialog() );
  if ( !ncd ) {
    UIERR << "No current NCDialog " << endl;
    return 0;
  }

  NCtoY2Event cevent;
  ncd->activate ( true );
  cevent = ncd->userInput( timeout_millisec ? (long)timeout_millisec : -1 );
  ncd->activate ( false );

  YEvent * yevent = cevent.propagate();
  UIDBG << "Returning event: " << ( yevent ? yevent->ycpEvent()->toString().c_str() : "(nil)" ) << endl;
  
  return yevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::pollInput
//	METHOD TYPE : YWidget *
//
//	DESCRIPTION :
//
YEvent * YNCursesUI::pollInput()
{
  NCDialog * ncd = static_cast<NCDialog *>( currentDialog() );
  if ( !ncd ) {
    UIERR << "No current NCDialog " << endl;
    return 0;
  }

  // no activation here, done in pollInput, if..
  NCtoY2Event cevent = ncd->pollInput();

  YEvent * yevent = cevent.propagate();

  if ( yevent )
      UIDBG << "Returning event: " << yevent->ycpEvent()->toString() << endl;

  return yevent;
}

///////////////////////////////////////////////////////////////////
//
// Dialog and widgets
//
///////////////////////////////////////////////////////////////////

YDialog * YNCursesUI::createDialog( YWidgetOpt & opt )
{
  NCDialog * dialog = new NCDialog( opt );
  UIDBG << dialog << endl;
  return dialog;
}

void YNCursesUI::showDialog( YDialog * dialog ) {
  NCDialog * dlg = dynamic_cast<NCDialog *>( dialog );
  UIDBG << dlg << endl;
  if ( dlg )
    dlg->showDialog();
}

void YNCursesUI::closeDialog( YDialog * dialog ) {
  NCDialog * dlg = dynamic_cast<NCDialog *>( dialog );
  UIDBG << dlg << endl;
  if ( dlg )
    dlg->closeDialog();
}

///////////////////////////////////////////////////////////////////
//
// Widget creation functions - container widgets
//
///////////////////////////////////////////////////////////////////

//#define ONCREATE WIDDBG << endl
#define ONCREATE

YContainerWidget * YNCursesUI::createSplit( YWidget * parent, YWidgetOpt & opt,
					     YUIDimension dimension )
{
  ONCREATE;
  return new NCSplit( dynamic_cast<NCWidget *>( parent ), opt,
		      dimension );
}

YContainerWidget * YNCursesUI::createReplacePoint( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCReplacePoint( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * YNCursesUI::createAlignment( YWidget * parent, YWidgetOpt & opt,
						 YAlignmentType halign,
						 YAlignmentType valign )
{
  ONCREATE;
  return new NCAlignment( dynamic_cast<NCWidget *>( parent ), opt,
			  halign, valign );
}

YContainerWidget * YNCursesUI::createSquash( YWidget * parent, YWidgetOpt & opt,
					      bool hsquash,
					      bool vsquash )
{
  ONCREATE;
  return new NCSquash( dynamic_cast<NCWidget *>( parent ), opt,
		       hsquash, vsquash );
}

YContainerWidget * YNCursesUI::createRadioButtonGroup( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCRadioButtonGroup( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * YNCursesUI::createFrame( YWidget * parent, YWidgetOpt & opt,
					     const YCPString & label )
{
  ONCREATE;
  return new NCFrame( dynamic_cast<NCWidget *>( parent ), opt,
		      label );
}

YContainerWidget * YNCursesUI::createCheckBoxFrame( YWidget * parent, YWidgetOpt & opt,
						    const YCPString & label, bool checked )
{
  ONCREATE;
  return new NCCheckBoxFrame( dynamic_cast<NCWidget *>( parent ), opt,
			      label, checked );
}

///////////////////////////////////////////////////////////////////
//
// Widget creation functions - leaf widgets
//
///////////////////////////////////////////////////////////////////

YWidget * YNCursesUI::createEmpty( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCEmpty( dynamic_cast<NCWidget *>( parent ), opt );
}


YWidget * YNCursesUI::createSpacing( YWidget * parent, YWidgetOpt & opt,
				      float size,
				      bool horizontal, bool vertical )
{
  ONCREATE;
  return new NCSpacing( dynamic_cast<NCWidget *>( parent ), opt,
			size, horizontal, vertical );
}

YWidget * YNCursesUI::createLabel( YWidget * parent, YWidgetOpt & opt,
				    const YCPString & text )
{
  ONCREATE;
  return new NCLabel( dynamic_cast<NCWidget *>( parent ), opt,
		      text );
}

YWidget * YNCursesUI::createRichText( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & text )
{
  ONCREATE;
  return new NCRichText( dynamic_cast<NCWidget *>( parent ), opt,
			 text );
}

YWidget * YNCursesUI::createLogView( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label,
				      int visibleLines,
				      int maxLines )
{
  ONCREATE;
  return new NCLogView( dynamic_cast<NCWidget *>( parent ), opt,
			label, visibleLines, maxLines );
}

YWidget * YNCursesUI::createPushButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCPushButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * YNCursesUI::createMenuButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCMenuButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * YNCursesUI::createRadioButton( YWidget * parent, YWidgetOpt & opt,
					  YRadioButtonGroup * rbg,
					  const YCPString & label,
					  bool checked )
{
  ONCREATE;
  return new NCRadioButton( dynamic_cast<NCWidget *>( parent ), opt,
			    rbg, label, checked );
}

YWidget * YNCursesUI::createCheckBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label,
				       bool checked )
{
  ONCREATE;
  return new NCCheckBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label, checked );
}

YWidget * YNCursesUI::createTextEntry( YWidget * parent, YWidgetOpt & opt,
					const YCPString & label,
					const YCPString & text )
{
  ONCREATE;
  return new NCTextEntry( dynamic_cast<NCWidget *>( parent ), opt,
			  label, text );
}

YWidget *YNCursesUI::createMultiLineEdit( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label,
					   const YCPString & initialText )
{
  ONCREATE;
  return new NCMultiLineEdit( dynamic_cast<NCWidget *>( parent ), opt,
			      label, initialText );
}

YWidget * YNCursesUI::createSelectionBox( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label )
{
  ONCREATE;
  return new NCSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
			     label );
}

YWidget * YNCursesUI::createMultiSelectionBox( YWidget *parent, YWidgetOpt & opt,
						const YCPString & label )
{
  ONCREATE;
  return new NCMultiSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
				  label );
}

YWidget * YNCursesUI::createComboBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label )
{
  ONCREATE;
  return new NCComboBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label );
}

YWidget * YNCursesUI::createTree( YWidget * parent, YWidgetOpt & opt,
				   const YCPString & label )
{
  ONCREATE;
  return new NCTree( dynamic_cast<NCWidget *>( parent ), opt,
		     label );
}

YWidget * YNCursesUI::createTable( YWidget * parent, YWidgetOpt & opt,
				    vector<string> header )
{
  ONCREATE;
  return new NCTable( dynamic_cast<NCWidget *>( parent ), opt,
		      header, true );
}

YWidget * YNCursesUI::createProgressBar( YWidget * parent, YWidgetOpt & opt,
					  const YCPString & label,
					  const YCPInteger & maxprogress,
					  const YCPInteger & progress )
{
  ONCREATE;
  return new NCProgressBar( dynamic_cast<NCWidget *>( parent ), opt,
			    label, maxprogress, progress );
}

YWidget * YNCursesUI::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPByteblock /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * YNCursesUI::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPString /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}


YWidget * YNCursesUI::createIntField( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label,
				       int minValue, int maxValue,
				       int initialValue )
{
  ONCREATE;
  return new NCIntField( dynamic_cast<NCWidget *>( parent ), opt,
			 label, minValue, maxValue, initialValue );
}

///////////////////////////////////////////////////////////////////
//
// package selection
//
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::createPackageSelector
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Reads the layout term of the package selection dialog
// 		      and creates the widget tree.
//
YWidget * YNCursesUI::createPackageSelector( YWidget * parent,
					      YWidgetOpt & opt,
					      const YCPString & floppyDevice )
{
    ONCREATE;
    YWidget * w = 0;
    try
    {
	w = new NCPackageSelector (this, dynamic_cast<NCWidget *>(parent), opt,
				   YD_HORIZ, floppyDevice->value() );
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
//	METHOD NAME : YNCursesUI::runPkgSelection
//	METHOD TYPE : void
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YCPValue YNCursesUI::runPkgSelection(  YWidget * selector )
{
    NCPackageSelector * ncSelector = 0;

    YDialog *dialog = currentDialog();

    if (!dialog)
    {
	UIERR << "ERROR package selection: No dialog existing." << endl;
    }
    else
    {
	ncSelector = dynamic_cast<NCPackageSelector *>( selector );

	// debug: dump the widget tree
	dialog->dumpDialogWidgetTree();
    }

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
	UIERR << "No NCPackageSelector existing" << endl;
    }

    if ( !event.result.isNull() )
    {
	UIMIL << "Return value: " << event.result->toString() << endl;
	return event.result;
    }
    else
	return YCPVoid();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::createPkgSpecial
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : creates special widgets used for the package selection
//		      dialog (which do not have a corresponding widget in qt-ui)
//
YWidget * YNCursesUI::createPkgSpecial( YWidget *parent, YWidgetOpt &opt, const YCPString &subwidget )
{
    ONCREATE;

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
//	METHOD NAME : YNCursesUI::setLanguage
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue YNCursesUI::setLanguage( const YCPTerm & term )
{
   if ( term->size() == 1 && term->value(0)->isString() ) {
     setLanguage( term->value(0)->asString()->value() );
     return YCPVoid();
   }
   return YCPNull();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::setLanguage
//	METHOD TYPE : bool
//
//	DESCRIPTION : Expect lang to be 'language[_country][.encoding]'
//
//		      From version 9.0 on (UTF-8 support) setLanguage()
//		      don't have to look for the correct encoding.
//
bool YNCursesUI::setLanguage( string lang )
{
  string encoding;
  string language = lang;

  string::size_type pos = language.find( '.' );
  if ( pos != string::npos )
  {
    encoding = language.substr( pos+1 );
    language.erase( pos );
  }

  Refresh();
  NCDBG << "Language: " << language << " Encoding: " << ((encoding!="")?encoding:"NOT SET") << endl;
  
  return true;
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
//	METHOD NAME : YNCursesUI::runInTerminal
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION:: Run external program supplied as string parameter
// 		      in the same terminal
//

int YNCursesUI::runInTerminal( const YCPString & module )
{ 
    int ret;
    string cmd = module->value();  

    //Save tty modes and end ncurses mode temporarily
    ::def_prog_mode();
    ::endwin();

    //Regenerate saved stdout and stderr, so that app called
    //via system() can use them and draw something to the terminal
    dup2(NCurses::stdout_save, 1);
    dup2(NCurses::stderr_save, 2);

    //Call external program
    ret = system(cmd.c_str());

    if ( ret != 0 )
    {
	NCERR << cmd << " returned:" << ret << endl;
    }

    //Redirect stdout and stderr to y2log again
    NCurses::RedirectToLog();

    //Resume tty modes and refresh the screen
    ::reset_prog_mode();
    ::refresh();

    return ret;
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
//	METHOD NAME : YNCursesUI::hasFullUtf8Support
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool YNCursesUI::hasFullUtf8Support()
{
    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	return true;
    }
    else
    {
	return false;
    }
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::askForSaveFileName()
//	METHOD TYPE : YCPValue()
//
//	DESCRIPTION :
//
YCPValue YNCursesUI::askForSaveFileName( const YCPString & startDir,
					  const YCPString & filter,
					  const YCPString & headline )
{
    NCAskForSaveFileName filePopup( wpos( 1, 1 ), startDir, filter, headline );
    
    NCursesEvent retEvent = filePopup.showDirPopup( );

    if ( !retEvent.result.isNull() )
    {
	NCMIL << "Returning: " <<  retEvent.result->toString() << endl;
	return retEvent.result;
    }
    else
	return YCPVoid(); // nothing selected -> return 'nil'  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::askForExistingFile()
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue YNCursesUI::askForExistingFile( const YCPString & startDir,
					  const YCPString & filter,
					  const YCPString & headline )
{
    NCAskForExistingFile filePopup( wpos( 1, 1 ), startDir, filter, headline );
    
    NCursesEvent retEvent = filePopup.showDirPopup( );

    if ( !retEvent.result.isNull() )
    {
	NCMIL << "Returning: " <<  retEvent.result->toString() << endl;
	return retEvent.result;
    }
    else
	return YCPVoid(); // nothing selected -> return 'nil'  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YNCursesUI::askForExistingDirectory()
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue YNCursesUI::askForExistingDirectory( const YCPString & startDir,
					       const YCPString & headline )
{
    NCAskForExistingDirectory dirPopup( wpos( 1, 1 ), startDir, headline );
    
    NCursesEvent retEvent = dirPopup.showDirPopup( );

    if ( !retEvent.result.isNull() )
    {
	NCMIL << "Returning: " <<  retEvent.result->toString() << endl;
	return retEvent.result;
    }
    else
	return YCPVoid(); // nothing selected -> return 'nil'
}


