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

   File:       NCRadioButtonGroup.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCRadioButtonGroup.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButtonGroup::NCRadioButtonGroup
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCRadioButtonGroup::NCRadioButtonGroup( NCWidget * parent, YWidgetOpt & opt )
    : YRadioButtonGroup( opt )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButtonGroup::~NCRadioButtonGroup
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCRadioButtonGroup::~NCRadioButtonGroup()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButtonGroup::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButtonGroup::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YRadioButtonGroup::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButtonGroup::addRadioButton
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButtonGroup::addRadioButton( YRadioButton *button )
{
  YRadioButtonGroup::addRadioButton( button );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButtonGroup::removeRadioButton
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButtonGroup::removeRadioButton( YRadioButton *button )
{
  YRadioButtonGroup::removeRadioButton( button );
}

