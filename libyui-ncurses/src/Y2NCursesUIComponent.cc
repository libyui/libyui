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

   File:       Y2NCursesUIComponent.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include "Y2NCursesUIComponent.h"
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
#include <yui/Y2UIComponent.h>

#include "NCDialog.h"
#include "NCAlignment.h"
#include "NCCheckBox.h"
#include "NCEmpty.h"
#include "NCSpacing.h"
#include "NCFrame.h"
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

Y2NCursesUIComponent::Y2NCursesUIComponent(int argc, char **argv, bool with_threads, Y2Component *callback)
    : Y2UIComponent (with_threads, callback)
    , argc(argc)
    , argv(argv)
    , m_callback(0)
    , with_threads(true)
{
    y2milestone ("Start Y2NCursesUIComponent");
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
    
    topmostConstructorHasFinished();

// FIXME NI don't know what to do with it
//    if ( macro_file )
//	playMacro( macro_file );
}

Y2NCursesUIComponent::~Y2NCursesUIComponent()
{
    y2milestone ("Stop Y2NCursesUIComponent");
}

string Y2NCursesUIComponent::name() const
{
    return "ncurses";
}

void Y2NCursesUIComponent::result(const YCPValue &result)
{
/*    if (interpreter)
    {
	y2milestone ("Shutdown Y2NCursesUI...");
	delete interpreter;
	y2milestone ("Y2NCursesUI down");
    }
    interpreter = 0;*/
}

void Y2NCursesUIComponent::setServerOptions(int argc, char **argv)
{
    this->argc = argc;
    this->argv = argv;
}

#if 0

Y2Component *
Y2NCursesUIComponent::getCallback (void) const
{
    Y2Component *callback;
    if (interpreter)
    {
	callback = interpreter->getCallback ();
    }
    else
    {
	callback = m_callback;
    }
    y2debug ("Y2NCursesUIComponent[%p]::getCallback[i %p]() = %p", this, interpreter, callback);
    return callback;
}


void
Y2NCursesUIComponent::setCallback (Y2Component *callback)
{
    y2debug ("Y2NCursesUIComponent[%p]::setCallback[i %p](%p)", this, interpreter, callback);
    if (interpreter)
    {
	// interpreter allready running, pass callback directly
	// to where it belongs
	return interpreter->setCallback (callback);
    }
    else
    {
	// interpreter not yet running, save the callback information
	// until first evaluate() call which starts the interpreter
	// and passes this information to it.
	m_callback = callback;
    }
    return;
}

#endif

