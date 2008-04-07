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

   File:       NCTablePad.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCTablePad_h
#define NCTablePad_h

#include <iosfwd>

#include <vector>
using namespace std;

#include "NCTableItem.h"
#include "NCPad.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;


class NCTablePad : public NCPad {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCTablePad & OBJ );

  NCTablePad & operator=( const NCTablePad & );
  NCTablePad            ( const NCTablePad & );

  private:

    NCursesPad  Headpad;
    bool        dirtyHead;
    bool        dirtyFormat;

    NCTableStyle         ItemStyle;
    NCTableLine          Headline;
    vector<NCTableLine*> Items;
    wpos                 citem;

    void assertLine( unsigned idx );

  protected:

    void         DirtyFormat() { dirty = dirtyFormat = true; }
    virtual wsze UpdateFormat();

    virtual int  dirtyPad() { return setpos( CurPos() ); }
    virtual int  setpos( const wpos & newpos );
    virtual int  DoRedraw();
    virtual void updateScrollHint();

  public:

    NCTablePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTablePad();

  public:

    virtual void wRecoded();

    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

    bool setItemByKey( int key );

    wsze tableSize() { return dirtyFormat ? UpdateFormat()
					  : wsze( Lines(), ItemStyle.TableWidth() ); }
					  
    void setOrder( int column );

  public:

    bool SetHeadline( const vector<NCstring> & head );

    virtual void SendHead() {
      SetHead( Headpad, srect.Pos.C );
      dirtyHead = false;
    }

    void SetSepChar( const chtype colSepchar ) {
      ItemStyle.SetSepChar( colSepchar );
    }
    void SetSepWidth( const unsigned sepwidth ) {
      ItemStyle.SetSepWidth( sepwidth );
    }
    void SetHotCol( const int hcol ) {
      ItemStyle.SetHotCol( hcol );
    }

    unsigned Cols()  const { return ItemStyle.Cols(); }
    unsigned Lines() const { return Items.size(); }
    unsigned HotCol()const { return ItemStyle.HotCol(); }

    void     SetLines( unsigned idx );
    void     SetLines( vector<NCTableLine*> & nItems );
    void     ClearTable()  { SetLines( 0 ); }

    void Append( NCTableLine * item )           { AddLine( Lines(), item ); }
    void Append( vector<NCTableCol*> & nItems, int index = -1 ) 
    { 
	AddLine( Lines(), new NCTableLine( nItems, index ) ); 
    }
    void AddLine( unsigned idx, NCTableLine * item );
    void DelLine( unsigned idx );

    const NCTableLine * GetLine( unsigned idx ) const;
    NCTableLine *       ModifyLine( unsigned idx );
    
    void stripHotkeys();
};


#endif // NCTablePad_h
