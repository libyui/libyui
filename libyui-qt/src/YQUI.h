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

  File:		YQUI.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQUI_h
#define YQUI_h

#include <qapplication.h>
#include <qmap.h>
#include <qtimer.h>
#include <vector>

#include "YSimpleEventHandler.h"
#include <YUI.h>

#define YQWidgetMargin	4
#define YQWidgetSpacing	4
#define YQButtonBorder	3

class QCursor;
class QVBox;
class QWidgetStack;
class QY2Settings;
class YEvent;
class YQOptionalWidgetFactory;
class YQPackageSelectorPlugin;
class YQWidgetFactory;
class YQApplication;

using std::string;
using std::vector;


class YQUI: public QObject, public YUI
{
    Q_OBJECT
public:

    /**
     * Constructor.
     **/
    YQUI( int 		argc,
	  char **	argv,
	  bool 		with_threads,
	  const char *	macro_file );

    /**
     * Destructor.
     **/
    ~YQUI();

    /**
     * Access the global Qt-UI.
     **/
    static YQUI * ui() { return _ui; }


protected:
    /**
     * Create the widget factory that provides all the createXY() methods for
     * standard (mandatory, i.e. non-optional) widgets.
     *
     * Reimplemented from YUI.
     **/
    virtual YWidgetFactory * createWidgetFactory();

    /**
     * Create the widget factory that provides all the createXY() methods for
     * optional ("special") widgets and the corresponding hasXYWidget()
     * methods.
     *
     * Reimplemented from YUI.
     **/
    virtual YOptionalWidgetFactory * createOptionalWidgetFactory();

    /*
     * Create the YApplication object that provides global methods.
     *
     * Reimplemented from YUI.
     **/
    virtual YApplication * createApplication();

public:

    /**
     * Return the global YApplication object as YQApplication.
     *
     * This will create the Y(Q)Application upon the first call and return a
     * pointer to the one and only (singleton) Y(Q)Application upon each
     * subsequent call.  This may throw exceptions if the Y(Q)Application
     * cannot be created.
     **/
    static YQApplication * yqApp();
    
    /**
     * Widget event handlers (slots) call this when an event occured that
     * should be the answer to a UserInput() / PollInput() (etc.) call.
     *
     * The UI assumes ownership of the event object that 'event' points to.
     * In particular, it takes care to delete that object.
     *
     * It is an error to pass 0 for 'event'.
     **/
    void sendEvent( YEvent * event );

    /**
     * Returns 'true' if there is any event pending for the specified widget.
     **/
    bool eventPendingFor( YWidget * widget ) const
	{ return _event_handler.eventPendingFor( widget ); }

    /**
     * Returns the last event that isn't processed yet or 0 if there is none.
     *
     * The Qt UI keeps track of only one single (the last one) event.
     **/
    YEvent * pendingEvent() const { return _event_handler.pendingEvent(); }

    /**
     * Return 'true' if defaultsize windows should use the full screen.
     **/
    bool fullscreen() const { return _fullscreen; }

    /**
     * Return 'true' if defaultsize windows should not get window manager
     * borders / frames. 
     **/
    bool noBorder() const { return _noborder; }
    /**
     * Returns 'true' if the UI had a fatal error that requires the application
     * to abort.
     **/
    bool fatalError() const { return _fatal_error; }

    /**
     * Raise a fatal UI error. It will be delivered when it is safe to do so.
     * The caller should make sure it can continue for some time until the
     * error is delivered.
     **/
    void raiseFatalError() { _fatal_error = true; }

    /**
     * Returns size for `opt(`defaultsize) dialogs (in one dimension).
     **/
    int defaultSize( YUIDimension dim ) const;

    /**
     * Make a screen shot in .png format and save it to 'filename'.
     * Opens a file selection box if 'filename' is empty.
     **/
    void makeScreenShot( string filename );

    /**
     * UI-specific runPkgSeleciton method: Start the package selection.
     * This implementation does the same as UserInput().
     *
     * Reimplemented from YUI.
     **/
    YCPValue runPkgSelection( YWidget * packageSelector );

    /**
     * Toggle macro recording (activated by Ctrl-Shift-Alt-M):
     * Stop macro recording if it is in progress,
     * open a file selection box and ask for a macro file name to save to and
     * start recording if no recording has been in progress.
     **/
    void toggleRecordMacro();

    /**
     * Open file selection box and ask for a macro file to play
     * (activated by Ctrl-Shift-Alt-P)
     **/
    void askPlayMacro();

