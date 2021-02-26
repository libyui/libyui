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

   File:       NCReplacePoint.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCReplacePoint_h
#define NCReplacePoint_h

#include <iosfwd>

#include "YReplacePoint.h"
#include "NCWidget.h"

class NCReplacePoint;


class NCReplacePoint : public YReplacePoint, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCReplacePoint & OBJ );

    NCReplacePoint & operator=( const NCReplacePoint & );
    NCReplacePoint( const NCReplacePoint & );

protected:

    virtual const char * location() const { return "NCReplacePoint"; }

public:

    NCReplacePoint( YWidget * parent );
    virtual ~NCReplacePoint();

    virtual int preferredWidth() { return YReplacePoint::preferredWidth(); }
    virtual int preferredHeight() { return YReplacePoint::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCReplacePoint_h
