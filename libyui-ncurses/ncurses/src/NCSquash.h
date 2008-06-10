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

   File:       NCSquash.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCSquash_h
#define NCSquash_h

#include <iosfwd>

#include "YSquash.h"
#include "NCWidget.h"


class NCSquash : public YSquash, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCSquash & OBJ );

private:
    
    NCSquash & operator=( const NCSquash & );
    NCSquash( const NCSquash & );

protected:

    virtual const char * location() const { return "NCSquash"; }

public:

    NCSquash( YWidget * parent, bool hsquash, bool vsquash );
    virtual ~NCSquash();

    virtual int preferredWidth() { return YSquash::preferredWidth(); }
    virtual int preferredHeight() { return YSquash::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCSquash_h