    /**
     * Issue an internal error: Open popup with that message and wait.
     *
     * Reimplemented from YUI.
     **/
    void internalError( const char * msg );

    /**
     * Block (or unblock) events. If events are blocked, any event sent
     * should be ignored until events are unblocked again.
     *
     * Reimplemented from YUI.
     **/
    virtual void blockEvents( bool block = true )
	{ _event_handler.blockEvents( block ); }

    /**
     * Returns 'true' if events are currently blocked.
     *
     * Reimplemented from YUI.
     **/
    virtual bool eventsBlocked() const
	{ return _event_handler.eventsBlocked(); }

    /**
     * Returns the current product name
     * ("SuSE Linux", "SuSE Linux Enterprise Server", "United Linux", etc.)
     * as QString.
     **/
    QString productName() const;

    /**
     * Beep - activate the system (X11) bell.
     *
     * Reimplemented from YUI.
     **/
    void beep();


public slots:

    /**
     * Show hourglass cursor.
     *
     * Reimplemented from YUI.
     **/
    void busyCursor();

    /**
     * Show pointer cursor.
     *
     * Reimplemented from YUI.
     **/
    void normalCursor();

    /**
     * Open file selection box and let the user Save y2logs to that location.
     * (Shift-F8)
     **/
    void askSaveLogs();

    /**
     * Open dialog to configure logging.
     * (Shift-F7)
     **/
    void askConfigureLogging();

    /**
     * A mouse click with the wrong mouse button was detected - e.g., a right
     * click on a push button. The user might be left-handed, but his mouse
     * might not (yet) be configured for left-handed use - e.g., during
     * installation. Ask him if he would like his mouse temporarily configured
     * as a left-handed mouse.
     *
     * This status can be queried with UI::GetDisplayInfo() ("LeftHandedMouse").
     **/
    void maybeLeftHandedUser();


protected:

    /**
     * Idle around until fd_ycp is readable and handle repaints.
     * This is only used when a separate ui thread is running.
     *
     * Reimplemented from YUI.
     **/
    void idleLoop( int fd_ycp );

    /**
     * Return a representation for the glyph symbol specified in UTF-8 encoding
     * or an empty string to get a default textual representation.
     *
     * Reimplemented from YUI.
     **/
    YCPString glyph( const YCPSymbol & glyphSymbol );

    /**
     * Go into event loop until next user input is available.
     *
     * Reimplemented from YUI.
     **/
    YEvent * userInput( unsigned long timeout_millisec = 0 );

    /**
     * Check the event queue for user input. Don't wait.
     *
     * Reimplemented from YUI.
     **/
    YEvent * pollInput();

    /**
     * Show and activate a dialog.
     *
     * Reimplemented from YUI.
     **/
    void showDialog( YDialog * dialog );

    /**
     * Decativate and close a dialog. This does not delete the dialog yet.
     *
     * Reimplemented from YUI.
     **/
    void closeDialog( YDialog * dialog );


public:

    /**
     *
     * Open a directory selection box and prompt the user for an existing directory.
     * [Reimplemented from YUI]
     *
     * 'startDir' is the initial directory that is displayed.
     *
     * 'headline' is an explanatory text for the directory selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected directory name
     * or 'nil' (YCPVoid() ) if the user canceled the operation.
     **/
    YCPValue askForExistingDirectory ( const YCPString & startDir,
				       const YCPString & headline );

    /**
     * Open a file selection box and prompt the user for an existing file.
     * [Reimplemented from YUI]
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g. "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or 'nil' (YCPVoid() ) if the user canceled the operation.
     **/
    YCPValue askForExistingFile	( const YCPString & startWith,
				  const YCPString & filter,
				  const YCPString & headline );

    /**
     * Open a file selection box and prompt the user for a file to save data to.
     * Automatically asks for confirmation if the user selects an existing file.
     * [Reimplemented from YUI]
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g. "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or 'nil' (YCPVoid() ) if the user canceled the operation.
     **/
    YCPValue askForSaveFileName	( const YCPString & startWith,
				  const YCPString & filter,
				  const YCPString & headline );

    /**
     * Lower-level version that works with QStrings and does not change
     * the mouse cursor.
     **/
    QString askForSaveFileName( const QString & startWith,
				const QString & filter,
				const QString & headline );

    /**
     * Initialize and set a textdomain for gettext()
     **/
    static void setTextdomain( const char * domain );

    /**
     * Returns a high-contrast color palette suitable for vision impaired users.
     **/
    static QPalette visionImpairedPalette();

