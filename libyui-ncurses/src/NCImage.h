/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCImage.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCImage_h
#define NCImage_h

#include <iosfwd>

#include "YImage.h"
#include "NCWidget.h"

class NCImage;


class NCImage : public YImage, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCImage & OBJ );

    NCImage & operator=( const NCImage & );
    NCImage( const NCImage & );


    NClabel label;

protected:

    virtual const char * location() const { return "NCImage"; }

public:

    NCImage( YWidget * parent, string defaulttext, bool animated = false );
    virtual ~NCImage();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCImage_h
