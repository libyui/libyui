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

  Authors:	Mathias Kettner   <kettner@suse.de>,
		Stefan Hundhammer <sh@suse.de>

  Maintainer:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQUI_h
#define YQUI_h

#include <qapplication.h>
#include <qmap.h>
#include <qfont.h>
#include <qtimer.h>
#include <qtranslator.h>
#include <vector>

#include "YSimpleEventHandler.h"
#include <YUI.h>

#define YQWidgetMargin	4
#define YQWidgetSpacing	4
#define YQButtonBorder	3

class QVBox;
class QWidgetStack;
class QCursor;
class YEvent;
using std::string;
using std::vector;


class YQUI: public QObject, public YUI
{
    Q_OBJECT
public:

    /**
     * Constructor.
     */
    YQUI( int 		argc,
	  char **	argv,
	  bool 		with_threads,
	  const char *	macro_file );

    /**
     * Destructor.
     */
    ~YQUI();

    /**
     * Access the global Qt-UI.
     **/
    static YQUI * ui() { return _ui; }

    /**
     * Set the parent widget for embedding the UI into a KPart or KCMShell.
     * Used by kyast.
     **/
    static void setEmbeddingParent( QWidget * p );

    /**
     * Returns the UI's default font.
     **/
    const QFont & currentFont();

    /**
     * Returns the UI's heading font.
     **/
    const QFont & headingFont();

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
     * Returns 'false" if the "--no-wm" was specified on the command line, i.e.
     * we should assume no window manager is running.
     */
    bool haveWM() const { return _have_wm; }

    /**
     * Returns 'true' if defaultsize windows should use the full screen.
     **/
    bool fullscreen() const { return _fullscreen; }

    /**
     * Returns 'false' if toplevel (defaultsize) windows should not get window
     * manager decorations, i.e. "--noborder" was specified on the command
     * line.
     **/
    bool decorateToplevelWindow() const { return _decorate_toplevel_window; }

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
     */
    long defaultSize( YUIDimension dim ) const;

    /**
     * Make a screen shot in .png format and save it to 'filename'.
     * Opens a file selection box if 'filename' is empty.
     */
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
     */
    void toggleRecordMacro();

    /**
     * Open file selection box and ask for a macro file to play
     * (activated by Ctrl-Shift-Alt-P)
     */
    void askPlayMacro();

    /**
     * Issue an internal error: Open popup with that message and wait.
     *
     * Reimplemented from YUI.
     */
    void internalError( const char * msg );


    /**
     * Block WM_CLOSE events for the main window.
     **/
    void blockWmClose()		{ _wm_close_blocked = true;  }

    /**
     * Unblock WM_CLOSE events for the main window.
     **/
    void unblockWmClose()	{ _wm_close_blocked = false; }

    /**
     * Check if dialogs are to be activated automatically
     **/
    bool autoActivateDialogs() const { return _auto_activate_dialogs; }

    /**
     * Are we running embedded into another application, e.g., inside the KDE
     * control center?
     **/
    bool runningEmbedded() const;

    /**
     * Returns true if debugging embedding mode is on, i.e., when cmd line arg
     * --debug-embedding was given.
     **/
    bool debugEmbedding() const { return _debug_embedding; }

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
    QString YQUI::productName() const;


public slots:

    /**
     * Show hourglass cursor.
     *
     * Reimplemented from YUI.
     */
    void busyCursor();

    /**
     * Show pointer cursor.
     *
     * Reimplemented from YUI.
     */
    void normalCursor();

    /**
     * Fun stuff (release dependent)
     **/
    void easterEgg();


signals:
    /**
     * Emitted upon WM_CLOSE
     **/
    void wmClose();


protected:


    /**
     * Idle around until fd_ycp is readable and handle repaints.
     * This is only used when a separate ui thread is running.
     *
     * Reimplemented from YUI.
     */
    void idleLoop( int fd_ycp );

    /**
     * Return a representation for the glyph symbol specified in UTF-8 encoding
     * or an empty string to get a default textual representation.
     *
     * Reimplemented from YUI.
     */
    YCPString glyph( const YCPSymbol & glyphSymbol );

    /**
     * Go into event loop until next user input is available.
     *
     * Reimplemented from YUI.
     */
    YEvent * userInput( unsigned long timeout_millisec = 0 );

    /**
     * Check the event queue for user input. Don't wait.
     *
     * Reimplemented from YUI.
     */
    YEvent * pollInput();

    /**
     * Create a dialog.
     *
     * Reimplemented from YUI.
     */
    YDialog * createDialog( YWidgetOpt & opt );

    /**
     * Show and activate a dialog.
     *
     * Reimplemented from YUI.
     */
    void showDialog( YDialog * dialog );

