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

   File:       NCLogView.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCLogView_h
#define NCLogView_h

#include <iosfwd>

#include <yui/YLogView.h>
#include "NCPadWidget.h"


class NCLogView : public YLogView, public NCPadWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCLogView & OBJ );

    NCLogView & operator=( const NCLogView & );
    NCLogView( const NCLogView & );


    NCtext text;

protected:

    virtual const char * location() const { return "NCLogView"; }

    virtual void wRedraw();
    virtual void wRecoded();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

public:

    NCLogView( YWidget * parent,
	       const string & label,
	       int visibleLines,
	       int maxLines );
    virtual ~NCLogView();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );
    virtual void displayLogText( const string & ntext );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }
};


#endif // NCLogView_h
