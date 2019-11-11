/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCProgressBar.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCProgressBar_h
#define NCProgressBar_h

#include <iosfwd>

#include <yui/YProgressBar.h>
#include "NCWidget.h"

class NCProgressBar;


class NCProgressBar : public YProgressBar, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCProgressBar & obj );

    NCProgressBar & operator=( const NCProgressBar & );
    NCProgressBar( const NCProgressBar & );


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
		   const std::string & label,
		   int maxValue = 100 );
    virtual ~NCProgressBar();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setValue( int newValue );

    virtual void setEnabled( bool do_bv );
};


#endif // NCProgressBar_h
