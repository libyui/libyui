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

/-*/


#include <rpc/types.h>		// MAXHOSTNAMELEN
#include <unistd.h>		// gethostname()
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
#include "QXEmbed.h"

#include <X11/Xlib.h>


#define DEFAULT_MACRO_FILE_NAME		"/tmp/macro.ycp"

YUIQt * YUIQt::_yuiqt = 0;


YUIQt::YUIQt(int argc, char **argv, bool with_threads, Y2Component *callback)
    : QApplication(argc, argv)
    , YUIInterpreter(with_threads, callback)
    , has_windowmanager(true)
    , main_dialog_id(0)
    , event_widget(0)
    , event_type(ET_NONE)
    , do_exit_loop(false)
    , loaded_current_font(false)
    , loaded_heading_font(false)
    , wm_close_blocked(false)
    , auto_activate_dialogs(true)
{
    suseheaderID = -1;
    _yuiqt = this;

    if (argv)
    {
	for(int i=0; i < argc; i++)
	{
	    if (argv[i] == QString( "-no-wm" ) )
	    {
		y2milestone( "Assuming the UI runs without window manager" );
		has_windowmanager = false;
	    }

	    if ( argv[i] == QString( "-kcontrol_id" ) )
	    {
		if ( i >= argc )
		{
		    y2error( "Missing arg for '-kcontrol_id'" );
		}
		else
		{
		    kcontrol_id = argv[++i];
		    y2milestone( "Starting with kcontrol_id='%s'",
				 (const char *) kcontrol_id );
		}
	    }
	}
    }


    if ( has_windowmanager )
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

	    y2milestone( "Assuming default size of %dx%d",
			 default_size.width(), default_size.height() );
	}
    }
    else	// ! has_windowmanager
    {
	default_size.setWidth ( qApp->desktop()->width()  );
	default_size.setHeight( qApp->desktop()->height() );
    }


    // Create widget stack for `opt(`defaultsize) dialogs

    main_win	 = new QVBox( 0, 0, WType_TopLevel );	// QWidgetStack doesn't accept a WFlags arg
    widget_stack = new QWidgetStack( main_win );
    widget_stack->setFocusPolicy( QWidget::StrongFocus );
    setMainWidget( main_win );
    qApp->installEventFilter( this );
    main_win->installEventFilter( this );
    main_win->resize( default_size );
    main_win->hide();	// visible only upon showDialog()
    busy_cursor = new QCursor( WaitCursor );


    // Set window title

    if ( kcontrol_id.isEmpty() )
    {
	QString title( "YaST2" );
	char hostname[ MAXHOSTNAMELEN+1 ];
	if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
	{
	    hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated
	    title += "@";
	    title += hostname;
	}
	main_win->setCaption( title );
	kcontrol_id = title;
    }
    else
    {
	main_win->setCaption( kcontrol_id );
    }


    //  Init other stuff

    setFont( currentFont() );
    QXEmbed::initialize();
    screenShotNameTemplate = "/tmp/%s-%03d.png";
    busyCursor();
    topmostConstructorHasFinished();
#if defined(QT_THREAD_SUPPORT)
    qApp->unlock ();
#endif
}


YUIQt::~YUIQt()
{
    y2debug("Closing down Qt UI.");

    if ( busy_cursor )
	delete busy_cursor;
}