    /**
     * Decativate and close a dialog. This does not delete the dialog yet.
     *
     * Reimplemented from YUI.
     */
    void closeDialog( YDialog * dialog );

    /**
     * Grab show events and work around QWidgetStack bug.
     *
     * Reimplemented from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    /**
     * Make all UI windows usable without a mouse - even predefined Qt dialogs
     * that don't know the UI's dialogs' activate() magic.
     *
     * Reimplemented from QObject.
     **/
    bool showEventFilter( QObject * obj, QEvent * ev );

    /**
     * Load translations for Qt's predefined dialogs like file selection box
     * etc.
     **/
    void loadPredefinedQtTranslations();

    /**
     * Determine good fonts based on defaultsize geometry and set
     * _auto_normal_font_size and _auto_heading_font_size accordingly.
     * Caches the values, so it's safe to call this repeatedly.
     **/
    void pickAutoFonts();

    /**
     * Returns 'true' if the UI  automatically picks fonts, disregarding Qt
     * standard settings.
     *
     * This makes sense in the installation system where
     * the display DPI cannot reliably be retrieved and thus Qt uses random
     * font sizes based on that random DPI.
     **/
    bool autoFonts() const { return _auto_fonts; }


    /*** Widget creation methods, all reimplemented from YUI ***/

    YContainerWidget * createAlignment		( YWidget * parent, YWidgetOpt & opt, YAlignmentType horAlign, YAlignmentType vertAlign );
    YContainerWidget * createFrame		( YWidget * parent, YWidgetOpt & opt, const YCPString & label );
    YContainerWidget * createRadioButtonGroup	( YWidget * parent, YWidgetOpt & opt);
    YContainerWidget * createReplacePoint	( YWidget * parent, YWidgetOpt & opt);
    YContainerWidget * createSplit		( YWidget * parent, YWidgetOpt & opt, YUIDimension dimension);
    YContainerWidget * createSquash		( YWidget * parent, YWidgetOpt & opt, bool horSquash, bool vertSquash);
    YContainerWidget * createWeight		( YWidget * parent, YWidgetOpt & opt, YUIDimension dim, long weight);

