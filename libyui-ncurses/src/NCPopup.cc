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

   File:       NCPopup.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopup.h"

///////////////////////////////////////////////////////////////////

YWidgetOpt NCPopup::wopt;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::NCPopup
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopup::NCPopup( const wpos at, const bool boxed )
    : NCDialog( wopt, at, boxed )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::~NCPopup
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopup::~NCPopup()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::popupDialog
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopup::popupDialog()
{
  initDialog();
  showDialog();
  activate ( true );
  while ( !(postevent = userInput()) )
    ;
  activate ( false );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::popdownDialog
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopup::popdownDialog()
{
  closeDialog();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopup::wHandleInput( wint_t ch )
{
  if ( ch == 27 ) // ESC
    return NCursesEvent::cancel;
  return NCDialog::wHandleInput( ch );
}

