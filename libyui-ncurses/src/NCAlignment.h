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

   File:       NCAlignment.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCAlignment_h
#define NCAlignment_h

#include <iosfwd>

#include "YAlignment.h"
#include "NCWidget.h"

class NCAlignment;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCAlignment
//
//	DESCRIPTION :
//
class NCAlignment : public YAlignment, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCAlignment & OBJ );

  NCAlignment & operator=( const NCAlignment & );
  NCAlignment            ( const NCAlignment & );

  private:

  protected:

    virtual const char * location() const { return "NCAlignment"; }

  public:

    NCAlignment( NCWidget * parent, YWidgetOpt & opt,
		 YAlignmentType halign, YAlignmentType valign );
    virtual ~NCAlignment();

    virtual long nicesize( YUIDimension dim ) { return YAlignment::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );

    virtual void moveChild( YWidget * child, long newx, long newy );
};

///////////////////////////////////////////////////////////////////

#endif // NCAlignment_h
