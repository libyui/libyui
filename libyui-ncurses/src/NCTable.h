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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCTable_h
#define NCTable_h

#include <iosfwd>

#include "YTable.h"
#include "NCPadWidget.h"
#include "NCTablePad.h"

class NCTable;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCTable
//
//	DESCRIPTION :
//
class NCTable : public YTable, public NCPadWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCTable & OBJ );

  NCTable & operator=( const NCTable & );
  NCTable            ( const NCTable & );

  private:

    bool immediate;

  protected:

    NCTablePad *& pad;
    bool          biglist;

  protected:

    virtual const char * location() const { return "NCTable"; }

    virtual NCPad * CreatePad();

    virtual void itemAdded( vector<string> elements, int index );
    virtual void itemsCleared();
    virtual void cellChanged( int index, int colnum, const YCPString & newtext );
    
  protected:

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()  { stopMultidraw(); }

  public:

    NCTable( NCWidget * parent, YWidgetOpt & opt,
	     vector<string> header );
    virtual ~NCTable();

    bool bigList() const { return biglist; }
    
    bool isImmediate() const { return immediate; }	// addey by gs
    void setHeader( vector<NCstring> head ); 		// added by gs

    void setBigList( const bool big ) { biglist = big; }
    void SetSepChar( const chtype colSepchar )  { pad->SetSepChar( colSepchar ); }
    void SetSepWidht( const unsigned sepwidth ) { pad->SetSepWidht( sepwidth ); }
    void SetHotCol( const int hcol )            { pad->SetHotCol( hcol ); }

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual int  getCurrentItem();
    virtual void setCurrentItem( int index );

    bool setItemByKey( int key );

    virtual NCursesEvent wHandleInput( int key );

    virtual void setEnabling( bool enabled ) { Enable( enabled ); }

    virtual bool setKeyboardFocus() {
      if ( !grabFocus() )
        return YWidget::setKeyboardFocus();
      return true;
    }
};

///////////////////////////////////////////////////////////////////

#endif // NCTable_h
