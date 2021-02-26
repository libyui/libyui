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

   File:       NCComboBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCComboBox_h
#define NCComboBox_h

#include <iosfwd>
#include <list>

#include "YComboBox.h"
#include "NCWidget.h"
#include "NCApplication.h"
#include "YItem.h"

using std::list;


class NCComboBox : public YComboBox, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCComboBox & OBJ );

    NCComboBox & operator=( const NCComboBox & );
    NCComboBox( const NCComboBox & );


    bool     mayedit;
    NClabel  label;
    NCstring privText;
    wstring   buffer;
    bool     modified;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    string::size_type fldstart;
    string::size_type fldlength;
    string::size_type curpos;
    string::size_type longest_line;

    NCstring validChars;

    list<string> deflist;
    int  index;

    void setDefsze();
    void tUpdate();

    bool haveUtf8() { return YUI::app()->hasFullUtf8Support(); }

protected:

    virtual const char * location() const { return "NCComboBox"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();
    virtual void wRecoded();

    int listPopup();

    bool validKey( wint_t key ) const;

    // specifies how much characters can be inserted. -1 for unlimited input
    int InputMaxLength;

public:

    NCComboBox( YWidget * parent,
		const string & label,
		bool editable );
    virtual ~NCComboBox();

    virtual void addItem( YItem * item );
    virtual void selectItem( YItem * item, bool selected = true );
    
    void addItem( const string & label, bool selected );
    
    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setText( const string & ntext );
    virtual string text();

    virtual void setValidChars( const string & validchars );

    virtual int getCurrentItem() const;
    virtual void setCurrentItem( int index );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    unsigned int getListSize( ) { return deflist.size(); }

    void deleteAllItems();

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int  nr );

};


#endif // NCComboBox_h
