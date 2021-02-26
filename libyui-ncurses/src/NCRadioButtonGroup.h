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

   File:       NCRadioButtonGroup.h

   Author:     Michael Andres <ma@suse.de>

/-*/
#ifndef NCRadioButtonGroup_h
#define NCRadioButtonGroup_h

#include <iosfwd>

#include "YRadioButtonGroup.h"
#include "NCRadioButton.h"
#include "NCWidget.h"

class NCRadioButtonGroup;


class NCRadioButtonGroup : public YRadioButtonGroup, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCRadioButtonGroup & OBJ );

  NCRadioButtonGroup & operator=( const NCRadioButtonGroup & );
  NCRadioButtonGroup            ( const NCRadioButtonGroup & );

  private:
    int focusId;
    
  protected:

    virtual const char * location() const { return "NCRadioButtonGroup"; }

  public:

    NCRadioButtonGroup( YWidget * parent );
    virtual ~NCRadioButtonGroup();

    virtual int preferredWidth() { return YRadioButtonGroup::preferredWidth(); }
    virtual int preferredHeight() { return YRadioButtonGroup::preferredHeight(); }
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void addRadioButton(YRadioButton *button);
    virtual void removeRadioButton(YRadioButton *button);

    virtual void setEnabled( bool do_bv );
    
    void focusNextButton( );
    void focusPrevButton( );

};


#endif // NCRadioButtonGroup_h
