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

   File:       NCFrame.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCFrame_h
#define NCFrame_h

#include <iosfwd>

#include "YFrame.h"
#include "NCWidget.h"

class NCFrame;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCFrame
//
//	DESCRIPTION :
//
class NCFrame : public YFrame, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCFrame & OBJ );

  NCFrame & operator=( const NCFrame & );
  NCFrame            ( const NCFrame & );

  private:

    NClabel label;

  protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCFrame"; }

    virtual void wRedraw();

  public:

    NCFrame( NCWidget * parent, const YWidgetOpt & opt,
	     const YCPString & label );
    virtual ~NCFrame();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setEnabling( bool do_bv );

};

///////////////////////////////////////////////////////////////////

#endif // NCFrame_h