void YUIQt::internalError( const char *msg )
{
    normalCursor();
    int button = QMessageBox::critical( 0, "YaST2 Internal Error", msg,
					QMessageBox::Retry | QMessageBox::Default,
					QMessageBox::Abort );
    busyCursor();

    if ( button == QMessageBox::Abort )
    {
	abort();
	// exit(1);	// exit() leaves a process running (WFM?).
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
    event_type = ET_NONE; // Clear for next time
    return event_widget;
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


YDialog *YUIQt::createDialog(YWidgetOpt &opt)
{
    bool has_defaultsize = opt.hasDefaultSize.value();
    QWidget *qt_parent = has_defaultsize ? main_win : 0;
    YDialog *dialog = new YQDialog( this, opt, qt_parent, has_defaultsize );

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
	    main_win->resize( default_size );

	    if ( ! has_windowmanager )
	    {
		main_win->move( 0, 0 );
	    }

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
	    main_win->hide();
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
    }
}


YContainerWidget *YUIQt::createReplacePoint(YWidget *parent, YWidgetOpt &opt)
{
    return new YQReplacePoint(this, ((QWidget *)parent->widgetRep()), opt);
}

YWidget *YUIQt::createEmpty(class YWidget *parent, YWidgetOpt &opt)
{
    return new YQEmpty(this, ((QWidget *)parent->widgetRep()), opt);
}

YWidget *YUIQt::createSpacing(YWidget *parent, YWidgetOpt &opt, float size, bool horizontal, bool vertical)
{
    return new YQSpacing(this, ((QWidget *)parent->widgetRep()), opt, size, horizontal, vertical);
}

YContainerWidget *YUIQt::createFrame(class YWidget *parent, YWidgetOpt &opt, const YCPString &label )
{
    return new YQFrame ( this, ( (QWidget *) parent->widgetRep()), opt, label );
}

YContainerWidget *YUIQt::createSplit(YWidget *parent, YWidgetOpt &opt, YUIDimension dimension)
{
    return new YQSplit(((QWidget *)parent->widgetRep()), opt, dimension);
}

YContainerWidget *YUIQt::createAlignment(YWidget *parent, YWidgetOpt &opt,
					 YAlignmentType halign,
					 YAlignmentType valign)
{
    return new YQAlignment(this, ((QWidget *)parent->widgetRep()), opt, halign, valign);
}

YContainerWidget *YUIQt::createSquash(YWidget *parent, YWidgetOpt &opt, bool hsquash, bool vsquash)
{
    return new YQSquash(this, ((QWidget *)parent->widgetRep()), opt, hsquash, vsquash);
}

YWidget *YUIQt::createLabel(YWidget *parent, YWidgetOpt &opt, const YCPString& text)
{
    return new YQLabel(this, (QWidget *)(parent->widgetRep()), opt, text);
}

YWidget *YUIQt::createLogView(YWidget *parent, YWidgetOpt &opt,
			      const YCPString& label, int visibleLines, int maxLines )
{
    return new YQLogView(this, (QWidget *)(parent->widgetRep()), opt, label, visibleLines, maxLines);
}

YWidget *YUIQt::createRichText(YWidget *parent, YWidgetOpt &opt, const YCPString& text)
{
    return new YQRichText(this, (QWidget *)(parent->widgetRep()), opt, text);
}

YWidget *YUIQt::createPackageSelector(YWidget *parent, YWidgetOpt &opt)
{
    auto_activate_dialogs = false;
    return new YQPackageSelector(this, (QWidget *)(parent->widgetRep()), opt);
}

YWidget *YUIQt::createPkgSpecial( YWidget *parent, YWidgetOpt &opt, const YCPString &subwidget )
{
    y2error( "The Qt UI does not support PkgSpecial subwidgets!" );
    return 0;
}

YWidget *YUIQt::createPushButton(YWidget *parent, YWidgetOpt &opt, const YCPString& label)
{
    return new YQPushButton(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createMenuButton(YWidget *parent, YWidgetOpt &opt, const YCPString& label)
{
    return new YQMenuButton(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createCheckBox(YWidget *parent, YWidgetOpt &opt, const YCPString& label, bool checked)
{
    return new YQCheckBox(this, (QWidget *)(parent->widgetRep()), opt, label, checked);
}

YWidget *YUIQt::createRadioButton(YWidget *parent, YWidgetOpt &opt, YRadioButtonGroup *rbg,
				  const YCPString& label, bool checked)
{
    return new YQRadioButton(this, (QWidget *)(parent->widgetRep()), opt, rbg, label, checked);
}

YContainerWidget *YUIQt::createRadioButtonGroup(YWidget *parent, YWidgetOpt &opt)
{
    return new YQRadioButtonGroup(this, (QWidget *)(parent->widgetRep()), opt);
}

YWidget *YUIQt::createTextEntry(YWidget *parent, YWidgetOpt &opt, const YCPString &label, const YCPString& text)
{
    return new YQTextEntry(this, (QWidget *)(parent->widgetRep()), opt, label, text);
}

YWidget *YUIQt::createMultiLineEdit(YWidget *parent, YWidgetOpt &opt, const YCPString &label, const YCPString& initialText)
{
    return new YQMultiLineEdit(this, (QWidget *)(parent->widgetRep()), opt, label, initialText);
}

YWidget *YUIQt::createSelectionBox(YWidget *parent, YWidgetOpt &opt, const YCPString &label)
{
    return new YQSelectionBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createMultiSelectionBox(YWidget *parent, YWidgetOpt &opt, const YCPString &label)
{
    return new YQMultiSelectionBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createComboBox(YWidget *parent, YWidgetOpt &opt, const YCPString &label)
{
    return new YQComboBox(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createTree(YWidget *parent, YWidgetOpt &opt, const YCPString &label)
{
    return new YQTree(this, (QWidget *)(parent->widgetRep()), opt, label);
}

YWidget *YUIQt::createTable(YWidget *parent, YWidgetOpt &opt, vector<string> header)
{
    return new YQTable(this, (QWidget *)(parent->widgetRep()), opt, header);
}

YWidget *YUIQt::createProgressBar(YWidget *parent, YWidgetOpt &opt, const YCPString &label,
				  const YCPInteger& maxProgress, const YCPInteger& progress)
{
    return new YQProgressBar(this, (QWidget *)(parent->widgetRep()), opt, label, maxProgress, progress);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt &opt, YCPByteblock imagedata, YCPString default_text)
{
    return new YQImage(this, (QWidget *)(parent->widgetRep()), opt, imagedata);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt &opt, YCPString file_name, YCPString default_text)
{
    return new YQImage(this, (QWidget *)(parent->widgetRep()), opt, file_name);
}

YWidget *YUIQt::createImage(YWidget *parent, YWidgetOpt &opt, ImageType img, YCPString default_text)
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

YWidget *YUIQt::createBarGraph(YWidget *parent, YWidgetOpt &opt)
{
    return new YQBarGraph(this, (QWidget *)(parent->widgetRep()), opt);
}


bool YUIQt::hasColoredLabel()
{
    return QColor::numBitPlanes() >= 15;
}

YWidget *YUIQt::createColoredLabel(YWidget *parent, YWidgetOpt &opt,
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
					  YWidgetOpt &opt,
					  const YCPString &label,
					  const YCPString &filename,
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


YCPValue YUIQt::setLanguage(const YCPTerm &term)
{
    return YCPVoid();	// OK (YCPNull() would mean error)
}


void
YUIQt::busyCursor ( void )
{
    /**
     * Simply using QApplication::setOverrideCursor( waitCursor ) doesn't work
     * any more: We _need_ the WType_Modal flag for non-defaultsize dialogs
     * (i.e. for popups), but Qt unfortunately doesn't let such dialogs have a
     * clock cursor.   :-(
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
}


void
YUIQt::normalCursor ( void )
{
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
// private helper functions


bool YUIQt::hasWM() const
{
    return has_windowmanager;
}


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
	current_font = QFont( "Helvetica", 12 );
	current_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	current_font.setRawName( "-gnu-unifont-medium-r-normal--16-160-75-75-p-80-iso10646-1" );
	loaded_current_font = true;
	y2milestone( "Loading default font: %s", (const char *) current_font.rawName() );
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
	heading_font = QFont( "Helvetica", 14, QFont::Bold );
	heading_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	heading_font.setRawName( "-gnu-unifont-bold-r-normal--18-180-75-75-p-80-iso10646-1" );
	y2milestone( "Loading heading font: %s", (const char *) heading_font.rawName() );
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
	if ( ! wm_close_blocked )
	{
	    // Don't simply close the application window, return from UserInput()
	    // with `id(`cancel) and let the YCP application decide how to handle
	    // that (e.g., ask for confirmation).

	    y2milestone( "Caught window close event - returning with `cancel" );
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
    if ( ! hasWM() )
    {
	// Make sure newly opened windows get the keyboard focus even without a
	// window manager. Otherwise the app might be unusable without a mouse.

	QWidget * wid = dynamic_cast<QWidget *> (obj);

	if ( wid )
	{
	    wid->setActiveWindow();
	}
    }

    return false;	// Don't stop event processing
}


void YUIQt::leaveIdleLoop(int)
{
    leave_idle_loop = true;
}


long YUIQt::defaultSize(YUIDimension dim) const
{
    if ( hasWM() )
    {
	return dim == YD_HORIZ ? default_size.width() : default_size.height();
    }
    else
    {
	return dim == YD_HORIZ ? desktop()->width() : desktop()->height();
    }
}


void YUIQt::makeScreenShot()
{
    QWidget *dialog = (QWidget *) currentDialog()->widgetRep();
    QPixmap screenShot = QPixmap::grabWindow( dialog->winId() );
    QString fileName;
    const char *baseName = moduleName();
    if ( ! baseName ) baseName = "scr";
    int no = screenShotNo[ baseName ];

    fileName.sprintf( screenShotNameTemplate, baseName, no );
    y2milestone("Saving screen shot to %s", (const char *) fileName );

    // Open a popup dialog to inform the user about the successful screen shot

    QMessageBox *mbox = new QMessageBox("YaST2",				// caption
					"Saving screen shot to\n" + fileName,	// text
					QMessageBox::Information,		// icon
					QMessageBox::Ok | QMessageBox::Default,	// button0
					QMessageBox::NoButton,			// button1
					QMessageBox::NoButton );		// button2

    // Actually make the screen shot

    screenShot.save ( fileName, "PNG" );
    screenShotNo.insert( baseName, ++no );

    QTimer::singleShot( 3000,	// msec
			mbox, SLOT(close()) );
    mbox->exec();
    delete mbox;

}


void YUIQt::toggleRecordMacro()
{
    if ( recordingMacro() )
    {
	stopRecordMacro();
	QMessageBox::information( 0,						// parent
				  "YaST2 Macro Recorder",			// caption
				  "Macro recording done.",			// text
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
    }
    else
    {
	QString filename =
	    QFileDialog::getSaveFileName( DEFAULT_MACRO_FILE_NAME,		// startWith
					  "*.ycp",				// filter
					  0,					// parent
					  0,					// (widget) name
					  "Select Macro File to Record to" );	// caption

	if ( ! filename.isEmpty() )	// file selection dialog has been cancelled
	{
	    recordMacro( (const char *) filename );
	}
    }
}


void YUIQt::askPlayMacro()
{
    QString filename =
	QFileDialog::getOpenFileName( DEFAULT_MACRO_FILE_NAME,		// startWith
				      "*.ycp",				// filter
				      0,				// parent
				      0,				// (widget) name
				      "Select Macro File to Play" );	// caption

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


#include "YUIQt.moc.cc"
