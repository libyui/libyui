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

   File:       NCTree.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTree_h
#define NCTree_h

#include <iosfwd>

#include <yui/YTree.h>
#include "NCPadWidget.h"
#include "NCTreePad.h"
#include "NCTablePad.h"

class NCTreeLine;


class NCTree : public YTree, public NCPadWidget
{
private:
    friend std::ostream & operator<<( std::ostream & str, const NCTree & obj );

    NCTree & operator=( const NCTree & );
    NCTree( const NCTree & );

    int idx;
    bool multiSel;

    void CreateTreeLines( NCTreeLine * p, NCTreePad * pad, YItem * item );

protected:

    virtual NCTreePad * myPad() const
    { return dynamic_cast<NCTreePad*>( NCPadWidget::myPad() ); }

    const NCTreeLine * getTreeLine( unsigned idx ) const;
    NCTreeLine *       modifyTreeLine( unsigned idx );

    virtual const char * location() const { return "NCTree"; }

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()	{ stopMultidraw(); }

public:

    NCTree( YWidget * parent, const std::string & label, bool multiselection=false, bool recursiveselection=false );
    virtual ~NCTree();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );
    virtual void rebuildTree();

    virtual YTreeItem * getCurrentItem() const;

    virtual YTreeItem * currentItem();

    virtual void deselectAllItems();

    virtual void selectItem( YItem *item, bool selected );
    virtual void selectItem( int index );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    void deleteAllItems();
};


#endif // NCTree_h
