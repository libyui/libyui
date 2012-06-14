/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

   File:       NCLayoutBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCLayoutBox_h
#define NCLayoutBox_h

#include <iosfwd>

#include <yui/YLayoutBox.h>
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
