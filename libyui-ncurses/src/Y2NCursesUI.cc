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

   File:       Y2NCursesUI.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <sys/time.h>
#include <unistd.h>

#include "Y2Log.h"
#include "Y2NCursesUI.h"

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
#include "NCstring.h"

extern string language2encoding( string lang );

/******************************************************************
 **
 **
 **	FUNCTION NAME : operator<<
 **	FUNCTION TYPE : static ostream &
 **
 **	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const YUIInterpreter::EventType ev )
{
#define ENUM_OUT(v) case YUIInterpreter::v: return STREAM << #v
  switch ( ev ) {
    ENUM_OUT( ET_NONE );
    ENUM_OUT( ET_WIDGET );
    ENUM_OUT( ET_CANCEL );
    ENUM_OUT( ET_DEBUG );
    ENUM_OUT( ET_MENU );
  }
#undef ENUM_OUT
  return STREAM << "ET_unknown";
}

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
  return YUIInterpreter::Recode( str, from, to, outstr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::Y2NCursesUI
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
Y2NCursesUI::Y2NCursesUI( bool with_threads, Y2Component *callback )
    : YUIInterpreter(with_threads, callback)
{
  try {
    NCurses::init();
  }
  catch ( NCursesError & err ) {
    UIINT << err << endl;
    ::endwin();
    abort();
  }

  if ( getenv( "LANG" ) != NULL ) {
    setLanguage( getenv( "LANG" ) );
  }

  topmostConstructorHasFinished();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::~Y2NCursesUI
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
Y2NCursesUI::~Y2NCursesUI()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::idleLoop
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Y2NCursesUI::idleLoop( int fd_ycp )
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

  static YUIInterpreter::EventType typeOf( Type t ) {
    switch ( t ) {
    case button:
      return YUIInterpreter::ET_WIDGET;
      break;
    case menu:
      return YUIInterpreter::ET_MENU;
      break;
    case cancel:
      return YUIInterpreter::ET_CANCEL;
      break;
    case none:
      return YUIInterpreter::ET_NONE;
      break;

    default:
      UIINT << "Can't propagate NCursesEvent::" << t << endl;
      break;
    }
    return YUIInterpreter::ET_NONE;
  }

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

  YWidget * propagate( Y2NCursesUI & yui, YUIInterpreter::EventType * event ) {
    if ( type == menu ) {
      yui.setMenuSelection( selection );
    }
    if ( event ) {
      *event = typeOf( type );
    } else {
      UIINT << "Can't propagate through (EventType*)0" << endl;
    }
    UIDBG << *this << endl;
    return dynamic_cast<YWidget*>( widget );
  }
};

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::userInput
//	METHOD TYPE : YWidget *
//
//	DESCRIPTION :
//
YWidget * Y2NCursesUI::userInput( YDialog * dialog, EventType * event )
{
  return timeoutUserInput( dialog, event, 0 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::timeoutUserInput
//	METHOD TYPE : YWidget *
//
//	DESCRIPTION :
//
YWidget * Y2NCursesUI::timeoutUserInput( YDialog * dialog, EventType * event, unsigned timeout )
{
  NCDialog * ncd = static_cast<NCDialog *>( dialog );
  if ( !ncd ) {
    UIERR << "No NCDialog " << dialog << endl;
    return 0;
  }

  NCtoY2Event cevent;
  ncd->activate ( true );
  cevent = ncd->userInput( timeout ? (int)timeout : -1 );
  ncd->activate ( false );

  YWidget * ret = cevent.propagate( *this, event );
  UIDBG << (event?*event:-1) << " for " << ret << endl;
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::pollInput
//	METHOD TYPE : YWidget *
//
//	DESCRIPTION :
//
YWidget * Y2NCursesUI::pollInput( YDialog *dialog, EventType *event )
{
  NCDialog * ncd = static_cast<NCDialog *>( dialog );
  if ( !ncd ) {
    UIERR << "No NCDialog " << dialog << endl;
    return 0;
  }

  // no activation here, done in pollInput, if..
  NCtoY2Event cevent = ncd->pollInput();

  YWidget * ret = cevent.propagate( *this, event );
  UIDBG << (event?*event:-1) << " for " << ret << endl;
  return ret;
}

///////////////////////////////////////////////////////////////////
//
// Dialog and widgets
//
///////////////////////////////////////////////////////////////////

YDialog * Y2NCursesUI::createDialog( YWidgetOpt & opt )
{
  NCDialog * dialog = new NCDialog( opt );
  UIDBG << dialog << endl;
  return dialog;
}

void Y2NCursesUI::showDialog( YDialog * dialog ) {
  NCDialog * dlg = dynamic_cast<NCDialog *>( dialog );
  UIDBG << dlg << endl;
  if ( dlg )
    dlg->showDialog();
}

void Y2NCursesUI::closeDialog( YDialog * dialog ) {
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

YContainerWidget * Y2NCursesUI::createSplit( YWidget * parent, YWidgetOpt & opt,
					     YUIDimension dimension )
{
  ONCREATE;
  return new NCSplit( dynamic_cast<NCWidget *>( parent ), opt,
		      dimension );
}

YContainerWidget * Y2NCursesUI::createReplacePoint( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCReplacePoint( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * Y2NCursesUI::createAlignment( YWidget * parent, YWidgetOpt & opt,
						 YAlignmentType halign,
						 YAlignmentType valign )
{
  ONCREATE;
  return new NCAlignment( dynamic_cast<NCWidget *>( parent ), opt,
			  halign, valign );
}

YContainerWidget * Y2NCursesUI::createSquash( YWidget * parent, YWidgetOpt & opt,
					      bool hsquash,
					      bool vsquash )
{
  ONCREATE;
  return new NCSquash( dynamic_cast<NCWidget *>( parent ), opt,
		       hsquash, vsquash );
}

YContainerWidget * Y2NCursesUI::createRadioButtonGroup( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCRadioButtonGroup( dynamic_cast<NCWidget *>( parent ), opt );
}

YContainerWidget * Y2NCursesUI::createFrame( YWidget * parent, YWidgetOpt & opt,
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

YWidget * Y2NCursesUI::createEmpty( YWidget * parent, YWidgetOpt & opt )
{
  ONCREATE;
  return new NCEmpty( dynamic_cast<NCWidget *>( parent ), opt );
}


YWidget * Y2NCursesUI::createSpacing( YWidget * parent, YWidgetOpt & opt,
				      float size,
				      bool horizontal, bool vertical )
{
  ONCREATE;
  return new NCSpacing( dynamic_cast<NCWidget *>( parent ), opt,
			size, horizontal, vertical );
}

YWidget * Y2NCursesUI::createLabel( YWidget * parent, YWidgetOpt & opt,
				    const YCPString & text )
{
  ONCREATE;
  return new NCLabel( dynamic_cast<NCWidget *>( parent ), opt,
		      text );
}

YWidget * Y2NCursesUI::createRichText( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & text )
{
  ONCREATE;
  return new NCRichText( dynamic_cast<NCWidget *>( parent ), opt,
			 text );
}

YWidget * Y2NCursesUI::createLogView( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label,
				      int visibleLines,
				      int maxLines )
{
  ONCREATE;
  return new NCLogView( dynamic_cast<NCWidget *>( parent ), opt,
			label, visibleLines, maxLines );
}

YWidget * Y2NCursesUI::createPushButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCPushButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * Y2NCursesUI::createMenuButton( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label )
{
  ONCREATE;
  return new NCMenuButton( dynamic_cast<NCWidget *>( parent ), opt,
			   label );
}

YWidget * Y2NCursesUI::createRadioButton( YWidget * parent, YWidgetOpt & opt,
					  YRadioButtonGroup * rbg,
					  const YCPString & label,
					  bool checked )
{
  ONCREATE;
  return new NCRadioButton( dynamic_cast<NCWidget *>( parent ), opt,
			    rbg, label, checked );
}

YWidget * Y2NCursesUI::createCheckBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label,
				       bool checked )
{
  ONCREATE;
  return new NCCheckBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label, checked );
}

YWidget * Y2NCursesUI::createTextEntry( YWidget * parent, YWidgetOpt & opt,
					const YCPString & label,
					const YCPString & text )
{
  ONCREATE;
  return new NCTextEntry( dynamic_cast<NCWidget *>( parent ), opt,
			  label, text );
}

YWidget *Y2NCursesUI::createMultiLineEdit( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label,
					   const YCPString & initialText )
{
  ONCREATE;
  return new NCMultiLineEdit( dynamic_cast<NCWidget *>( parent ), opt,
			      label, initialText );
}

YWidget * Y2NCursesUI::createSelectionBox( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label )
{
  ONCREATE;
  return new NCSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
			     label );
}

YWidget * Y2NCursesUI::createMultiSelectionBox( YWidget *parent, YWidgetOpt & opt,
						const YCPString & label )
{
  ONCREATE;
  return new NCMultiSelectionBox( dynamic_cast<NCWidget *>( parent ), opt,
				  label );
}

YWidget * Y2NCursesUI::createComboBox( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label )
{
  ONCREATE;
  return new NCComboBox( dynamic_cast<NCWidget *>( parent ), opt,
			 label );
}

YWidget * Y2NCursesUI::createTree( YWidget * parent, YWidgetOpt & opt,
				   const YCPString & label )
{
  ONCREATE;
  return new NCTree( dynamic_cast<NCWidget *>( parent ), opt,
		     label );
}

YWidget * Y2NCursesUI::createTable( YWidget * parent, YWidgetOpt & opt,
				    vector<string> header )
{
  ONCREATE;
  return new NCTable( dynamic_cast<NCWidget *>( parent ), opt,
		      header );
}

YWidget * Y2NCursesUI::createProgressBar( YWidget * parent, YWidgetOpt & opt,
					  const YCPString & label,
					  const YCPInteger & maxprogress,
					  const YCPInteger & progress )
{
  ONCREATE;
  return new NCProgressBar( dynamic_cast<NCWidget *>( parent ), opt,
			    label, maxprogress, progress );
}

YWidget * Y2NCursesUI::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPByteblock /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUI::createImage( YWidget * parent, YWidgetOpt & opt,
				    YCPString /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUI::createImage( YWidget * parent, YWidgetOpt & opt,
				    ImageType /* not used */,
				    YCPString defaulttext )
{
  ONCREATE;
  return new NCImage( dynamic_cast<NCWidget *>( parent ), opt,
		      defaulttext );
}

