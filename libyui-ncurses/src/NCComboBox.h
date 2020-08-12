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

   File:       NCComboBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCComboBox_h
#define NCComboBox_h

#include <iosfwd>
#include <list>

#include <yui/YComboBox.h>
#include "NCWidget.h"
#include "NCApplication.h"
#include <yui/YItem.h>


class NCComboBox : public YComboBox, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCComboBox & obj );

    NCComboBox & operator=( const NCComboBox & );
    NCComboBox( const NCComboBox & );


    bool     mayedit;
    NClabel  label;
    NCstring privText;
    std::wstring   buffer;
    bool     modified;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    std::string::size_type fldstart;
    std::string::size_type fldlength;
    std::string::size_type curpos;
    std::string::size_type longest_line;

    NCstring validChars;

    std::list<std::string> deflist;
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
		const std::string & label,
		bool editable );
    virtual ~NCComboBox();

    virtual void addItem( YItem * item );
    virtual void selectItem( YItem * item, bool selected = true );

    void addItem( const std::string & label, bool selected );

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setText( const std::string & ntext );
    virtual std::string text();

    virtual void setValidChars( const std::string & validchars );

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

    unsigned int getListSize() { return deflist.size(); }

    void deleteAllItems();

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int  nr );

};


#endif // NCComboBox_h
