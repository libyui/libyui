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

   File:       NCWeight.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCWeight_h
#define NCWeight_h

#include <iosfwd>

#include "YWeight.h"
#include "NCWidget.h"

class NCWeight;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCWeight
//
//	DESCRIPTION :
//
class NCWeight : public YWeight, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCWeight & OBJ );

  NCWeight & operator=( const NCWeight & );
  NCWeight            ( const NCWeight & );

  private:

  protected:

    virtual const char * location() const { return "NCWeight"; }

  public:

    NCWeight( NCWidget * parent, YUIDimension dim, long weight );
    virtual ~NCWeight();

    virtual long minsize(YUIDimension dim)    { return YWeight::minsize( dim ); }
    virtual long nicesize( YUIDimension dim ) { return YWeight::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );
};

///////////////////////////////////////////////////////////////////

#endif // NCWeight_h
