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

  File:	      YUIQt.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define USE_QT_CURSORS		1
#define FORCE_UNICODE_FONT	0

#include <rpc/types.h>		// MAXHOSTNAMELEN
#include <unistd.h>
#include <qsocketnotifier.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qvbox.h>
#include <qwidgetstack.h>
#include <qcursor.h>
#include <qwidgetlist.h>
#include <ycp/YCPTerm.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YUIQt.h"
#include "YUISymbols.h"

#include "utf8.h"
#include "YQAlignment.h"
#include "YQBarGraph.h"
#include "YQCheckBox.h"
#include "YQComboBox.h"
#include "YQColoredLabel.h"
#include "YQDialog.h"
#include "YQDownloadProgress.h"
#include "YQEmpty.h"
#include "YQFrame.h"
#include "YQImage.h"
#include "YQIntField.h"
#include "YQLabel.h"
#include "YQLogView.h"
#include "YQMenuButton.h"
#include "YQMultiLineEdit.h"
#include "YQMultiSelectionBox.h"
#include "YQPartitionSplitter.h"
#include "YQProgressBar.h"
#include "YQPackageSelector.h"
#include "YQPushButton.h"
#include "YQRadioButton.h"
#include "YQRadioButtonGroup.h"
#include "YQReplacePoint.h"
#include "YQRichText.h"
#include "YQSelectionBox.h"
#include "YQSlider.h"
#include "YQSpacing.h"
#include "YQSplit.h"
#include "YQSquash.h"
#include "YQTable.h"
#include "YQTextEntry.h"
#include "YQTree.h"
#include "YQi18n.h"
#include "QXEmbed.h"

#include <X11/Xlib.h>
#include <unistd.h>
#include <sys/stat.h>


#define DEFAULT_MACRO_FILE_NAME		"macro.ycp"

YUIQt * YUIQt::_yuiqt = 0;


