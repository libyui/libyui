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

   File:       NCSpacing.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCSpacing_h
#define NCSpacing_h

#include <iosfwd>

#include "YSpacing.h"
#include "YEmpty.h"
#include "YSpacing.h"
#include "NCWidget.h"

class NCSpacing;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCSpacing
//
//	DESCRIPTION :
//
class NCSpacing : public YSpacing, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCSpacing & OBJ );

  NCSpacing & operator=( const NCSpacing & );
  NCSpacing            ( const NCSpacing & );

  const char * l;

  protected:

    virtual const char * location() const { return l; }

  public:

    NCSpacing( NCWidget * parent, const YWidgetOpt & opt,
               float size, bool horizontal, bool vertical );
    virtual ~NCSpacing();

    virtual void setSize( long newwidth, long newheight );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCSpacing_h
