/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCTree.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCTree_h
#define NCTree_h

#include <iosfwd>

#include "YTree.h"
#include "NCPadWidget.h"
#include "NCTreePad.h"

class NCTreeLine;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTree
//
//	DESCRIPTION :
//
class NCTree : public YTree, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCTree & OBJ );

  NCTree & operator=( const NCTree & );
  NCTree            ( const NCTree & );

  protected:

    NCTreePad *& pad;

    const NCTreeLine * getTreeLine( unsigned idx ) const;
    NCTreeLine *       modifyTreeLine( unsigned idx );

  protected:

    virtual const char * location() const { return "NCTree"; }

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

  protected:

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()  { stopMultidraw(); }

  public:

    NCTree( NCWidget * parent, YWidgetOpt & opt,
	    const YCPString & label );
    virtual ~NCTree();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );
    virtual void rebuildTree();

    virtual const YTreeItem * getCurrentItem() const;
    virtual void setCurrentItem( YTreeItem * it );

    virtual NCursesEvent wHandleInput( int key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCTree_h