Y2NCursesUIComponent * Y2NCursesUIComponent::_ui = 0;

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
  return Y2UIComponent::Recode( str, from, to, outstr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUIComponent::idleLoop
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Y2NCursesUIComponent::idleLoop( int fd_ycp )
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
	      return new YWidgetEvent( dynamic_cast<YWidget *> (widget), reason );
	    
	  case menu:
	      return new YMenuEvent( selection );
	    
	  case cancel:
	      return new YCancelEvent();
	      
	  case timeout:
	      return new YTimeoutEvent();

	  case key:
	      return new YKeyEvent( keySymbol, dynamic_cast<YWidget *> (widget) );
	      
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
//	METHOD NAME : Y2NCursesUIComponent::userInput
//	METHOD TYPE : YEvent *
//
//	DESCRIPTION :
//
YEvent * Y2NCursesUIComponent::userInput( unsigned long timeout_millisec )
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
//	METHOD NAME : Y2NCursesUIComponent::pollInput
//	METHOD TYPE : YWidget *
//
//	DESCRIPTION :
//
YEvent * Y2NCursesUIComponent::pollInput()
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

YDialog * Y2NCursesUIComponent::createDialog( YWidgetOpt & opt )
{
  NCDialog * dialog = new NCDialog( opt );
  UIDBG << dialog << endl;
  return dialog;
}

void Y2NCursesUIComponent::showDialog( YDialog * dialog ) {
  NCDialog * dlg = dynamic_cast<NCDialog *>( dialog );
  UIDBG << dlg << endl;
  if ( dlg )
    dlg->showDialog();
}

void Y2NCursesUIComponent::closeDialog( YDialog * dialog ) {
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

YContainerWidget * Y2NCursesUIComponent::createSplit( YWidget * parent, YWidgetOpt & opt,
					     YUIDimension dimension )
{
  ONCREATE;
  return new NCSplit( dynamic_cast<NCWidget *>( parent ), opt,
		      dimension );
}

YContainerWidget * Y2NCursesUIComponent::createReplacePoint( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCReplacePoint( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * Y2NCursesUIComponent::createAlignment( YWidget * parent, YWidgetOpt & opt,
						 YAlignmentType halign,
						 YAlignmentType valign )
{
  ONCREATE;
  return new NCAlignment( dynamic_cast<NCWidget *>( parent ), opt,
			  halign, valign );
}

YContainerWidget * Y2NCursesUIComponent::createSquash( YWidget * parent, YWidgetOpt & opt,
					      bool hsquash,
					      bool vsquash )
{
  ONCREATE;
  return new NCSquash( dynamic_cast<NCWidget *>( parent ), opt,
		       hsquash, vsquash );
}

YContainerWidget * Y2NCursesUIComponent::createRadioButtonGroup( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCRadioButtonGroup( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * Y2NCursesUIComponent::createFrame( YWidget * parent, YWidgetOpt & opt,
					     const YCPString & label )
{
  ONCREATE;
  return new NCFrame( dynamic_cast<NCWidget *>( parent ), opt,
		      label );
}

///////////////////////////////////////////////////////////////////
//
// Widget creation functions - leaf widgets
//
///////////////////////////////////////////////////////////////////

YWidget * Y2NCursesUIComponent::createEmpty( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCEmpty( dynamic_cast<NCWidget *>( parent ), opt );
}


YWidget * Y2NCursesUIComponent::createSpacing( YWidget * parent, YWidgetOpt & opt,
				      float size,
				      bool horizontal, bool vertical )
{
  ONCREATE;
  return new NCSpacing( dynamic_cast<NCWidget *>( parent ), opt,
			size, horizontal, vertical );
}

YWidget * Y2NCursesUIComponent::createLabel( YWidget * parent, YWidgetOpt & opt,
				    const YCPString & text )
{
  ONCREATE;
  return new NCLabel( dynamic_cast<NCWidget *>( parent ), opt,
		      text );
}

YWidget * Y2NCursesUIComponent::createRichText( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & text )
{
  ONCREATE;
  return new NCRichText( dynamic_cast<NCWidget *>( parent ), opt,
			 text );
}

YWidget * Y2NCursesUIComponent::createLogView( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label,
				      int visibleLines,
				      int maxLines )
{
  ONCREATE;
  return new NCLogView( dynamic_cast<NCWidget *>( parent ), opt,
			label, visibleLines, maxLines );
}

YWidget * Y2NCursesUIComponent::createPushButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCPushButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * Y2NCursesUIComponent::createMenuButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCMenuButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * Y2NCursesUIComponent::createRadioButton( YWidget * parent, YWidgetOpt & opt,
					  YRadioButtonGroup * rbg,
					  const YCPString & label,
					  bool checked )
{
  ONCREATE;
  return new NCRadioButton( dynamic_cast<NCWidget *>( parent ), opt,
			    rbg, label, checked );
}

YWidget * Y2NCursesUIComponent::createCheckBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label,
				       bool checked )
{
  ONCREATE;
  return new NCCheckBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label, checked );
}

YWidget * Y2NCursesUIComponent::createTextEntry( YWidget * parent, YWidgetOpt & opt,
					const YCPString & label,
					const YCPString & text )
{
  ONCREATE;
  return new NCTextEntry( dynamic_cast<NCWidget *>( parent ), opt,
			  label, text );
}

YWidget *Y2NCursesUIComponent::createMultiLineEdit( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label,
					   const YCPString & initialText )
{
  ONCREATE;
  return new NCMultiLineEdit( dynamic_cast<NCWidget *>( parent ), opt,
			      label, initialText );
}

YWidget * Y2NCursesUIComponent::createSelectionBox( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label )
{
  ONCREATE;
  return new NCSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
			     label );
}

YWidget * Y2NCursesUIComponent::createMultiSelectionBox( YWidget *parent, YWidgetOpt & opt,
						const YCPString & label )
{
  ONCREATE;
  return new NCMultiSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
				  label );
}

YWidget * Y2NCursesUIComponent::createComboBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label )
{
  ONCREATE;
  return new NCComboBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label );
}

YWidget * Y2NCursesUIComponent::createTree( YWidget * parent, YWidgetOpt & opt,
				   const YCPString & label )
{
  ONCREATE;
  return new NCTree( dynamic_cast<NCWidget *>( parent ), opt,
		     label );
}

YWidget * Y2NCursesUIComponent::createTable( YWidget * parent, YWidgetOpt & opt,
				    vector<string> header )
{
  ONCREATE;
  return new NCTable( dynamic_cast<NCWidget *>( parent ), opt,
		      header );
}