YUIQt::YUIQt(int argc, char **argv, bool with_threads, Y2Component *callback)
    : QApplication(argc, argv)
    , YUIInterpreter(with_threads, callback)
    , main_dialog_id(0)
    , event_widget(0)
    , event_type(ET_NONE)
    , do_exit_loop(false)
    , loaded_current_font(false)
    , loaded_heading_font(false)
    , wm_close_blocked(false)
    , auto_activate_dialogs(true)
    , running_embedded(false)
{
    _yuiqt 			= this;
    _fatal_error		= false;
    _have_wm			= true;
    _fullscreen			= false;
    _decorate_toplevel_window	= true;
    suseheaderID 		= -1;
    screenShotNameTemplate 	= "";

    if ( argv )
    {
	for( int i=0; i < argc; i++ )
	{
	    QString opt = argv[i];

	    // Normalize command line option - accept "--xy" as well as "-xy"

	    if ( opt.startsWith( "--" ) )
		opt.remove(0, 1);

	    if      ( opt == QString( "-no-wm"	 	) )	_have_wm 			= false;
	    else if ( opt == QString( "-fullscreen"	) )	_fullscreen 			= true;
	    else if ( opt == QString( "-noborder" 	) )	_decorate_toplevel_window	= false;
	    else if ( opt == QString( "-kcontrol_id"	) )
	    {
		if ( i >= argc )
		{
		    y2error( "Missing arg for '--kcontrol_id'" );
		}
		else
		{
		    kcontrol_id = argv[++i];
		    y2milestone( "Starting with kcontrol_id='%s'",
				 (const char *) kcontrol_id );
		}
	    }
	    else if ( opt == QString( "-help"  ) )
	    {
		fprintf( stderr,
			 "Command line options for the YaST2 Qt UI:\n"
			 "\n"
			 "--nothreads   run without additional UI threads\n"
			 "--no-wm       assume no window manager is running\n"
			 "--fullscreen  use full screen for `opt(`defaultsize) dialogs\n"
			 "--noborder    no window manager border for `opt(`defaultsize) dialogs\n"
			 "--help        this help text\n"
			 "\n"
			 "--kcontrol_id <ID-String>   set KDE control center identification\n"
			 "\n"
			 "-no-wm, -noborder etc. are accepted as well as --no-wm, --noborder\n"
			 "to maintain backwards compatibility.\n"
			 "\n"
			 );

		raiseFatalError();
	    }
	}
    }


    if ( _fullscreen )
    {
	default_size.setWidth ( desktop()->width()  );
	default_size.setHeight( desktop()->height() );
	y2milestone( "-fullscreen: using %dx%d for `opt(`defaultsize)",
		     default_size.width(), default_size.height() );
    }
    else if ( _have_wm )
    {
	// Get default_size via -geometry command line option

	QWidget *dummy = new QWidget();
	dummy->hide();
	setMainWidget(dummy);
	default_size = dummy->size();


        // Set min defaultsize

	if ( default_size.width()  < 640 ||
	     default_size.height() < 480   )
	{
	    // 640x480 is the absolute minimum, but let's go for 800x600 if we can

	    if ( desktop()->width()  >= 800 &&
		 desktop()->height() >= 600  )
	    {
		default_size.setWidth ( 800 );
		default_size.setHeight( 600 );
	    }
	    else
	    {
		default_size.setWidth ( 640 );
		default_size.setHeight( 480 );
	    }

	    y2debug( "Assuming default size of %dx%d",
		     default_size.width(), default_size.height() );
	}
    }
    else	// ! _have_wm
    {
	default_size.setWidth ( desktop()->width()  );
	default_size.setHeight( desktop()->height() );
    }


    // Create main window for `opt(`defaultsize) dialogs.
    //
    // We have to use something else than QWidgetStack since QWidgetStack
    // doesn't accept a WFlags arg which we badly need here.

    WFlags wflags = WType_TopLevel;

    if ( ! _decorate_toplevel_window )
    {
	y2debug( "Suppressing WM decorations for toplevel window" );
	wflags |= WStyle_Customize | WStyle_NoBorder;
    }

    main_win = new QVBox( 0, 0, wflags ); // parent, name, wflags


    // Create widget stack for `opt(`defaultsize) dialogs

    widget_stack = new QWidgetStack( main_win );
    widget_stack->setFocusPolicy( QWidget::StrongFocus );
    setMainWidget( main_win );
    qApp->installEventFilter( this );
    main_win->installEventFilter( this );
    main_win->resize( default_size );

    if ( _fullscreen || ! _have_wm )
	main_win->move( 0, 0 );

    busy_cursor = new QCursor( WaitCursor );


    // Set window title

    if ( kcontrol_id.isEmpty() )
    {
	QString title( "YaST2" );
	char hostname[ MAXHOSTNAMELEN+1 ];
	if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
	{
	    hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated

	    if ( strlen( hostname ) > 0 &&
		 strcmp( hostname, "(none)" ) != 0 &&
		 strcmp( hostname, "linux"  ) != 0 )
	    {
		title += "@";
		title += hostname;
	    }
	}
	main_win->setCaption( title );
	kcontrol_id = title;
    }
    else // --kcontrol_id in command line
    {
	running_embedded = true;
	main_win->setCaption( kcontrol_id );
    }


    // Hide the main window unless we are running embedded. The first call to
    // UI::OpenDialog() on an `opt(`defaultSize) dialog will trigger a
    // showDialog() call that shows the main window - there is nothing to
    // display yet.
    //
    // In embedded mode, keep the main window open so the embedding application
    // (kcontrol) catches the main window as YaST2's first window and not some
    // popup window that may appear before this. An empty grey area for the
    // main window (that will appear for a while) is a lot better than a
    // "please wait" popup zoomed to near full screen that may be embedded -
    // with a large main window that opens somewhere else on the screen.

    if ( ! running_embedded )
	main_win->hide();
    else
    {
	main_win->show();
	y2milestone( "Running in embedded mode - leaving main window open" );
    }
    

    //  Init other stuff

    setFont( currentFont() );
    QXEmbed::initialize();
    busyCursor();
    topmostConstructorHasFinished();
#if defined(QT_THREAD_SUPPORT)
    qApp->unlock ();
#endif
}


YUIQt::~YUIQt()
{
    y2debug("Closing down Qt UI.");

    normalCursor();

    if ( busy_cursor )
	delete busy_cursor;
}


void YUIQt::internalError( const char *msg )
{
    normalCursor();
    int button = QMessageBox::critical( 0, "YaST2 Internal Error", msg,
					QMessageBox::Abort | QMessageBox::Default,
					0 ); // button1
    busyCursor();

    if ( button == QMessageBox::Abort )
    {
	raiseFatalError();
	abort();

	// exit() leaves a process running (WFM?), so this really seems to be
	// the only way to make sure we are really going down.
    }
}


void YUIQt::idleLoop(int fd_ycp)
{
    leave_idle_loop = false;

    // process Qt events until fd_ycp is readable.
    QSocketNotifier *notifier = new QSocketNotifier(fd_ycp, QSocketNotifier::Read);
    QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(leaveIdleLoop(int)));
    notifier->setEnabled(true);

    while (!leave_idle_loop)
	processOneEvent ();

    delete notifier;
}


