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

   File:       NCWidget.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCWidget_h
#define NCWidget_h

#include <iosfwd>

#include "NCurses.h"
#include "tnode.h"
#include "YWidgetOpt.h"

class NCWidget;
class NCursesWindow;
class NClabel;

#define DLOC  location() << ' '

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCWidget
//
//	DESCRIPTION :
//
class NCWidget : public tnode<NCWidget*>, protected NCursesError {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCWidget & OBJ );
  friend std::ostream & operator<<( std::ostream & STREAM, const NCWidget * OBJ );

  NCWidget & operator=( const NCWidget & );
  NCWidget            ( const NCWidget & );

  protected:

    virtual const char * location() const { return "NCWidget"; }

  protected:

    virtual void PreDisconnect();
    virtual void PostDisconnect();
    virtual void PreReparent();
    virtual void PostReparent();

    NCursesWindow * ParentWin();

  protected:

    NCWidget *const grabedBy;
    bool grabFocus();
    virtual void grabNotify( NCWidget * ) {}
    virtual bool wantFocus( NCWidget & ngrab ) { return false; }

  public:
    void grabSet( NCWidget * ngrab ) {
      if ( grabedBy && grabedBy != ngrab )
	grabedBy->grabNotify( this );
      const_cast<NCWidget *&>( grabedBy ) = ngrab;
    }
    void grabRelease( NCWidget * ograb ) {
      if ( grabedBy && grabedBy != ograb )
	grabedBy->grabNotify( this );
      const_cast<NCWidget *&>( grabedBy ) = 0;
    }

  protected:

    NCursesWindow * win;
    wsze            defsze;
    wrect           framedim;
    wrect           inparent;
    bool            noUpdates;
    bool            skipNoDimWin;

    void wMoveChildTo( NCWidget & child, const wpos & newpos );
    void wRelocate( const wrect & newrect );
    void wRelocate( const wpos & newpos, const wsze & newsze ) {
      wRelocate( wrect( newpos, newsze ) );
    }

    virtual void wCreate( const wrect & newrect );
    virtual void wMoveTo( const wpos & newpos );
    virtual void wDelete();
    virtual void wUpdate( bool forced_br = false );

    wpos ScreenPos() const;

  protected:

    NC::WState wstate;

    virtual void wRedraw();
    virtual void wRecoded();

  protected:

    NClabel * hotlabel;
    int hotfkey;

    virtual void setFunctionHotkey( YWidgetOpt & opt );

  public:

    NCWidget( NCWidget * myparent = 0 );
    virtual ~NCWidget();

    virtual const NCstyle::Style & wStyle() const {
      if ( Parent() )
	return Top().Value()->wStyle();
      return NCurses::style()[NCstyle::DefaultStyle];
    }
    const NCstyle::StWidget & widgetStyle( bool nonactive = false ) const { return wStyle().getWidget( GetState(), nonactive ); }
    const NCstyle::StWidget & frameStyle()  const { return wStyle().getFrame( GetState() ); }
    const NCstyle::StList   & listStyle()   const { return wStyle().getList( GetState() ); }

    wsze wGetDefsze() const { return defsze; }

    void Update();
    void Redraw( const bool sub = false );
    void Recoded();

    NC::WState GetState() const { return wstate; }
    void       SetState( const NC::WState newstate, const bool force = false );

    /**
     * Pure virtual to make shure every widget implements it. Necessary to make
     * shure that UI requests via YWidget::setEnabling perform, and behave the
     * same way as direct calls to NCWidget::setEnabling.
     *
     * Defaut implementation is provided, and for most derived widgets it's ok
     * to simply use it.
     **/
    virtual void setEnabling( bool do_bv ) = 0;

    virtual bool HasHotkey( int key ) const;
    virtual bool HasFunctionHotkey( int key ) const;
    virtual int  GetFunctionHotkey() const;

    virtual NCursesEvent wHandleHotkey( int key );
    virtual NCursesEvent wHandleInput( int key );

    void DumpOn( std::ostream & str, string prfx ) const;
};

///////////////////////////////////////////////////////////////////

#include "NCstring.h"
#include "NCtext.h"

///////////////////////////////////////////////////////////////////

#endif // NCWidget_h
