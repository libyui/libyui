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

   File:       NCLayoutBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCLayoutBox_h
#define NCLayoutBox_h

#include <iosfwd>

#include "YLayoutBox.h"
#include "NCWidget.h"

class NCLayoutBox;


class NCLayoutBox : public YLayoutBox, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCLayoutBox & OBJ );

    NCLayoutBox & operator=( const NCLayoutBox & );
    NCLayoutBox( const NCLayoutBox & );


protected:

    virtual const char * location() const
    {
	return primary() == YD_HORIZ ? "NC(H)LayoutBox" : "NC(V)LayoutBox" ;
    }

public:

    NCLayoutBox( YWidget * parent, YUIDimension dimension );
    virtual ~NCLayoutBox();

    virtual int preferredWidth() { return YLayoutBox::preferredWidth(); }
    virtual int preferredHeight() { return YLayoutBox::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void moveChild( YWidget * child, int newx, int newy );

    virtual void setEnabled( bool do_bv );
};


#endif // NCLayoutBox_h
