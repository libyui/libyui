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

/-*/
#ifndef NCFrame_h
#define NCFrame_h

#include <iosfwd>

#include "YFrame.h"
#include "NCWidget.h"

class NCFrame;


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

    NCFrame( YWidget * parent, const string & label );
    virtual ~NCFrame();

    virtual long nicesize( YUIDimension dim );

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    //virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
    virtual void setEnabled( bool do_bv );
};


#endif // NCFrame_h
