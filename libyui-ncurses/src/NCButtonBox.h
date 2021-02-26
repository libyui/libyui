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

   File:       NCButtonBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCButtonBox_h
#define NCButtonBox_h

#include <iosfwd>

#include "YButtonBox.h"
#include "NCWidget.h"

class NCButtonBox;


class NCButtonBox : public YButtonBox, public NCWidget
{

public:

    NCButtonBox( YWidget * parent );
    virtual ~NCButtonBox();

    virtual void moveChild( YWidget * child, int newX, int newY );
    virtual void setSize( int newWidth, int newHeight );
    virtual void setEnabled( bool enabled );
    
private:

    friend std::ostream & operator<<( std::ostream & stream, const NCButtonBox & widget );

    NCButtonBox & operator=( const NCButtonBox & );
    NCButtonBox( const NCButtonBox & );

};


#endif // NCButtonBox_h