YWidget *YUIQt::userInput(YDialog *dialog, EventType *event)
{
    if (event_type == ET_NONE)
    {
	YQDialog *qd = (YQDialog *)dialog;
	qd->activate(true);

	if ( qApp->focusWidget() )
	{
	    qApp->focusWidget()->setFocus();
	}
	normalCursor();
	do_exit_loop = true; // in returnNow exit_loop() should be called.

	while (event_type == ET_NONE)
	{
	    enter_loop();
	}

	do_exit_loop = false;
	busyCursor();
	qd->activate(false);
    }
    
    *event = event_type;
    YWidget * ret = event_widget;
    
    // Clear for next time
    event_type	 = ET_NONE;
    event_widget = 0;
    
    return ret;
}


YWidget *YUIQt::pollInput(YDialog *dialog, EventType *event)
{
    if (event_type == ET_NONE)
    {
	// if ( focusWidget() ) focusWidget()->repaint();
	YQDialog *qd = (YQDialog *)dialog;
	qd->activate(true);
	processEvents();
	qd->activate(false);
    }
    *event = event_type;
    event_type = ET_NONE; // Clear for next time
    return event_widget;
}


YCPValue YUIQt::runPkgSelection( YWidget * packageSelector )
{
    y2milestone( "Running package selection..." );
    wm_close_blocked		= true;
    auto_activate_dialogs	= false;

    YCPValue input = evaluateUserInput( YCPTerm( YCPSymbol( "", false ) ), false );

    auto_activate_dialogs	= true;
    wm_close_blocked 		= false;
    y2milestone( "Package selection done - returning %s", input->toString().c_str() );

    return input;
}


YDialog *YUIQt::createDialog(YWidgetOpt & opt)
{
    bool has_defaultsize = opt.hasDefaultSize.value();
    QWidget *qt_parent = main_win;


    // Popup dialogs get the topmost other popup dialog as their parent since
    // some window managers (e.g., fvwm2 as used in the inst-sys) otherwise
    // tend to confuse the stacking order of popup dialogs.
    //
    // This popup_stack handling would be better placed in showDialog(), but we
    // need the parent here for QWidget creation. libyui guarantees that each
    // createDialog() will be followed by showDialog() for the same dialog
    // without any chance for other dialogs to get in between.

    if ( ! has_defaultsize && ! popup_stack.empty() )
	qt_parent = popup_stack.back();

    YQDialog *dialog = new YQDialog( this, opt, qt_parent, has_defaultsize );
    CHECK_PTR( dialog );

    if ( ! has_defaultsize )
	popup_stack.push_back( (QWidget *) dialog->widgetRep() );

    return dialog;
}


void YUIQt::showDialog( YDialog *dialog )
{
    QWidget *qw = (QWidget *) dialog->widgetRep();

    if ( ! qw )
    {
	y2error( "No widgetRep() for dialog" );
	return;
    }

    if ( dialog->hasDefaultSize() )
    {
	widget_stack->addWidget  ( qw, ++main_dialog_id );
	widget_stack->raiseWidget( qw ); // maybe this is not necessary (?)

	if ( ! main_win->isVisible() )
	{
	    // y2milestone( "Showing main window" );
	    main_win->resize( default_size );

	    if ( ! _have_wm )
		main_win->move( 0, 0 );

	    main_win->show();
	    qw->setFocus();
	}
    }
    else	// non-defaultsize dialog
    {
	qw->setCaption( kcontrol_id );
	qw->show();
    }

    ( (YQDialog *) dialog)->ensureOnlyOneDefaultButton();
    processEvents();
}


void YUIQt::closeDialog( YDialog *dialog )
{
    QWidget *qw = (QWidget *) dialog->widgetRep();

    if ( ! qw )
    {
	y2error( "No widgetRep() for dialog" );
	return;
    }

    if ( dialog->hasDefaultSize() )
    {
	widget_stack->removeWidget( qw );

	if ( --main_dialog_id < 1 )	// nothing left on the stack
	{
	    if ( ! running_embedded )
	    {
		// y2milestone( "Hiding main window" );
		main_win->hide();
	    }
	    else
	    {
		y2milestone( "Running embedded - keeping (empty) main window open" );
	    }
	    
	    main_dialog_id = 0;	// this should not be necessary - but better be safe than sorry
	}
	else
	{
	    widget_stack->raiseWidget( main_dialog_id );
	}
    }
    else	// non-defaultsize dialog
    {
	qw->hide();

	// Clean up the popup stack. libyui guarantees that a dialog will be
	// deleted after closeDialog() so it is safe to pop that dialog from
	// the popup stack here.

	if ( ! popup_stack.empty() && popup_stack.back() == qw )
	    popup_stack.pop_back();
	else
	    y2error( "Popup dialog stack corrupted!" );
    }
}


YContainerWidget *YUIQt::createReplacePoint(YWidget *parent, YWidgetOpt & opt)
{
    return new YQReplacePoint(this, ((QWidget *)parent->widgetRep()), opt);
}

YWidget *YUIQt::createEmpty(class YWidget *parent, YWidgetOpt & opt)
{
    return new YQEmpty(this, ((QWidget *)parent->widgetRep()), opt);
}

