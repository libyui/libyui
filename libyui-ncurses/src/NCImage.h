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

   File:       NCImage.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCImage_h
#define NCImage_h

#include <iosfwd>

#include "YImage.h"
#include "NCWidget.h"

class NCImage;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCImage
//
//	DESCRIPTION :
//
class NCImage : public YImage, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCImage & OBJ );

  NCImage & operator=( const NCImage & );
  NCImage            ( const NCImage & );

  private:

    NClabel label;

  protected:

    virtual const char * location() const { return "NCImage"; }

  public:

    NCImage( NCWidget * parent, YWidgetOpt & opt, YCPString defaulttext );
    virtual ~NCImage();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );
    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCImage_h
