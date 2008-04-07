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

   File:       NCEmpty.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCEmpty_h
#define NCEmpty_h

#include <iosfwd>

#include "YEmpty.h"
#include "NCWidget.h"

class NCEmpty;


class NCEmpty : public YEmpty, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCEmpty & OBJ );

  NCEmpty & operator=( const NCEmpty & );
  NCEmpty            ( const NCEmpty & );

  private:

  protected:

    virtual const char * location() const { return "NCEmpty"; }

  public:

    NCEmpty( YWidget * parent );
    virtual ~NCEmpty();

    // virtual long nicesize( YUIDimension dim ){ return YEmpty::nicesize( dim ); }
    virtual int preferredWidth() { return YEmpty::preferredWidth(); }
    virtual int preferredHeight() { return YEmpty::preferredHeight(); }
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
    //virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
     virtual void setEnabled( bool do_bv );
};


#endif // NCEmpty_h