    YWidget * createCheckBox		( YWidget * parent, YWidgetOpt & opt, const YCPString & label, bool checked);
    YWidget * createComboBox		( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createDate		( YWidget * parent, YWidgetOpt & opt,const YCPString & label,  const YCPString & date);
    YWidget * createEmpty		( YWidget * parent, YWidgetOpt & opt);
    YWidget * createImage		( YWidget * parent, YWidgetOpt & opt, YCPByteblock imageData,	YCPString fallbackText );
    YWidget * createImage		( YWidget * parent, YWidgetOpt & opt, YCPString    fileName, 	YCPString fallbackText );
    YWidget * createIntField		( YWidget * parent, YWidgetOpt & opt, const YCPString & label, int minValue, int maxValue, int initialValue);
    YWidget * createLabel		( YWidget * parent, YWidgetOpt & opt, const YCPString & text);
    YWidget * createLogView		( YWidget * parent, YWidgetOpt & opt, const YCPString & label, int visibleLines, int maxLines );
    YWidget * createMultiLineEdit	( YWidget * parent, YWidgetOpt & opt, const YCPString & label, const YCPString & text);
    YWidget * createProgressBar		( YWidget * parent, YWidgetOpt & opt, const YCPString & label, const YCPInteger & maxProgress, const YCPInteger & currentProgress );
    YWidget * createPackageSelector	( YWidget * parent, YWidgetOpt & opt, const YCPString & floppyDevice );
    YWidget * createPushButton		( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createMenuButton		( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createRadioButton		( YWidget * parent, YWidgetOpt & opt, YRadioButtonGroup * rbg, const YCPString & label, bool checked);
    YWidget * createRichText		( YWidget * parent, YWidgetOpt & opt, const YCPString & text);
    YWidget * createSelectionBox	( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createMultiSelectionBox	( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createSpacing		( YWidget * parent, YWidgetOpt & opt, float size, bool horizontal, bool vertical);
    YWidget * createTable		( YWidget * parent, YWidgetOpt & opt, vector<string> header);
    YWidget * createTextEntry		( YWidget * parent, YWidgetOpt & opt, const YCPString & label, const YCPString & text);
    YWidget * createTime		( YWidget * parent, YWidgetOpt & opt,const YCPString & label,  const YCPString & time);
    YWidget * createTree		( YWidget * parent, YWidgetOpt & opt, const YCPString & label);
    YWidget * createPkgSpecial		( YWidget * parent, YWidgetOpt & opt, const YCPString & subwidget );


    /*** Widget creation methods for optional widgets, all reimplemented from YUI ***/

    bool 	hasBarGraph();
    YWidget *	createBarGraph		( YWidget * parent, YWidgetOpt & opt);


    bool 	hasColoredLabel();
    YWidget *	createColoredLabel	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YCPString 		label,
					  YColor 		foreground,
					  YColor 		background,
					  int 			margin );

    bool 	hasDownloadProgress();
    YWidget *	createDownloadProgress	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	filename,
					  int 			expectedSize );

    bool	hasDumbTab();
    YWidget *	createDumbTab		( YWidget *		parent,
					  YWidgetOpt & 		opt );

    bool	hasMultiProgressMeter();
    YWidget *	createMultiProgressMeter( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  bool 			horizontal,
					  const YCPList & 	maxValues );

    bool 	hasSlider();
    YWidget *	createSlider		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  int 			minValue,
					  int 			maxValue,
					  int 			initialValue );

    bool 	hasPartitionSplitter();
    YWidget *	createPartitionSplitter( YWidget *		parent,
					 YWidgetOpt &		opt,
					 int 			usedSize,
					 int 			totalFreeSize,
					 int 			newPartSize,
					 int 			minNewPartSize,
					 int 			minFreeSize,
					 const YCPString &	usedLabel,
					 const YCPString &	freeLabel,
					 const YCPString &	newPartLabel,
					 const YCPString &	freeFieldLabel,
					 const YCPString &	newPartFieldLabel );

    bool	hasWizard();
    YWidget * 	createWizard		( YWidget * parent, YWidgetOpt & opt,
					  const YCPValue & backButtonId,	const YCPString & backButtonLabel,
					  const YCPValue & abortButtonId,	const YCPString & abortButtonLabel,
					  const YCPValue & nextButtonId,	const YCPString & nextButtonLabel  );



    /*** END widget creation methods ***/


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
     * main widget
     */
    QWidget* mainWidget();

protected:

    /**
     * Sets the X input method according to the locale.
     * [Reimplemented from YUI]
     */
    YCPValue setLanguage( const YCPTerm & term );


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
     */
    void leaveIdleLoop( int );


protected:

    /**
     * Handle command line args
     **/
    void processCommandLineArgs( int argc, char **argv );


    //
    // Data members
    //

    /**
     * Assume presence of a window manager
     */
    bool _have_wm;

    /**
     * Use the entire available screen
     **/
    bool _fullscreen;

    /**
     * Decorate the toplevel window
     **/
    bool _decorate_toplevel_window;

    /**
     * Fake embedding mode (for layout debugging in embedding mode)
     **/
    bool _debug_embedding;

    /**
     * Container for the widget stack. QWidgetStack cannot handle a WFlags
     * argument, so this needs to be embedded into something else - and a QVBox
     * at least handles all the sizeHint and resize stuff.
     **/
    QVBox * _main_win;

    /**
     * Stack for the Qt widgets inside the main window.
     **/
    QWidgetStack * _widget_stack;

    /**
     * Stack to keep track of the stacking order of popup dialogs.
     **/
    vector<QWidget *> _popup_stack;

    /**
     * Numeric ID for defaultsize dialogs for the widget stack
     **/
    int _main_dialog_id;

    /**
     * Size for `opt(`defaultsize) dialogs.
     */
    QSize _default_size;

    /**
     * A flag used during the idle loop. If it is set to true,
     * the idle loop is left. This happens, if the ycp-ui-communication
     * pipe to the ui gets readable.
     */
    bool _leave_idle_loop;

    /**
     * This flag is set during @ref #userInput in order to tell
     * @ref #returnNow to call exit_loop, which only may be called
     * after enter_loop.
     */
    bool _do_exit_loop;

    /**
     * Default font (cached)
     **/
    QFont _current_font;
    bool _loaded_current_font;


    /**
     * Heading font (cached)
     **/
    QFont _heading_font;
    bool _loaded_heading_font;

    /**
     * For auto fonts
     **/
    bool _auto_fonts;
    int  _auto_normal_font_size;
    int  _auto_heading_font_size;

    /**
     * Window manager close events blocked?
     **/
    bool _wm_close_blocked;

    /**
     * Force new dialogs to the foreground and grab the keyboard focus?
     * (Only if running without a window manager)
     **/
    bool _auto_activate_dialogs;

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
     * Translator for the predefined Qt dialogs
     **/
    QTranslator _qtTranslations;


    /**
     * Parent widget for embedding in a KPart or KCMShell
     **/
    static QWidget * _embeddingParent;

    /**
     * Flag that indicates if we are running embedded
     **/
    bool _running_embedded;
};


#endif // YQUI_h
