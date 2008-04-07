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

   File:       NCPopupTextEntry.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPopupTextEntry.h"



NCPopupTextEntry::NCPopupTextEntry( const wpos at,
				    const string & label,
				    const string & text,
				    unsigned maxInput,
				    unsigned maxFld,
				    NCInputField::FTYPE t )
    : NCPopup( at )
    , wtext( 0 )
{
  wtext = new NCInputField( this, 
			    label,
			    false,	    // passwordMode = false
			    maxInput,
			    maxFld
			    );
  YUI_CHECK_NEW( wtext );
  
  wtext->setValue( text );
  wtext->setFldtype( t );
  wtext->setReturnOnReturn( true );

}



NCPopupTextEntry::~NCPopupTextEntry()
{
}



bool NCPopupTextEntry::postAgain()
{
  if ( postevent == NCursesEvent::button )
    postevent.detail = 0;
  return false;
}