YWidget *YUIQt::createSpacing(YWidget *parent, YWidgetOpt & opt, float size, bool horizontal, bool vertical)
{
    return new YQSpacing(this, ((QWidget *)parent->widgetRep()), opt, size, horizontal, vertical);
}

YContainerWidget *YUIQt::createFrame(class YWidget *parent, YWidgetOpt & opt, const YCPString & label )
{
    return new YQFrame ( this, ( (QWidget *) parent->widgetRep()), opt, label );
}

YContainerWidget *YUIQt::createSplit(YWidget *parent, YWidgetOpt & opt, YUIDimension dimension)
{
    return new YQSplit(((QWidget *)parent->widgetRep()), opt, dimension);
}

YContainerWidget *YUIQt::createAlignment(YWidget *parent, YWidgetOpt & opt,
					 YAlignmentType halign,
					 YAlignmentType valign)
{
    return new YQAlignment(this, ((QWidget *)parent->widgetRep()), opt, halign, valign);
}

YContainerWidget *YUIQt::createSquash(YWidget *parent, YWidgetOpt & opt, bool hsquash, bool vsquash)
{
    return new YQSquash(this, ((QWidget *)parent->widgetRep()), opt, hsquash, vsquash);
}

YWidget *YUIQt::createLabel(YWidget *parent, YWidgetOpt & opt, const YCPString & text)
{
    return new YQLabel(this, (QWidget *)(parent->widgetRep()), opt, text);
}

YWidget *YUIQt::createLogView(YWidget *parent, YWidgetOpt & opt,
			      const YCPString & label, int visibleLines, int maxLines )
{
    return new YQLogView(this, (QWidget *)(parent->widgetRep()), opt, label, visibleLines, maxLines);
}

YWidget *YUIQt::createRichText(YWidget *parent, YWidgetOpt & opt, const YCPString & text)
{
    return new YQRichText(this, (QWidget *)(parent->widgetRep()), opt, text);
}

YWidget *YUIQt::createPackageSelector(YWidget *parent, YWidgetOpt & opt, const YCPString & floppyDevice )
{
    auto_activate_dialogs = false;
    return new YQPackageSelector(this, (QWidget *)(parent->widgetRep()), opt, floppyDevice );
}

YWidget *YUIQt::createPkgSpecial( YWidget *parent, YWidgetOpt & opt, const YCPString &subwidget )
{
    y2error( "The Qt UI does not support PkgSpecial subwidgets!" );
    return 0;
}

YWidget *YUIQt::createPushButton(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    YQDialog * dialog = dynamic_cast<YQDialog *> ( parent->yDialog() );
    return new YQPushButton(this, (QWidget *)(parent->widgetRep()), dialog, opt, label);
}

