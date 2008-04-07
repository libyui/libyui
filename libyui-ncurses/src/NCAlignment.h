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

/-*/
#ifndef NCAlignment_h
#define NCAlignment_h

#include <iosfwd>

#include "YAlignment.h"
#include "NCWidget.h"

class NCAlignment;


class NCAlignment : public YAlignment, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCAlignment & OBJ );

  NCAlignment & operator=( const NCAlignment & );
  NCAlignment            ( const NCAlignment & );

  private:

  protected:

    virtual const char * location() const { return "NCAlignment"; }

  public:

    NCAlignment( YWidget * parent, YAlignmentType halign, YAlignmentType valign );
    virtual ~NCAlignment();

    virtual int preferredWidth() { return YAlignment::preferredWidth(); }
    virtual int preferredHeight() { return YAlignment::preferredHeight(); }
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void moveChild( YWidget * child, int newx, int newy );

    virtual void setEnabled( bool do_bv );
};


#endif // NCAlignment_h
