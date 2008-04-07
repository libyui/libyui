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

   File:       NCProgressBar.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCProgressBar_h
#define NCProgressBar_h

#include <iosfwd>

#include "YProgressBar.h"
#include "NCWidget.h"

class NCProgressBar;


class NCProgressBar : public YProgressBar, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCProgressBar & OBJ );

  NCProgressBar & operator=( const NCProgressBar & );
  NCProgressBar            ( const NCProgressBar & );

  private:
    typedef long long Value_t;

    NClabel  label;
    Value_t  maxval;
    Value_t  cval;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    void setDefsze();
    void tUpdate();

  protected:

    
    virtual const char * location() const { return "NCProgressBar"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

  public:

    NCProgressBar( YWidget * parent,
		   const string & label,
		   int maxValue = 100 );
    virtual ~NCProgressBar();

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

    virtual void setValue( int newValue );

    virtual void setEnabled( bool do_bv );
};


#endif // NCProgressBar_h
