/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

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

   File:       NCMenuButton.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCMenuButton_h
#define NCMenuButton_h

#include <iosfwd>

#include "YMenuButton.h"
#include "NCApplication.h"
#include "NCWidget.h"


class NCMenuButton : public YMenuButton, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCMenuButton & OBJ );

    NCMenuButton & operator=( const NCMenuButton & );
    NCMenuButton( const NCMenuButton & );

    NClabel label;
    bool haveUtf8() { return YUI::app()->hasFullUtf8Support(); }

protected:

    virtual const char * location() const { return "NCMenuButton"; }

    virtual void wRedraw();

    NCursesEvent postMenu();

public:

    NCMenuButton( YWidget * parent,
		  string label );
    virtual ~NCMenuButton();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );
    
    virtual void rebuildMenuTree();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setLabel( const string & nlabel );

    virtual void setEnabled( bool do_bv );

    YMenuItem * findItem( int selection ) { return findMenuItem( selection ); }

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }
};


#endif // NCMenuButton_h
