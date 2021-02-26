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

/-*/
#ifndef NCMultiSelectionBox_h
#define NCMultiSelectionBox_h

#include <iosfwd>

#include "YMultiSelectionBox.h"
#include "NCPadWidget.h"
#include "NCTablePad.h"

class NCMultiSelectionBox;
class NCTableTag;



class NCTableTag : public NCTableCol {

  private:

    YItem *yitem; 
    bool selected;

  public:

    NCTableTag( YItem *item, const bool sel = false )
      : NCTableCol( NCstring( "[ ]" ), SEPARATOR )
      , yitem (item)
      , selected( sel )
    {
        //store pointer to this tag in Yitem data
	yitem->setData( this );
    }
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
    YItem *origItem() { return yitem; }
};



class NCMultiSelectionBox : public YMultiSelectionBox, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiSelectionBox & OBJ );

  NCMultiSelectionBox & operator=( const NCMultiSelectionBox & );
  NCMultiSelectionBox            ( const NCMultiSelectionBox & );

  protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTablePad * myPad () const
        { return dynamic_cast<NCTablePad*> ( NCPadWidget::myPad () ); }

    NCTableTag * tagCell( int index );
    const NCTableTag * tagCell( int index ) const;

    bool isItemSelected( YItem *item );

    void toggleCurrentItem();

  public:

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()  { stopMultidraw(); }
    
    virtual const char * location() const { return "NCMultiSelectionBox"; }

    virtual void addItem( YItem * item );
    
    virtual void deleteAllItems();

    virtual void selectItem( YItem * item, bool selected );
    
    virtual void deselectAllItems();      

  protected:
    
    virtual NCPad * CreatePad();
    virtual void    wRecoded();

    
  public:

    NCMultiSelectionBox( YWidget * parent, const string & label );
    virtual ~NCMultiSelectionBox();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual YItem * currentItem();
    virtual void setCurrentItem( YItem * item );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    // added by gs
    unsigned int getNumLines( ) { return myPad()->Lines(); }
    const NCTableLine * getLine( const int & index ) { return myPad()->GetLine(index); }
    void clearItems() { return myPad()->ClearTable(); }
};


#endif // NCMultiSelectionBox_h
