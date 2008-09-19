/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       ncursesp.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef _NCURSESP_H
#define _NCURSESP_H

#include <iosfwd>

#include "ncursesw.h"
#include <ncursesw/etip.h>
#include <ncursesw/panel.h>


class NCursesPanel : public NCursesWindow
{
    friend std::ostream & operator<<( std::ostream & Stream, const NCursesPanel & Obj_Cv );
    friend std::ostream & operator<<( std::ostream & Stream, const NCursesPanel * Obj_Cv );

    friend class NCDialog;

protected:

    PANEL *p;
    static NCursesPanel *dummy;

private:
    /**
     * This structure is used for the panel's user data field to link the
     * PANEL* to the C++ object and to provide extra space for a user pointer.
    */

    typedef struct
    {
	/**
	 * the pointer for the user's data
	*/
	void *		    m_user;
	/**
	 * backward pointer to C++ object
	*/
	const NCursesPanel* m_back;
	/**
	 * the panel itself
	*/
	const PANEL*	    m_owner;
    } UserHook;

    /**
     * Initialize the panel object
    */
    void init();

protected:
    /**
     * Set the user pointer of the panel.
     */
    void set_user( void *user )
    {
	UserHook* uptr = ( UserHook* )::panel_userptr( p );
	assert( uptr && uptr->m_back == this && uptr->m_owner == p );
	uptr->m_user = user;
    }

    void *get_user() const
    {
	UserHook* uptr = ( UserHook* )::panel_userptr( p );
	assert( uptr && uptr->m_back == this && uptr->m_owner == p );
	return uptr->m_user;
    }

    static const NCursesPanel * get_Panel_of( const PANEL & pan )
    {
	UserHook* uptr = ( UserHook* )::panel_userptr( &pan );

	if ( uptr && uptr->m_owner == &pan
	     && uptr->m_back && uptr->m_back->p == &pan )
	{
	    return uptr->m_back;
	}

	return 0;
    }

    /**
     * If err is equal to the curses error indicator ERR, an error handler
     * is called.
     */
    void OnError( int err ) const THROWS( NCursesPanelException )
    {
	if ( err == ERR )
	    THROW( new NCursesPanelException( this, err ) );
    }

public:
    /**
     * Create a panel with this size starting at the requested position.
     */
    NCursesPanel( int lines,
		  int cols,
		  int begin_y = 0,
		  int begin_x = 0 )
	: NCursesWindow( lines, cols, begin_y, begin_x ), p(0)
    {
	init();
    }

    /**
     * This constructor creates the default Panel associated with the
     * ::stdscr window
     */
    NCursesPanel() : NCursesWindow( ::stdscr ), p(0) { init(); }

    virtual ~NCursesPanel();

    // basic manipulation

    /**
     * Resize the panel window.
     */
    virtual int resize( int lines, int columns )
    {
	::wresize( w, lines, columns );
	return ::replace_panel( p, w );
    }

    /**
     * Hide the panel. It stays in the stack but becomes invisible.
     */
    inline void hide()
    {
	// [ma] hiding a hiden one should not abort.
	if ( !hidden() )
	{
	    OnError( ::hide_panel( p ) );
	}
    }

    /**
     * Show the panel, i.e. make it visible.
     */
    inline void show()
    {
	OnError( ::show_panel( p ) );
    }

    /**
     * Make this panel the top panel in the stack.
     */
    inline void top()
    {
	OnError( ::top_panel( p ) );
    }

    /**
     * Make this panel the bottom panel in the stack.
     * N.B.: The panel associated with ::stdscr is always on the bottom. So
     * actually bottom() makes the panel the first above ::stdscr.
     */
    inline void bottom()
    {
	// warning FIX for broken bottom_panel (libpanel)
	// [ma] panel stack is messed up if the last panel is
	// moved to the bottom.
	if ( ::panel_above( 0 ) != p )
	{
	    OnError( ::bottom_panel( p ) );
	}
    }

