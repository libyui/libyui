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

   File:       NCFrame.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCFrame_h
#define NCFrame_h

#include <iosfwd>

#include "YFrame.h"
#include "NCWidget.h"

class NCFrame;


class NCFrame : public YFrame, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCFrame & OBJ );

    NCFrame & operator=( const NCFrame & );
    NCFrame( const NCFrame & );


    NClabel label;

protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCFrame"; }

    virtual void wRedraw();

public:

    NCFrame( YWidget * parent, const string & label );
    virtual ~NCFrame();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setEnabled( bool do_bv );
};


#endif // NCFrame_h
