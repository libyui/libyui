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

   File:       NCSplit.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCSplit_h
#define NCSplit_h

#include <iosfwd>

#include "YSplit.h"
#include "NCWidget.h"

class NCSplit;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCSplit
//
//	DESCRIPTION :
//
class NCSplit : public YSplit, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCSplit & OBJ );

  NCSplit & operator=( const NCSplit & );
  NCSplit            ( const NCSplit & );

  private:

  protected:

    virtual const char * location() const {
      return dimension() == YD_HORIZ ? "NC(H)Split" : "NC(V)Split" ;
    }

  public:

    NCSplit( NCWidget * parent, YWidgetOpt & opt,
	     YUIDimension dimension );
    virtual ~NCSplit();

    virtual long nicesize( YUIDimension dim ) { return YSplit::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );

    virtual void moveChild( YWidget * child, long newx, long newy );
};

///////////////////////////////////////////////////////////////////

#endif // NCSplit_h
