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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupTextEntry.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTextEntry::NCPopupTextEntry
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupTextEntry::NCPopupTextEntry( const wpos at,
				    const YCPString & label,
				    const YCPString & text,
				    unsigned maxInput,
				    unsigned maxFld,
				    NCTextEntry::FTYPE t )
    : NCPopup( at )
    , wtext( 0 )
{
  YWidgetOpt opt;
  //opt.notifyMode.setValue( true );
  wtext = new NCTextEntry( this, opt,
			   label, text,
			   maxInput, maxFld );
  wtext->setFldtype( t );
  wtext->setReturnOnReturn( true );
  addChild( wtext );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTextEntry::~NCPopupTextEntry
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupTextEntry::~NCPopupTextEntry()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTextEntry::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupTextEntry::postAgain()
{
  if ( postevent == NCursesEvent::button )
    postevent.detail = 0;
  return false;
}
