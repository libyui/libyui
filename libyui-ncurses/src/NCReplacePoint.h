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

   File:       NCReplacePoint.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCReplacePoint_h
#define NCReplacePoint_h

#include <iosfwd>

#include "YReplacePoint.h"
#include "NCWidget.h"

class NCReplacePoint;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCReplacePoint
//
//	DESCRIPTION :
//
class NCReplacePoint : public YReplacePoint, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCReplacePoint & OBJ );

  NCReplacePoint & operator=( const NCReplacePoint & );
  NCReplacePoint            ( const NCReplacePoint & );

  private:

  protected:

    virtual const char * location() const { return "NCReplacePoint"; }

  public:

    NCReplacePoint( NCWidget * parent, const YWidgetOpt & opt );
    virtual ~NCReplacePoint();

    virtual long nicesize( YUIDimension dim ) { return YReplacePoint::nicesize( dim ); }
    virtual void setSize( long newwidth, long newheight );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCReplacePoint_h
