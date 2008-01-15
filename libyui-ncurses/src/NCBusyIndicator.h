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

   File:       NCBusyIndicator.h

   Author:     NobodyCares
 
*/
#ifndef NCBusyIndicator_h
#define NCBusyIndicator_h

#include "YBusyIndicator.h"
#include "NCWidget.h"

class  NCBusyIndicator;

class NCBusyIndicator : public YBusyIndicator, public NCWidget {


  friend std::ostream & operator<<( std::ostream & STREAM, const NCBusyIndicator & OBJ );
 
  NCBusyIndicator & operator=( const NCBusyIndicator & );
  NCBusyIndicator            ( const NCBusyIndicator & );

  private:

    NClabel label;
    NCursesWindow * lwin // label window
    NCursesWindow * twin // text window

    bool alive;

    void setDefsze();
    void tUpdate();

  protected:

   virtual void wCreate( const wrect & newrect );
   virtual void wDelete();
   virtual void wRedraw();

  public:
  
    NCBusyIndicator ( YWidget * parent,
		      const string & label,
		      int timeout = 100 );
   
    virtual ~NCBusyIndicator();

    virtual int preferredWidth();
    virtual int preferredHeight();

#endif //NCBusyIndicator.h
