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

   File:       NCEmpty.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCEmpty_h
#define NCEmpty_h

#include <iosfwd>

#include "YEmpty.h"
#include "NCWidget.h"

class NCEmpty;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCEmpty
//
//	DESCRIPTION :
//
class NCEmpty : public YEmpty, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCEmpty & OBJ );

  NCEmpty & operator=( const NCEmpty & );
  NCEmpty            ( const NCEmpty & );

  private:

  protected:

    virtual const char * location() const { return "NCEmpty"; }

  public:

    NCEmpty( NCWidget * parent, YWidgetOpt & opt );
    virtual ~NCEmpty();

    virtual long nicesize( YUIDimension dim ){ return YEmpty::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );
};

///////////////////////////////////////////////////////////////////

#endif // NCEmpty_h
