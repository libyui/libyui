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

   File:       NCProgressBar.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCProgressBar_h
#define NCProgressBar_h

#include <iosfwd>

#include "YProgressBar.h"
#include "NCWidget.h"

class NCProgressBar;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCProgressBar
//
//	DESCRIPTION :
//
class NCProgressBar : public YProgressBar, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCProgressBar & OBJ );

  NCProgressBar & operator=( const NCProgressBar & );
  NCProgressBar            ( const NCProgressBar & );

  private:

    NClabel  label;
    int      maxval;
    int      cval;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    void setDefsze();
    void tUpdate();

  protected:

    virtual const char * location() const { return "NCProgressBar"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

  public:

    NCProgressBar( NCWidget * parent, YWidgetOpt & opt,
		   const YCPString & label,
		   const YCPInteger & maxprogress,
		   const YCPInteger & progress );
    virtual ~NCProgressBar();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setProgress( const YCPInteger & nval );

    virtual void setEnabling( bool do_bv ) { NCWidget::setEnabling( enabled=do_bv ); }
};

///////////////////////////////////////////////////////////////////

#endif // NCProgressBar_h
