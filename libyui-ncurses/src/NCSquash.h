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

   File:       NCSquash.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCSquash_h
#define NCSquash_h

#include <iosfwd>

#include "YSquash.h"
#include "NCWidget.h"

class NCSquash;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCSquash
//
//	DESCRIPTION :
//
class NCSquash : public YSquash, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCSquash & OBJ );

  NCSquash & operator=( const NCSquash & );
  NCSquash            ( const NCSquash & );

  private:

  protected:

    virtual const char * location() const { return "NCSquash"; }

  public:

    NCSquash( NCWidget * parent, YWidgetOpt & opt,
	      bool hsquash, bool vsquash );
    virtual ~NCSquash();

    virtual long nicesize( YUIDimension dim ) { return YSquash::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCSquash_h
