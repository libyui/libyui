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

   File:       NCLabel.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCLabel_h
#define NCLabel_h

#include <iosfwd>

#include "YLabel.h"
#include "NCWidget.h"

class NCLabel;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCLabel
//
//	DESCRIPTION :
//
class NCLabel : public YLabel, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCLabel & OBJ );

  NCLabel & operator=( const NCLabel & );
  NCLabel            ( const NCLabel & );

  private:

    bool    heading;
    NClabel label;

  protected:

    virtual const char * location() const { return "NCLabel"; }

    virtual void wRedraw();

  public:

    NCLabel( NCWidget * parent, const YWidgetOpt & opt,
	     const YCPString & text );
    virtual ~NCLabel();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCLabel_h
