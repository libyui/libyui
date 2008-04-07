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

/-*/
#ifndef NCSpacing_h
#define NCSpacing_h

#include <iosfwd>

#include "YSpacing.h"
#include "YEmpty.h"
#include "YSpacing.h"
#include "NCWidget.h"

class NCSpacing;


class NCSpacing : public YSpacing, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCSpacing & OBJ );

  NCSpacing & operator=( const NCSpacing & );
  NCSpacing            ( const NCSpacing & );

  const char * l;

  protected:

    virtual const char * location() const { return l; }

  public:

    NCSpacing( YWidget * parent,
	       YUIDimension dim,
	       bool stretchable = false,
	       YLayoutSize_t layoutUnits = 0.0 );
    
    virtual ~NCSpacing();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual int preferredWidth()  { return YSpacing::preferredWidth(); }
    virtual int preferredHeight() { return YSpacing::preferredHeight(); }
    
    //virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
    virtual void setEnabled( bool do_bv ); 
};


#endif // NCSpacing_h
