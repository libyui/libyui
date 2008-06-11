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

   File:       NCSquash.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCSquash.h"



NCSquash::NCSquash( YWidget * parent,
		    bool hsquash, bool vsquash )
	: YSquash( parent, hsquash, vsquash )
	, NCWidget( parent )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCSquash::~NCSquash()
{
    yuiDebug() << endl;
}


void NCSquash::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YSquash::setSize( newwidth, newheight );
}


void NCSquash::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YSquash::setEnabled( do_bv );
}
