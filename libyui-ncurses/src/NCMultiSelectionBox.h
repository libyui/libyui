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

   File:       NCMultiSelectionBox.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCMultiSelectionBox_h
#define NCMultiSelectionBox_h

#include <iosfwd>

#include <ycp/YCPVoid.h>
#include "YMultiSelectionBox.h"
#include "NCPadWidget.h"
#include "NCTablePad.h"

class NCMultiSelectionBox;
class NCTableTag;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTableTag
//
//	DESCRIPTION :
//
class NCTableTag : public NCTableCol {

  private:

    bool selected;

  public:

    NCTableTag( const bool sel = false )
      : NCTableCol( NCstring( "[ ]" ), SEPARATOR )
      , selected( sel )
    {}
    virtual ~NCTableTag() {}

    virtual void SetLabel( const NCstring & ) { /*NOOP*/; }

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const {
      NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );
      if ( selected ) {
	setBkgd( w, tableStyle, linestate, DATA );
	w.addch( at.Pos.L, at.Pos.C +1, 'x' );
      }
    }

    void SetSelected( const bool sel ) { selected = sel; }
    bool Selected() const              { return selected; }
};


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCMultiSelectionBox
//
//	DESCRIPTION :
//
class NCMultiSelectionBox : public YMultiSelectionBox, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiSelectionBox & OBJ );

  NCMultiSelectionBox & operator=( const NCMultiSelectionBox & );
  NCMultiSelectionBox            ( const NCMultiSelectionBox & );

  protected:

    NCTablePad *& pad;
    NCTableTag * tagCell( int index );
    const NCTableTag * tagCell( int index ) const;

    bool isItemSelected( int index ) const;
    bool setItemSelected( int index, bool val, bool update = true );
    void setAllItemsSelected( bool val );

    void toggleCurrentItem();

  protected:

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()  { stopMultidraw(); }

  protected:

    virtual const char * location() const { return "NCMultiSelectionBox"; }

    virtual NCPad * CreatePad();
    virtual void    wRecoded();

    virtual void itemAdded( const YCPString& string, bool selected );
    virtual void deleteAllItems();

    virtual bool itemIsSelected( int index ) { return isItemSelected( index ); }
    virtual void selectItem( int index )  { setItemSelected( index, true ); }
    virtual void deselectItem( int index ){ setItemSelected( index, false ); }
    virtual void selectAllItems()         { setAllItemsSelected( true ); }
    virtual void deselectAllItems()       { setAllItemsSelected( false ); }

  public:

    NCMultiSelectionBox( NCWidget * parent, const YWidgetOpt & opt,
			 const YCPString & label );
    virtual ~NCMultiSelectionBox();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual int getCurrentItem();
    virtual void setCurrentItem( int index );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    // added by gs
    unsigned int getNumLines( ) { return pad->Lines(); }
    const NCTableLine * getLine( const int & index ) { return pad->GetLine(index); }
    void clearItems() { return pad->ClearTable(); }
};

///////////////////////////////////////////////////////////////////

#endif // NCMultiSelectionBox_h
