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

   File:       NCTable.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCTable_h
#define NCTable_h

#include <iosfwd>

#include "YTable.h"

#include "NCPadWidget.h"
#include "NCTablePad.h"

class NCTable;


class NCTable : public YTable, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCTable & OBJ );

  NCTable & operator=( const NCTable & );
  NCTable            ( const NCTable & );

  private:

    vector<NCstring> _header;

  protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTablePad * myPad () const
        { return dynamic_cast<NCTablePad*> ( NCPadWidget::myPad () ); }
    bool          biglist;

  protected:

    virtual const char * location() const { return "NCTable"; }

    virtual NCPad * CreatePad();

    virtual void cellChanged( int index, int colnum, const string & newtext );
    virtual void cellChanged( const YTableCell *cell );
    

  protected:

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()  { stopMultidraw(); }

  public:

    NCTable( YWidget * parent, YTableHeader *tableHeader );

    virtual ~NCTable();

    bool bigList() const { return biglist; }

    void setHeader ( vector <string> head );
    virtual void setAlignment ( int col, YAlignmentType al );

    void setBigList( const bool big ) { biglist = big; }
    void SetSepChar( const chtype colSepchar )  { myPad()->SetSepChar( colSepchar ); }
    void SetSepWidth( const unsigned sepwidth ) { myPad()->SetSepWidth( sepwidth ); }
    void SetHotCol( const int hcol )            { myPad()->SetHotCol( hcol ); }

    virtual void addItem( YItem *yitem );
    virtual void deleteAllItems( );

    virtual int getCurrentItem();
    YItem * getCurrentItemPointer();
    
    virtual void setCurrentItem( int index );
    virtual void selectItem( YItem *yitem, bool selected );
    void selectCurrentItem(); 
    virtual void deselectAllItems();

    virtual int preferredWidth();
    virtual int preferredHeight();
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
    virtual void setLabel( const string & nlabel );
    virtual void setEnabled( bool do_bv );

    bool setItemByKey( int key );

    virtual NCursesEvent wHandleInput( wint_t key );

    //virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
    
    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }

    void stripHotkeys() { myPad()->stripHotkeys(); }
};


#endif // NCTable_h
