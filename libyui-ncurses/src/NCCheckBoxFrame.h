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

   File:       NCCheckBoxFrame.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCCheckBoxFrame_h
#define NCCheckBoxFrame_h

#include <iosfwd>

#include "YCheckBoxFrame.h"
#include "NCWidget.h"
#include "NCCheckBox.h"

class NCCheckBoxFrame;


class NCCheckBoxFrame : public YCheckBoxFrame, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBoxFrame & OBJ );

  NCCheckBoxFrame & operator=( const NCCheckBoxFrame & );
  NCCheckBoxFrame            ( const NCCheckBoxFrame & );

  private:

    NClabel label;
    bool isEnabled;
    
  protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCCheckBoxFrame"; }

    virtual void wRedraw();

  public:

    NCCheckBoxFrame( YWidget * parent, const string & label,
		     bool checked );
    virtual ~NCCheckBoxFrame();

    virtual long nicesize( YUIDimension dim );

    virtual int preferredWidth();
    virtual int preferredHeight();
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    //virtual void setEnabling( bool do_bv );
    virtual void setEnabled( bool do_bv );
    
    virtual bool getValue() { return isEnabled; }
   
    //Do not forget to call Redraw(), so that UI::ChangeWidget works
    //correctly - #301370 
    virtual void setValue( bool enable ) { isEnabled = enable; Redraw();}

    virtual bool setKeyboardFocus();

    virtual bool value();

    virtual NCursesEvent wHandleInput( wint_t key );
};


#endif // NCCheckBoxFrame_h