YWidget * Y2NCursesUI::createIntField( YWidget * parent, YWidgetOpt & opt,
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
//
//	METHOD NAME : Y2NCursesUI::setLanguage
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUI::setLanguage( const YCPTerm & term )
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
//	METHOD NAME : Y2NCursesUI::setLanguage
//	METHOD TYPE : bool
//
//	DESCRIPTION : expect lang to be 'language[_country][.encoding]'
//
bool Y2NCursesUI::setLanguage( string lang )
{
  string encoding;
  UIMIL << form( "Encoding for '%s' ", lang.c_str() );

  string::size_type pos = lang.find( '.' );
  if ( pos != string::npos ) {
    encoding = lang.substr( pos+1 );
  } else {
    pos = lang.find( '_' );
    if ( pos != string::npos ) {
      lang.erase( pos );
    }
    encoding = language2encoding( lang );
  }

  UIMIL << form( "is '%s'\n", encoding.c_str() );
  if ( NCstring::setDefaultEncoding( encoding ) )
    Redraw();

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::setConsoleFont
//	METHOD TYPE : YCPValue
//
//	DESCRIPTION :
//
YCPValue Y2NCursesUI::setConsoleFont( const YCPString & console_magic,
				      const YCPString & font,
				      const YCPString & screen_map,
				      const YCPString & unicode_map,
				      const YCPString & encoding )
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
  if ( ret ) {
    UIERR << cmd.c_str() << " returned " << ret << endl;
    Refresh();
    return YCPVoid();
  }

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

  if ( NCstring::setDefaultEncoding( encoding->value() ) )
    Redraw();
  else
    Refresh();

  return YCPVoid();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2NCursesUI::init_title
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Y2NCursesUI::init_title()
{
    string title_ti( "YaST2" );
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
//	METHOD NAME : Y2NCursesUI::want_colors()
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool Y2NCursesUI::want_colors()
{
  if ( getenv( "Y2NCURSES_BW" ) != NULL ) {
    UIMIL << "Y2NCURSES_BW is set - won't use colors" << endl;
    return false;
  }
  return true;
}


