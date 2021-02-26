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

   File:       NCPopupTextEntry.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupTextEntry_h
#define NCPopupTextEntry_h

#include <iosfwd>

#include "NCPopup.h"
#include "NCInputField.h"



class NCPopupTextEntry : public NCPopup {

  NCPopupTextEntry & operator=( const NCPopupTextEntry & );
  NCPopupTextEntry            ( const NCPopupTextEntry & );

  private:

    NCInputField * wtext;

    virtual bool postAgain();

  public:

    NCPopupTextEntry( const wpos at,
		      const string & label,
		      const string & text,
		      unsigned maxInput  = 0,
		      unsigned maxFld    = 0,
		      NCInputField::FTYPE t = NCInputField::PLAIN );

    virtual ~NCPopupTextEntry();

    void   setValue( const string & ntext ) { wtext->setValue( ntext ); }
    string value() { return wtext->value(); }
};



#endif // NCPopupTextEntry_h
