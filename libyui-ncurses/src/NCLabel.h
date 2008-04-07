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

/-*/
#ifndef NCLabel_h
#define NCLabel_h

#include <iosfwd>

#include "YLabel.h"
#include "NCWidget.h"

class NCLabel;


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

    NCLabel( YWidget * parent,
	     const string & text,
	     bool isHeading = false,
	     bool isOutputField = false );
    
    virtual ~NCLabel();

    virtual long nicesize( YUIDimension dim );

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setText( const string & nlabel );
    
    virtual void setEnabled( bool do_bv ); 

    virtual int preferredWidth();
    virtual int preferredHeight();
};


#endif // NCLabel_h
