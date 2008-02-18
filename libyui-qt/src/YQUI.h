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
#include <QMap>
#include <QTimer>
#include <QPalette>
#include <vector>

#include "YSimpleEventHandler.h"
#include <YUI.h>

#define YQWidgetMargin	4
#define YQWidgetSpacing	4
#define YQButtonBorder	3

class QY2Styler;
class QCursor;
class QFrame;
class QStackedWidget;
class QY2Settings;
class YEvent;
class YQOptionalWidgetFactory;
class YQWidgetFactory;
class YQApplication;
class YQUISignalReceiver;

using std::string;
using std::vector;


class YQUI: public YUI
{
    friend class YQUISignalReceiver;

public:

    /**
     * Constructor.
     **/
    YQUI( bool withThreads );

    /**
     * Destructor.
     **/
    virtual ~YQUI();

    /**
     * Access the global Qt-UI.
     **/
    static YQUI * ui() { return _ui; }

    /**
     * Post-constructor initialization. If running with threads, this has to be
     * called in the UI thread. Any subsequent calls will do nothing.
     **/
    void initUI();

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
	{ return _eventHandler.eventPendingFor( widget ); }

    /**
     * Returns the last event that isn't processed yet or 0 if there is none.
     *
     * The Qt UI keeps track of only one single (the last one) event.
     **/
    YEvent * pendingEvent() const { return _eventHandler.pendingEvent(); }

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
    bool fatalError() const { return _fatalError; }

    /**
     * Raise a fatal UI error. It will be delivered when it is safe to do so.
     * The caller should make sure it can continue for some time until the
     * error is delivered.
     **/
    void raiseFatalError() { _fatalError = true; }

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
    virtual YEvent * runPkgSelection( YWidget * packageSelector );

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
     * Block (or unblock) events. If events are blocked, any event sent
     * should be ignored until events are unblocked again.
     *
     * Reimplemented from YUI.
     **/
    virtual void blockEvents( bool block = true );

    /**
     * Returns 'true' if events are currently blocked.
     *
     * Reimplemented from YUI.
     **/
    virtual bool eventsBlocked() const;

    /**
     * Show mouse cursor indicating busy state.
     **/
    void busyCursor();

    /**
     * Show normal mouse cursor not indicating busy status.
     **/
    void normalCursor();

    /**
     * Open file selection box and let the user save y2logs to that location.
     * (Shift-F8)
     **/
    void askSaveLogs();

    /**
     * Open dialog to configure logging.
     * (Shift-F7)
     **/
    void askConfigureLogging();

    /**
     * Go into event loop until next user input is available.
     *
     * Reimplemented from YUI.
     **/
    YEvent * userInput( int timeout_millisec = 0 );

    /**
     * Check the event queue for user input. Don't wait.
     *
     * Reimplemented from YUI.
     **/
    YEvent * pollInput();

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


protected:

    /**
     * Application shutdown
     **/
    bool close();

    /**
     * Handle command line args
     **/
    void processCommandLineArgs( int argc, char **argv );

    /**
     * Calculate size of `opt(`defaultsize) dialogs
     **/
    void calcDefaultSize();

    /**
     * Idle around until fd_ycp is readable and handle repaints.
     * This is only used when a separate ui thread is running.
     *
     * Reimplemented from YUI.
     **/
    virtual void idleLoop( int fd_ycp );

    /**
     * Timeout during TimeoutUserInput() / WaitForEvent()
     **/
    void userInputTimeout();

    void leaveIdleLoop();


    //
    // Data members
    //

    static YQUI *	_ui;

    QMap<QString, int>	screenShotNo;
    QString		screenShotNameTemplate;

    bool _fullscreen;
    bool _noborder;

#if 0
    QWidget * _main_win;
#endif

    /**
     * Size for `opt(`defaultsize) dialogs.
     **/
    QSize _defaultSize;

    /**
     * This flag is set during userInput() in order to tell
     * returnNow() to call exit_loop(), which only may be called
     * after enter_loop().
     **/
    bool _do_exit_loop;

    bool _leave_idle_loop;

    /**
     * Event loop object. Required since a YaST2 UI needs to react to commands
     * from the YCP command stream as well as to X11 / Qt events.
     **/
    QEventLoop * _eventLoop;

    /**
     * Indicate a fatal error that requires the UI to terminate
     **/
    bool _fatalError;

    /**
     * Timer for TimeoutUserInput() / WaitForEvent().
     **/
    QTimer * _userInputTimer;

    /**
     * Timer for delayed busy cursor
     **/
    QTimer * _busyCursorTimer;

    /**
     * The handler for the single pending event this UI keeps track of
     **/
    YSimpleEventHandler _eventHandler;

    int blockedLevel;

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

    bool _uiInitialized;

    /*
     * Reads the style sheet, parses some comments and passes it to qapp
     */
    QY2Styler * _styler;

    YQUISignalReceiver * _signalReceiver;

    // Qt copies the _reference_ to argc, so we need to store argc forever
    int _ui_argc;
};


/**
 * Helper class that acts as a Qt signal receiver for YQUI.
 * YQUI itself cannot be a QObject to avoid problems with starting up the UI
 * with multiple threads.
 **/
class YQUISignalReceiver : public QObject
{
    Q_OBJECT

public:
    YQUISignalReceiver();

public slots:

    void slotBusyCursor();
    void slotUserInputTimeout();
    void slotLeaveIdleLoop();
};


/**
 * Create a new UI if there is none yet or return the existing one if there is.
 *
 * This is the UI plugin's interface to the outside world, so don't change the
 * name or signature! 
 **/
YUI * createUI( bool withThreads );


#endif // YQUI_h