YWidget * Y2NCursesUIComponent::createProgressBar( YWidget * parent, YWidgetOpt & opt,
					  const YCPString & label,
					  const YCPInteger & maxprogress,
					  const YCPInteger & progress )
{
  ONCREATE;
  return new NCProgressBar( dynamic_cast<NCWidget *>( parent ), opt,
			    label, maxprogress, progress );
}

YWidget * Y2NCursesUIComponent::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPByteblock /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUIComponent::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPString /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUIComponent::createImage( YWidget * parent, YWidgetOpt & opt,
				    ImageType /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUIComponent::createIntField( YWidget * parent, YWidgetOpt & opt,
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
//	METHOD NAME : Y2NCursesUIComponent::createPackageSelector
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Reads the layout term of the package selection dialog
// 		      and creates the widget tree.
//
YWidget * Y2NCursesUIComponent::createPackageSelector( YWidget * parent,
					      YWidgetOpt & opt,
					      const YCPString & floppyDevice )
{
    ONCREATE;
    return new NCPackageSelector ( this, dynamic_cast<NCWidget *>(parent), opt,
				   YD_HORIZ, floppyDevice->value() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUIComponent::runPkgSelection
//	METHOD TYPE : void
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YCPValue Y2NCursesUIComponent::runPkgSelection(  YWidget * selector )
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
//	METHOD NAME : Y2NCursesUIComponent::createPkgSpecial
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : creates special widgets used for the package selection
//		      dialog (which do not have a corresponding widget in qt-ui)
//
YWidget * Y2NCursesUIComponent::createPkgSpecial( YWidget *parent, YWidgetOpt &opt, const YCPString &subwidget )
{
    ONCREATE;

    YCPString pkgTable( "pkgTable" );

    if ( subwidget->compare( pkgTable ) == YO_EQUAL )
    {
	NCDBG << "Creating a NCPkgTable" << endl;
	return new NCPkgTable( dynamic_cast<NCWidget *>( parent ), opt );
    }
    else
    {
	NCERR <<  "PkgSpecial( "  << subwidget->toString() << " )  not found - take default `Label" << endl;
	return new NCLabel( dynamic_cast<NCWidget *>( parent ), opt, subwidget );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUIComponent::setLanguage
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUIComponent::setLanguage( const YCPTerm & term )
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
//	METHOD NAME : Y2NCursesUIComponent::setLanguage
//	METHOD TYPE : bool
//
//	DESCRIPTION : Expect lang to be 'language[_country][.encoding]'
//
//		      From version 9.0 on (UTF-8 support) setLanguage()
//		      don't have to look for the correct encoding.
//
bool Y2NCursesUIComponent::setLanguage( string lang )
{
  string encoding;
  string language = lang;

  string::size_type pos = language.find( '.' );
  if ( pos != string::npos )
  {
    encoding = language.substr( pos+1 );
    language.erase( pos );
  }

  NCDBG << "Language: " << language << " Encoding: " << ((encoding!="")?encoding:"NOT SET") << endl;
  
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUIComponent::setKeyboard
//	METHOD TYPE : YCPValue
//
//
YCPValue Y2NCursesUIComponent::setKeyboard()
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
//	METHOD NAME : Y2NCursesUIComponent::setConsoleFont
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION : UI::setConsoleFont() is called in Console.ycp.
//		      The terminal encoding must be set correctly.
//
YCPValue Y2NCursesUIComponent::setConsoleFont( const YCPString & console_magic,
				      const YCPString & font,
				      const YCPString & screen_map,
				      const YCPString & unicode_map,
				      const YCPString & lang )
{
  string cmd( "setfont" );
  cmd += " -c " + myTerm;
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
//	METHOD NAME : Y2NCursesUIComponent::hasFullUtf8Support
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool Y2NCursesUIComponent::hasFullUtf8Support()
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
//	METHOD NAME : Y2NCursesUIComponent::init_title
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Y2NCursesUIComponent::init_title()
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
//	METHOD NAME : Y2NCursesUIComponent::want_colors()
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool Y2NCursesUIComponent::want_colors()
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
//	METHOD NAME : Y2NCursesUIComponent::askForSaveFileName()
//	METHOD TYPE : YCPValue()
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUIComponent::askForSaveFileName( const YCPString & startDir,
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
//	METHOD NAME : Y2NCursesUIComponent::askForExistingFile()
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUIComponent::askForExistingFile( const YCPString & startDir,
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
//	METHOD NAME : Y2NCursesUIComponent::askForExistingDirectory()
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUIComponent::askForExistingDirectory( const YCPString & startDir,
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


