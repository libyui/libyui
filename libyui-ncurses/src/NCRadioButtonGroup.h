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

   File:       NCRadioButtonGroup.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCRadioButtonGroup_h
#define NCRadioButtonGroup_h

#include <iosfwd>

#include "YRadioButtonGroup.h"
#include "NCRadioButton.h"
#include "NCWidget.h"

class NCRadioButtonGroup;


class NCRadioButtonGroup : public YRadioButtonGroup, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCRadioButtonGroup & OBJ );

    NCRadioButtonGroup & operator=( const NCRadioButtonGroup & );
    NCRadioButtonGroup( const NCRadioButtonGroup & );

    int focusId;

protected:

    virtual const char * location() const { return "NCRadioButtonGroup"; }

public:

    NCRadioButtonGroup( YWidget * parent );
    virtual ~NCRadioButtonGroup();

    virtual int preferredWidth() { return YRadioButtonGroup::preferredWidth(); }
    virtual int preferredHeight() { return YRadioButtonGroup::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void addRadioButton( YRadioButton *button );
    virtual void removeRadioButton( YRadioButton *button );

    virtual void setEnabled( bool do_bv );

    void focusNextButton( );
    void focusPrevButton( );

};


#endif // NCRadioButtonGroup_h
