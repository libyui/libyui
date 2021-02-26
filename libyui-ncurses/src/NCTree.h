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

   File:       NCTree.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTree_h
#define NCTree_h

#include <iosfwd>

#include "YTree.h"
#include "NCPadWidget.h"
#include "NCTreePad.h"
#include "NCTablePad.h"

class NCTreeLine;


class NCTree : public YTree, public NCPadWidget
{
private:
    friend std::ostream & operator<<( std::ostream & STREAM, const NCTree & OBJ );

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

    NCTree( YWidget * parent, const string & label, bool multiselection );
    virtual ~NCTree();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );
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
