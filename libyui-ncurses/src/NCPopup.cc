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

   File:       NCPopup.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPopup.h"


NCPopup::NCPopup( const wpos at, const bool boxed )
	: NCDialog( YPopupDialog, at, boxed )
{
}


NCPopup::~NCPopup()
{
}


void NCPopup::popupDialog()
{
    initDialog();
    showDialog();
    activate( true );

    while ( !( postevent = userInput() ) )
	;

    activate( false );
}


void NCPopup::popdownDialog()
{
    closeDialog();
}


NCursesEvent NCPopup::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}


int NCPopup::post( NCursesEvent * returnevent )
{
    postevent = NCursesEvent();

    do
    {
	popupDialog();
    }
    while ( postAgain() );

    popdownDialog();

    if ( returnevent )
	*returnevent = postevent;

    yuiDebug() << "Return event.detail:  " << postevent.detail << endl;

    return postevent.detail;
}
