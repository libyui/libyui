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

   File:       NCNew.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCNew_h
#define NCNew_h

#include <iosfwd>

#include "YWidget.h"
#include "NCWidget.h"

class NCNew;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCNew
//
//	DESCRIPTION :
//
class NCNew : public YWidget, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCNew & OBJ );

  NCNew & operator=( const NCNew & );
  NCNew            ( const NCNew & );

  private:

    bool    heading;
    NClabel label;

  protected:

    virtual const char * location() const { return "NCNew"; }

    virtual void wRedraw();

  public:

    NCNew( NCWidget * parent, YWidgetOpt & opt,
	     const YCPString & text );
    virtual ~NCNew();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
};

///////////////////////////////////////////////////////////////////

#endif // NCNew_h
