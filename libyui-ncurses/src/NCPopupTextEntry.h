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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupTextEntry_h
#define NCPopupTextEntry_h

#include <iosfwd>

#include "NCPopup.h"
#include "NCTextEntry.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupTextEntry
//
//	DESCRIPTION :
//
class NCPopupTextEntry : public NCPopup {

  NCPopupTextEntry & operator=( const NCPopupTextEntry & );
  NCPopupTextEntry            ( const NCPopupTextEntry & );

  private:

    NCTextEntry * wtext;

    virtual bool postAgain();

  public:

    NCPopupTextEntry( const wpos at,
		      const YCPString & label,
		      const YCPString & text,
		      unsigned maxInput  = 0,
		      unsigned maxFld    = 0,
		      NCTextEntry::FTYPE t = NCTextEntry::PLAIN );

    virtual ~NCPopupTextEntry();

    void setText( const YCPString & ntext ) { wtext->setText( ntext ); }
    YCPString getText()                     { return wtext->getText(); }
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupTextEntry_h