    /**
     * Returns the normal color palette
     **/
    QPalette normalPalette() const { return _normalPalette; }

    /**
     * Toggle between the vision impaired and the normal color palette.
     **/
    void toggleVisionImpairedPalette();

    /**
     * Returns 'true' if high-contrast colors for vision impaired users is in use.
     * This should be queried at other places before using custom colors.
     **/
    bool usingVisionImpairedPalette() const { return _usingVisionImpairedPalette; }

    /**
     * Convert logical layout spacing units into device dependent units.
     * A default size dialog is assumed to be 80x25 layout spacing units
     * and 640x480 device dependent spacing units.
     *
     * Reimplemented from YUI.
     **/
    virtual int deviceUnits( YUIDimension dim, float layout_units );

    /**
     * Convert device dependent units into logical layout spacing units.
     * A default size dialog is assumed to be 80x25 layout spacing units
     * and 640x480 device dependent spacing units.
     *
     * Reimplemented from YUI.
     **/
    virtual float layoutUnits( YUIDimension dim, int device_units );

    /**
     * Returns the package selector plugin singleton of this UI or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    YQPackageSelectorPlugin * packageSelectorPlugin();


protected:

    /**
     * Display capabilities.
     * [Reimplemented from YUI]
     * See UI builtin GetDisplayInfo() doc for details.
     **/
    int  getDisplayWidth();
    int  getDisplayHeight();
    int  getDisplayDepth();
    long getDisplayColors();
    int  getDefaultWidth();
    int  getDefaultHeight();
    bool textMode()	 		{ return false;	}
    bool hasImageSupport()		{ return true; 	}
    bool hasLocalImageSupport()		{ return true; 	}
    bool hasAnimationSupport()		{ return true; 	}
    bool hasIconSupport()		{ return false; }	// not yet
    bool hasFullUtf8Support()		{ return true; 	}
    bool richTextSupportsTable()	{ return true; }
    bool leftHandedMouse()		{ return _leftHandedMouse; }


    QMap<QString, int>	screenShotNo;
    QString		screenShotNameTemplate;


protected slots:

    /**
     * Application shutdown
     **/
    bool close();

    /**
     * Timeout during TimeoutUserInput() / WaitForEvent()
     **/
    void userInputTimeout();

    /**
     * Sets @ref #leave_idle_loop to true.
     **/
    void leaveIdleLoop( int );


protected:

    /**
     * Handle command line args
     **/
    void processCommandLineArgs( int argc, char **argv );

    /**
     * Calculate size of `opt(`defaultsize) dialogs
     **/
    void calcDefaultSize();


    //
    // Data members
    //

    /**
     * Use the entire available screen
     **/
    bool _fullscreen;

    /**
     * No window border for the main window
     **/
    bool _noborder;

    /**
     * Container for the widget stack. QWidgetStack cannot handle a WFlags
     * argument, so this needs to be embedded into something else - and a QVBox
     * at least handles all the sizeHint and resize stuff.
     **/
    QVBox * _main_win;

    /**
     * Size for `opt(`defaultsize) dialogs.
     **/
    QSize _default_size;

    /**
     * A flag used during the idle loop. If it is set to true,
     * the idle loop is left. This happens, if the ycp-ui-communication
     * pipe to the ui gets readable.
     **/
    bool _leave_idle_loop;

    /**
     * This flag is set during @ref #userInput in order to tell
     * @ref #returnNow to call exit_loop, which only may be called
     * after enter_loop.
     **/
    bool _do_exit_loop;

    /**
     * Global reference to the UI
     **/
    static YQUI * _ui;

    /**
     * Indicate a fatal error that requires the UI to terminate
     **/
    bool _fatal_error;

    /**
     * Timer for TimeoutUserInput() / WaitForEvent().
     **/
    QTimer _user_input_timer;

    /**
     * Timer for delayed busy cursor
     **/
    QTimer _busy_cursor_timer;

    /**
     * The handler for the single pending event this UI keeps track of
     **/
    YSimpleEventHandler _event_handler;

    /**
     * Saved normal palette
     **/
    QPalette _normalPalette;

    /**
     * Flag: currently using special palette for vision impaired users?
     **/
    bool _usingVisionImpairedPalette;

    /**
     * Flag: Does the user want to use a left-handed mouse?
     **/
    bool _leftHandedMouse;

    /**
     * Flag: Was the user already asked if he wants to use a left-handed mouse?
     **/
    bool _askedForLeftHandedMouse;
};


#endif // YQUI_h