    inline int mvwin( int y, int x )
    {
	OnError( ::move_panel( p, y, x ) );
	return OK;
    }

    /**
     * Return TRUE if the panel is hidden, FALSE otherwise.
    */
    inline bool hidden() const
    {
	return ( ::panel_hidden( p ) );
    }

    /**
     * The functions panel_above() and panel_below() are not reflected in
     * the NCursesPanel class. The reason for this is, that we cannot
     * assume that a panel retrieved by those operations is one wrapped
     * by a C++ class. Although this situation might be handled, we also
     * need a reverse mapping from PANEL to NCursesPanel which needs some
     * redesign of the low level stuff. At the moment, we define them in the
     * interface but they will always produce an error.
     */
    inline NCursesPanel& above() const
    {
	OnError( ERR );
	return *dummy;
    }

    inline NCursesPanel& below() const
    {
	OnError( ERR );
	return *dummy;
    }

    inline PANEL * PANEL_above() const
    {
	return( p ? ::panel_above( p ) : 0 );
    }

    inline PANEL * PANEL_below() const
    {
	return( p ? ::panel_below( p ) : 0 );
    }

    int transparent( int y, int x );

    // Those two are rewrites of the corresponding virtual members of NCursesWindow
    
    /**
     * Propagate all panel changes to the virtual screen and update the
     * physical screen.
     */
    virtual int refresh();

    /**
     * Propagate all panel changes to the virtual screen.
     */
    virtual int noutrefresh();

    /**
     * Redraw all panels.
     */
    static void redraw();

    // decorations
    /**
     * Put a frame around the panel and put the title centered in the top line
     * and btitle in the bottom line.
     */
    virtual void frame( const char* title = NULL,
			const char* btitle = NULL );

    /**
     * Same as frame(), but use highlighted attributes.
     */
    virtual void boldframe( const char* title = NULL,
			    const char* btitle = NULL );

    /**
     * Put the title centered in the top line and btitle in the bottom line.
     */
    virtual void label( const char* topLabel,
			const char* bottomLabel );

    /**
     * Put the label text centered in the specified row.
    */
    virtual void centertext( int row, const char* label );
};


/**
 * @short Associate user data with a panel.
 * We use templates to provide a typesafe mechanism to associate
 * user data with a panel. A NCursesUserPanel<T> is a panel
 * associated with some user data of type T.
 */
template<class T> class NCursesUserPanel : public NCursesPanel
{

public:
    /**
     * This creates an user panel of the requested size with associated
     * user data pointed to by p_UserData.
     */
    NCursesUserPanel( int lines,
		      int cols,
		      int begin_y = 0,
		      int begin_x = 0,
		      const T* p_UserData = ( T* )0 )
	: NCursesPanel( lines, cols, begin_y, begin_x )
    {
	if ( p )
	    set_user(( void * )p_UserData );
    };

    /**
     * This creates an user panel associated with the ::stdscr and user data
     * pointed to by p_UserData.
     */
    NCursesUserPanel( const T* p_UserData = ( T* )0 ) : NCursesPanel()
    {
	if ( p )
	    set_user(( void * )p_UserData );
    };

    virtual ~NCursesUserPanel() {};

    /**
     * Retrieve the user data associated with the panel.
     */
    T* UserData( void ) const
    {
	return ( T* )get_user();
    };

    /**
     * Associate the user panel with the user data pointed to by p_UserData.
     */
    virtual void setUserData( const T* p_UserData )
    {
	if ( p )
	    set_user(( void * )p_UserData );
    }

    /**
     * Retrieve the user data if associated with the PANEL.
     */
    static T* UserDataOf( const PANEL & pan )
    {
	const NCursesUserPanel<T> * p = dynamic_cast<const NCursesUserPanel<T>*>( get_Panel_of( pan ) );

	if ( p )
	{
	    return p->UserData();
	}

	return ( T* )0;
    };
};

#endif // _NCURSESP_H
