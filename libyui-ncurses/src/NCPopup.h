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
#ifndef NCPopup_h
#define NCPopup_h

#include <iosfwd>

#include "NCDialog.h"

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopup
//
//	DESCRIPTION :
//
class NCPopup : protected NCDialog {

  NCPopup & operator=( const NCPopup & );
  NCPopup            ( const NCPopup & );

  private:

    static YWidgetOpt wopt;

  protected:

    NCursesEvent postevent;

    void popupDialog();
    void popdownDialog();

    virtual NCursesEvent wHandleInput( int ch );

  protected:

    NCPopup( const wpos at, const bool boxed = true );
    virtual ~NCPopup();

    virtual bool postAgain() {
      if ( postevent != NCursesEvent::cancel )
	postevent.detail = 0;
      return false;
    }

  public:

    int post( NCursesEvent * returnevent = 0 ) {
      postevent = NCursesEvent();
      do {
	popupDialog();
      } while ( postAgain() );
      popdownDialog();
      if ( returnevent )
	*returnevent = postevent;
      return postevent.detail;
    }

};

#endif // NCPopup_h