YWidget *YUIQt::createMenuButton(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    return new YQMenuButton(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createCheckBox(YWidget *parent, YWidgetOpt & opt, const YCPString & label, bool checked)
{
    return new YQCheckBox(this, (QWidget *)(parent->widgetRep()), opt, label, checked);
}

YWidget *YUIQt::createRadioButton(YWidget *parent, YWidgetOpt & opt, YRadioButtonGroup *rbg,
				  const YCPString & label, bool checked)
{
    return new YQRadioButton(this, (QWidget *)(parent->widgetRep()), opt, rbg, label, checked);
}

YContainerWidget *YUIQt::createRadioButtonGroup(YWidget *parent, YWidgetOpt & opt)
{
    return new YQRadioButtonGroup(this, (QWidget *)(parent->widgetRep()), opt);
}

YWidget *YUIQt::createTextEntry(YWidget *parent, YWidgetOpt & opt, const YCPString & label, const YCPString & text)
{
    return new YQTextEntry(this, (QWidget *)(parent->widgetRep()), opt, label, text);
}

YWidget *YUIQt::createMultiLineEdit(YWidget *parent, YWidgetOpt & opt, const YCPString & label, const YCPString & initialText)
{
    return new YQMultiLineEdit(this, (QWidget *)(parent->widgetRep()), opt, label, initialText);
}

YWidget *YUIQt::createSelectionBox(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    return new YQSelectionBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createMultiSelectionBox(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    return new YQMultiSelectionBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createComboBox(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    return new YQComboBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createTree(YWidget *parent, YWidgetOpt & opt, const YCPString & label)
{
    return new YQTree(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createTable(YWidget *parent, YWidgetOpt & opt, vector<string> header)
{
    return new YQTable(this, (QWidget *)(parent->widgetRep()), opt, header);
}

YWidget *YUIQt::createProgressBar(YWidget *parent, YWidgetOpt & opt, const YCPString & label,
				  const YCPInteger & maxProgress, const YCPInteger & progress)
{
    return new YQProgressBar(this, (QWidget *)(parent->widgetRep()), opt, label, maxProgress, progress);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt & opt, YCPByteblock imagedata, YCPString default_text)
{
    return new YQImage(this, (QWidget *)(parent->widgetRep()), opt, imagedata);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt & opt, YCPString file_name, YCPString default_text)
{
    return new YQImage(this, (QWidget *)(parent->widgetRep()), opt, file_name);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt & opt, ImageType img, YCPString default_text)
{
    return new YQImage(this, (QWidget *)(parent->widgetRep()), opt, img);
}

YWidget *YUIQt::createIntField( YWidget *		parent,
				YWidgetOpt &		opt,
				const YCPString &	label,
				int 			minValue,
				int 			maxValue,
				int 			initialValue )
{
    return new YQIntField( this,
			   (QWidget *) ( parent->widgetRep() ),
			   opt,
			   label,
			   minValue,
			   maxValue,
			   initialValue );
}


bool YUIQt::hasBarGraph()
{
    return true;
}

YWidget *YUIQt::createBarGraph(YWidget *parent, YWidgetOpt & opt)
{
    return new YQBarGraph(this, (QWidget *)(parent->widgetRep()), opt);
}


bool YUIQt::hasColoredLabel()
{
    return QColor::numBitPlanes() >= 15;
}

YWidget *YUIQt::createColoredLabel(YWidget *parent, YWidgetOpt & opt,
				   YCPString label,
				   YColor foreground, YColor background,
				   int margin )
{
    if ( QColor::numBitPlanes() < 15 )
    {
	y2error( "Display depth is %d, need at least 15 for ColoredLabel",
		 QColor::numBitPlanes() );

	return 0;
    }


    return new YQColoredLabel(this, (QWidget *)(parent->widgetRep()), opt,
			      label, foreground, background, margin );
}


bool YUIQt::hasDownloadProgress()
{
    return true;
}

YWidget *YUIQt::createDownloadProgress	( YWidget *parent,
					  YWidgetOpt & opt,
					  const YCPString & label,
					  const YCPString & filename,
					  int expectedSize)
{
    return new YQDownloadProgress( this,
				   (QWidget *) ( parent->widgetRep() ),
				   opt,
				   label,
				   filename,
				   expectedSize );
}


bool YUIQt::hasSlider()
{
    return true;
}


YWidget *YUIQt::createSlider( YWidget *		parent,
			      YWidgetOpt &	opt,
			      const YCPString &	label,
			      int 		minValue,
			      int 		maxValue,
			      int 		initialValue )
{
    return new YQSlider( this,
			 (QWidget *) ( parent->widgetRep() ),
			 opt,
			 label,
			 minValue,
			 maxValue,
			 initialValue );
}


bool YUIQt::hasPartitionSplitter()
{
    return true;
}

YWidget *YUIQt::createPartitionSplitter( YWidget *		parent,
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
					 const YCPString &	newPartFieldLabel )
{
    return new YQPartitionSplitter( this,
				    (QWidget *) ( parent->widgetRep() ),
				    opt,
				    usedSize,
				    totalFreeSize,
				    newPartSize,
				    minNewPartSize,
				    minFreeSize,
				    usedLabel,
				    freeLabel,
				    newPartLabel,
				    freeFieldLabel,
				    newPartFieldLabel );
}


YCPValue YUIQt::setLanguage( const YCPTerm & term)
{
    return YCPVoid();	// OK (YCPNull() would mean error)
}


void
YUIQt::busyCursor ( void )
{
#if USE_QT_CURSORS

    setOverrideCursor( waitCursor );

#else
    /**
     * There were versions of Qt where simply using
     * QApplication::setOverrideCursor( waitCursor ) didn't work any more:
     * We _need_ the WType_Modal flag for non-defaultsize dialogs (i.e. for
     * popups), but Qt unfortunately didn't let such dialogs have a clock
     * cursor.  :-(
     *
     * They might have their good reasons for this (whatever they are), so
     * let's simply make our own busy cursors and set them to all widgets
     * created thus far.
     **/

    QWidgetList *widget_list = allWidgets();
    QWidgetListIt it( *widget_list );

    while ( *it )
    {
	if ( ! (*it)->testWFlags( WType_Desktop ) )	// except desktop (root window)
	{
	    XDefineCursor( (*it)->x11Display(), (*it)->winId(), busy_cursor->handle() );
	}
	++it;
    }

    if ( widget_list )
	delete widget_list;
#endif
}


void
YUIQt::normalCursor ( void )
{
#if USE_QT_CURSORS

    while ( overrideCursor() )
	restoreOverrideCursor();
#else
    /**
     * Restore the normal cursor for all widgets (undo busyCursor() ).
     *
     * Fortunately enough, Qt widgets keep track of their normal cursor
     * (QWidget::cursor()) so this can easily be restored - it's not always the
     * arrow cursor - e.g., input fields (QLineEdit) have the "I-beam" cursor.
     **/

    QWidgetList *widget_list = allWidgets();
    QWidgetListIt it( *widget_list );

    while ( *it )
    {
	if ( ! (*it)->testWFlags( WType_Desktop ) )	// except desktop (root window)
	{
	    XDefineCursor( (*it)->x11Display(), (*it)->winId(), (*it)->cursor().handle() );
	}
	++it;
    }

    if ( widget_list )
	delete widget_list;
#endif
}


YCPString
YUIQt::glyph( const YCPSymbol & glyphSymbol )
{
    string sym = glyphSymbol->symbol();
    QChar unicodeChar;

    // Hint: Use 'xfd' to view characters available in the Unicode font.

    if      ( sym == YUIGlyph_ArrowLeft		)	unicodeChar = QChar( 0x2190 );
    else if ( sym == YUIGlyph_ArrowRight	)	unicodeChar = QChar( 0x2192 );
    else if ( sym == YUIGlyph_ArrowUp		)	unicodeChar = QChar( 0x2191 );
    else if ( sym == YUIGlyph_ArrowDown		)	unicodeChar = QChar( 0x2193 );
    else if ( sym == YUIGlyph_CheckMark		)	unicodeChar = QChar( 0x2714 );
    else if ( sym == YUIGlyph_BulletArrowRight	)	unicodeChar = QChar( 0x279c );
    else if ( sym == YUIGlyph_BulletCircle	)	unicodeChar = QChar( 0x274d );
    else if ( sym == YUIGlyph_BulletSquare	)	unicodeChar = QChar( 0x274f );
    else return YCPString( "" );

    QString qstr( unicodeChar );

    return YCPString( toUTF8( qstr ) );
}


int YUIQt::getDisplayWidth()
{
    return desktop()->width();
}


int YUIQt::getDisplayHeight()
{
    return desktop()->height();
}


int YUIQt::getDisplayDepth()
{
    return QColor::numBitPlanes();
}


long YUIQt::getDisplayColors()
{
    return 1L << QColor::numBitPlanes();
}


int YUIQt::getDefaultWidth()
{
    return default_size.width();
}


int YUIQt::getDefaultHeight()
{
    return default_size.height();
}


// ----------------------------------------------------------------------


void YUIQt::returnNow(EventType et, YWidget *wid)
{

    if ( wid && ! wid->isValid() )
    {
	y2error("Widget has become invalid - ignoring.");
	return;
    }

    if ( wid && ! wid->yParent() )
    {
	/*
	 * This is a common situation, so let's not complain about it in the log
	 * file: The widget doesn't have a parent yet (since this is set from
	 * outside after the constructor has finished), yet it already sends
	 * signals. Most widgets with `opt(`notify) do this. Just silently ignore
	 * this situation.
	 */
	return;
    }

    if ( et != ET_CANCEL && wid &&
	 wid->yDialog() != currentDialog() )
    {
	/*
	 * Silently discard events from all but the current (topmost) dialog.
	 */

	y2warning( "Ignoring event from foreign dialog" );
	return;
    }

    if ( event_type == ET_NONE || event_type == ET_MENU )
    {
	event_type   = et;
	event_widget = wid;
    }

    if ( do_exit_loop )
    {
	exit_loop();
    }
}


const QFont &YUIQt::currentFont()
{
    /**
     * Brute force approach to make sure we'll really get a complete Unicode font:
     * Explicitly load the one font that we made sure to contain all required
     * characters, including Latin1, Latin2, Japanese, Korean, and the
     * characters used for glyphs.
     *
     * There are many fonts that claim to be Unicode, but most of them contain
     * just a sorry excuse for a complete Unicode character set. Qt can't know
     * how complete a font is, so it chooses one that might be better in other
     * aspects, but lacks necessary characters.
     **/

    if ( ! loaded_current_font )
    {
#if FORCE_UNICODE_FONT
	current_font = QFont( "Helvetica", 12 );
	current_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	current_font.setRawName( "-gnu-unifont-medium-r-normal--16-160-75-75-p-80-iso10646-1" );
	y2debug( "Loading default font: %s", (const char *) current_font.rawName() );
#else
	current_font = qApp->font();
#endif
	loaded_current_font = true;
    }

    return current_font;
}


const QFont &YUIQt::headingFont()
{
    /**
     * Brute force load the heading font - see currentFont() above for more.
     **/

    if ( ! loaded_heading_font )
    {
#if FORCE_UNICODE_FONT
	heading_font = QFont( "Helvetica", 14, QFont::Bold );
	heading_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	heading_font.setRawName( "-gnu-unifont-bold-r-normal--18-180-75-75-p-80-iso10646-1" );
	y2debug( "Loading heading font: %s", (const char *) heading_font.rawName() );
#else
	heading_font = QFont( "Helvetica", 14, QFont::Bold );
#endif
	loaded_heading_font = true;
    }

    return heading_font;
}


bool YUIQt::close()
{
    returnNow(ET_CANCEL, 0);
    return true;
}


bool YUIQt::eventFilter( QObject * obj, QEvent * ev )
{

    if ( ev->type() == QEvent::Close )
    {
	emit wmClose();

	if ( ! wm_close_blocked )
	{
	    // Don't simply close the application window, return from UserInput()
	    // with `id(`cancel) and let the YCP application decide how to handle
	    // that (e.g., ask for confirmation).

	    y2debug( "Caught window close event - returning with `cancel" );
	    returnNow(YUIInterpreter::ET_CANCEL, 0);
	}

	return true;	// Event processed
    }
    else if ( ev->type() == QEvent::Show )
    {
	if ( obj == main_win )
	{
	    if ( main_dialog_id > 0 )
	    {
		// Work around QWidgetStack bug: The last raiseWidget() call
		// (from closeDialog() ) might have failed if the widget was
		// invisible at that time, e.g., because the user had switched to
		// some other virtual desktop (bugzilla bug #11310)

		widget_stack->raiseWidget( main_dialog_id );
	    }
	}
	else
	{
	    return showEventFilter( obj, ev );
	}
    }

    return false;	// Don't stop event processing
}


bool YUIQt::showEventFilter( QObject * obj, QEvent * ev )
{
    if ( ! haveWM() )
    {
	// Make sure newly opened windows get the keyboard focus even without a
	// window manager. Otherwise the app might be unusable without a mouse.

	QWidget * widget = dynamic_cast<QWidget *> (obj);

	if ( widget )
	    widget->setActiveWindow();
    }

    return false;	// Don't stop event processing
}


void YUIQt::leaveIdleLoop(int)
{
    leave_idle_loop = true;
}


long YUIQt::defaultSize(YUIDimension dim) const
{
    if ( haveWM() )
    {
	return dim == YD_HORIZ ? default_size.width() : default_size.height();
    }
    else
    {
	return dim == YD_HORIZ ? desktop()->width() : desktop()->height();
    }
}


void YUIQt::makeScreenShot( std::string stl_filename )
{

    //
    // Grab the pixels off the screen
    //

    QWidget *dialog = (QWidget *) currentDialog()->widgetRep();
    QPixmap screenShot = QPixmap::grabWindow( dialog->winId() );
    QString fileName ( stl_filename.c_str() );

    if ( fileName.isEmpty() )
    {
	// Open a file selection box. Figure out a reasonable default
	// directory / file name.

	if ( screenShotNameTemplate.isEmpty() )
	{
	    //
	    // Initialize screen shot directory
	    //

	    QString home = QDir::homeDirPath();
	    char *ssdir = getenv("Y2SCREENSHOTS");
	    QString dir  = ssdir ? ssdir : "yast2-screen-shots";

	    if ( home == "/" )
	    {
		// Special case: $HOME is not set. This is normal in the inst-sys.
		// In this case, rather than simply dumping all screen shots into
		// /tmp which is world-writable, let's try to create a subdirectory
		// below /tmp with restrictive permissions.
		// If that fails, trust nobody - in particular, do not suggest /tmp
		// as the default in the file selection box.

		dir = "/tmp/" + dir;

		if ( mkdir( dir, 0700 ) == -1 )
		    dir = "";
	    }
	    else
	    {
		// For all others let's create a directory ~/yast2-screen-shots and
		// simply ignore if this is already present. This gives the user a
		// chance to create symlinks to a better location if he wishes so.

		dir = home + "/" + dir;
		(void) mkdir( dir, 0750 );
	    }

	    screenShotNameTemplate = dir + "/%s-%03d.png";
	}


	//
	// Figure out a file name
	//

	const char *baseName = moduleName();
	if ( ! baseName ) baseName = "scr";
	int no = screenShotNo[ baseName ];
	fileName.sprintf( screenShotNameTemplate, baseName, no );
	y2debug( "screenshot: %s", (const char *) fileName );
	fileName = askForSaveFileName( fileName, QString( "*.png" ) , "Save screen shot to..." );

	if ( fileName.isEmpty() )
	{
	    y2debug( "Save screen shot canceled by user" );
	    return;
	}

	screenShotNo.insert( baseName, ++no );
    } // if fileName.isEmpty()


    //
    // Actually save the screen shot
    //

    y2debug( "Saving screen shot to %s", (const char *) fileName );
    screenShot.save( fileName, "PNG" );
}


void YUIQt::toggleRecordMacro()
{
    if ( recordingMacro() )
    {
	stopRecordMacro();
	normalCursor();

	QMessageBox::information( 0,						// parent
				  "YaST2 Macro Recorder",			// caption
				  "Macro recording done.",			// text
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	busyCursor();
    }
    else
    {
	normalCursor();

	QString filename =
	    QFileDialog::getSaveFileName( DEFAULT_MACRO_FILE_NAME,		// startWith
					  "*.ycp",				// filter
					  0,					// parent
					  0,					// (widget) name
					  "Select Macro File to Record to" );	// caption
	busyCursor();

	if ( ! filename.isEmpty() )	// file selection dialog has been cancelled
	{
	    recordMacro( (const char *) filename );
	}
    }
}


void YUIQt::askPlayMacro()
{
    normalCursor();

    QString filename =
	QFileDialog::getOpenFileName( DEFAULT_MACRO_FILE_NAME,		// startWith
				      "*.ycp",				// filter
				      0,				// parent
				      0,				// (widget) name
				      "Select Macro File to Play" );	// caption
    busyCursor();

    if ( ! filename.isEmpty() )	// file selection dialog has been cancelled
    {
	playMacro( (const char *) filename );

	// Do special magic to get out of any UserInput() loop right now
	// without doing any harm - otherwise this would hang until the next
	// mouse click on a PushButton etc.

	event_type	= ET_WIDGET;
	event_widget	= currentDialog();

	if ( do_exit_loop )
	{
	    exit_loop();
	}
    }
}



YCPValue YUIQt::askForExistingDirectory( const YCPString & startDir,
					 const YCPString & headline )
{
    normalCursor();

    QString dir_name =
	QFileDialog::getExistingDirectory( fromUTF8( startDir->value() ),
					   main_win, 				// parent
					   "dir_selector",			// name
					   fromUTF8( headline->value() ) );	// caption
    busyCursor();

    if ( dir_name.isEmpty() )	// this includes dir_name.isNull()
	return YCPVoid();	// nothing selected -> return 'nil'

    return YCPString( toUTF8( dir_name ) );
}


YCPValue YUIQt::askForExistingFile( const YCPString & startWith,
				    const YCPString & filter,
				    const YCPString & headline )
{
    normalCursor();

    QString file_name =
	QFileDialog::getOpenFileName( fromUTF8( startWith->value() ),
				      fromUTF8( filter->value() ),
				      main_win, 			// parent
				      "file_selector",			// name
				      fromUTF8( headline->value() ) );	// caption
    busyCursor();

    if ( file_name.isEmpty() )	// this includes file_name.isNull()
	return YCPVoid();	// nothing selected -> return 'nil'

    return YCPString( toUTF8( file_name ) );
}


YCPValue YUIQt::askForSaveFileName( const YCPString & startWith,
				    const YCPString & filter,
				    const YCPString & headline )
{
    normalCursor();

    QString file_name = askForSaveFileName( fromUTF8( startWith->value() ),
					    fromUTF8( filter->value() ),
					    fromUTF8( headline->value() ) );
    busyCursor();

    if ( file_name.isEmpty() )		// this includes file_name.isNull()
	return YCPVoid();		// nothing selected -> return 'nil'

    return YCPString( toUTF8( file_name ) );
}



QString YUIQt::askForSaveFileName( const QString & startWith,
				   const QString & filter,
				   const QString & headline )
{
    QString file_name;
    bool try_again = false;

    do
    {
	// Leave the mouse cursor alone - this function might be called from
	// some other widget, not only from UI::AskForSaveFileName().

	file_name = QFileDialog::getSaveFileName( startWith,
						  filter,
						  main_win, 		// parent
						  "file_selector",	// name
						  headline );		// caption

	if ( file_name.isEmpty() )	// this includes file_name.isNull()
	    return QString::null;


	if ( access( (const char *) file_name, F_OK ) == 0 )	// file exists?
	{
	    QString msg;

	    if ( access( (const char *) file_name, W_OK ) == 0 )
	    {
		// Confirm if the user wishes to overwrite an existing file
		msg = ( _( "%1 exists! Really overwrite?" ) ).arg( file_name );
	    }
	    else
	    {
		// Confirm if the user wishes to overwrite a write-protected file %1
		msg = ( _( "%1 exists and is write-protected!\nReally overwrite?" ) ).arg( file_name );
	    }

	    int button_no = QMessageBox::information( main_win,
						      // Window title for confirmation dialog
						      _( "Confirm"   ),
						      msg,
						      _( "C&ontinue" ),
						      _( "&Cancel"   ) );
	    try_again = ( button_no != 0 );
	}

    } while ( try_again );

    return file_name;
}



#include "YUIQt.moc.cc"
