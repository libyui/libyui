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

  File:	      	YQDialog.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"qt"

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QDesktopWidget>
#include <QDebug>

#include "YQUI.h"
#include "YQi18n.h"
#include "YEvent.h"
#include "YQDialog.h"
#include "YQGenericButton.h"
#include "YQWizardButton.h"
#include "YQWizard.h"
#include "YQMainWinDock.h"
#include "YDialogSpy.h"
#include "QY2Styler.h"
#include "QY2StyleEditor.h"

// Include low-level X headers AFTER Qt headers:
// X.h pollutes the global namespace (!!!) with pretty useless #defines
// like "Above", "Below" etc. that clash with some Qt headers.
#include <X11/Xlib.h>

#define YQMainDialogWFlags	Qt::Widget
#define YQPopupDialogWFlags     Qt::Dialog

#define VERBOSE_EVENT_LOOP	0



YQDialog::YQDialog( YDialogType 	dialogType,
		    YDialogColorMode	colorMode )
    : QWidget( chooseParent( dialogType ),
	       dialogType == YPopupDialog ? YQPopupDialogWFlags : YQMainDialogWFlags )
    , YDialog( dialogType, colorMode )
{
    setWidgetRep( this );

    _userResized   	= false;
    _focusButton   	= 0;
    _defaultButton 	= 0;
    _highlightedChild	= 0;
    _styleEditor	= 0;

    setFocusPolicy( Qt::StrongFocus );
    setAutoFillBackground( true );

    if ( colorMode != YDialogNormalColor )
    {
	QColor normalBackground     ( 240, 100, 36 );
	QColor inputFieldBackground ( 0xbb, 0xff, 0xbb );
	QColor text = Qt::black;

	if ( colorMode == YDialogInfoColor )
	{
	    normalBackground = QColor ( 238, 232, 170 ); // PaleGoldenrod
	}

	QPalette warnPalette( normalBackground );
	warnPalette.setColor( QPalette::Text, text );
	warnPalette.setColor( QPalette::Base, inputFieldBackground );
	setPalette( warnPalette );
    }

    setWindowTitle( YQUI::ui()->applicationTitle() );

    if ( isMainDialog() && QWidget::parent() != YQMainWinDock::mainWinDock() )
    {
	setWindowFlags( YQPopupDialogWFlags );
    }
    
    if ( ! isMainDialog() )
       setWindowModality( Qt::ApplicationModal );

    if ( isMainDialog() && QWidget::parent() == YQMainWinDock::mainWinDock() )
    {
        YQMainWinDock::mainWinDock()->add( this );
    }

    _eventLoop  = new QEventLoop( this );
    YUI_CHECK_NEW( _eventLoop );
    
    _waitForEventTimer = new QTimer( this );
    YUI_CHECK_NEW( _waitForEventTimer );
    _waitForEventTimer->setSingleShot( true );
    
    QObject::connect( _waitForEventTimer, 	SIGNAL( timeout()	       ),
		      this,			SLOT  ( waitForEventTimeout() ) );
}


YQDialog::~YQDialog()
{
    if ( isMainDialog() )
    {
	YQMainWinDock::mainWinDock()->remove( this );
	// orphaned main dialogs are handled gracefully in YQWMainWinDock::remove()
    }
    
    if ( _defaultButton )
       _defaultButton->forgetDialog();
    
    if ( _focusButton )
       _focusButton->forgetDialog();

    if ( _styleEditor )
        delete _styleEditor;
}


QWidget *
YQDialog::chooseParent( YDialogType dialogType )
{
    QWidget * parent = YQMainWinDock::mainWinDock()->window();

    if ( dialogType == YPopupDialog)
    {
	YDialog * currentDialog = YDialog::currentDialog( false );
	if (currentDialog)
	    parent = (QWidget *) currentDialog->widgetRep();
    }

    if ( ( dialogType == YMainDialog || dialogType == YWizardDialog ) &&
	 YQMainWinDock::mainWinDock()->couldDock() )
    {
	yuiDebug() << "Adding dialog to mainWinDock" << endl;
	parent = YQMainWinDock::mainWinDock();
    }

    return parent;
}


void
YQDialog::openInternal()
{
    ensureOnlyOneDefaultButton();
    QWidget::show();
    QWidget::raise(); // FIXME: is this really necessary?
    QWidget::update();
}


void
YQDialog::activate()
{
    QWidget::raise();
    QWidget::update();
}


int
YQDialog::preferredWidth()
{
    int preferredWidth;

    if ( isMainDialog() )
    {
	if ( userResized() )
	    preferredWidth = _userSize.width();
	else
	    preferredWidth = YQUI::ui()->defaultSize( YD_HORIZ );
    }
    else
    {
	preferredWidth = YDialog::preferredWidth();
    }

    int screenWidth = qApp->desktop()->width();

    if ( preferredWidth > screenWidth )
    {
	yuiWarning() << "Limiting dialog width to screen width (" << screenWidth
		     << ") instead of " << preferredWidth
		     << " - check the layout!"
		     << endl;
    }

    return preferredWidth;
}


int
YQDialog::preferredHeight()
{
    int preferredHeight;

    if ( isMainDialog() )
    {
	if ( userResized() )
	    preferredHeight = _userSize.height();
	else
	    preferredHeight = YQUI::ui()->defaultSize( YD_VERT );
    }
    else
    {
	preferredHeight = YDialog::preferredHeight();
    }

    int screenHeight = qApp->desktop()->height();

    if ( preferredHeight > screenHeight )
    {
	yuiWarning() << "Limiting dialog height to screen height (" << screenHeight
		     << ") instead of " << preferredHeight
		     << " - check the layout!"
		     << endl;
    }

    return preferredHeight;
}


void
YQDialog::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YDialog::setEnabled( enabled );
}


void
YQDialog::setSize( int newWidth, int newHeight )
{
    // yuiDebug() << "Resizing dialog to " << newWidth << " x " << newHeight << endl;

    if ( newWidth > qApp->desktop()->width() )
	newWidth = qApp->desktop()->width();

    if ( newHeight > qApp->desktop()->height() )
	newHeight = qApp->desktop()->height();

    resize( newWidth, newHeight );

    if ( hasChildren() )
    {
        firstChild()->setSize( newWidth, newHeight );
        ( ( QWidget* )firstChild()->widgetRep() )->show();
    }
}


void
YQDialog::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	// yuiDebug() << "Resize event: " << event->size().width() << " x " << event->size().height() << endl;
	setSize ( event->size().width(), event->size().height() );
	_userSize = event->size();

	if ( QWidget::parent() )
	    _userResized = true;
    }
}


YQGenericButton *
YQDialog::findDefaultButton()
{
    if ( _defaultButton )
	return _defaultButton;

    _defaultButton = findDefaultButton( childrenBegin(), childrenEnd() );

    YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
    YDialog::setDefaultButton( _defaultButton );

    return _defaultButton;
}


YQGenericButton *
YQDialog::findDefaultButton( YWidgetListConstIterator begin,
			     YWidgetListConstIterator end ) const
{
    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YWidget * widget = *it;

	//
	// Check this widget
	//

	YQGenericButton * button = dynamic_cast<YQGenericButton *> (widget);

	if ( button && button->isDefaultButton() )
	{
	    return button;
	}


	//
	// Recurse over the children of this widget
	//

	if ( widget->hasChildren() )
	{
	    button = findDefaultButton( widget->childrenBegin(),
					widget->childrenEnd() );
	    if ( button )
		return button;
	}
    }

    return 0;
}


YQWizard *
YQDialog::ensureOnlyOneDefaultButton( YWidgetListConstIterator begin,
				      YWidgetListConstIterator end )
{
    YQGenericButton * def  = _focusButton ? _focusButton : _defaultButton;
    YQWizard * wizard = 0;

    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YQGenericButton * button       = dynamic_cast<YQGenericButton *> (*it);
	YQWizardButton  * wizardButton = dynamic_cast<YQWizardButton * > (*it);

	if ( ! wizard )
	    wizard = dynamic_cast<YQWizard *> (*it);

	if ( wizardButton )
	{
	    wizardButton->showAsDefault( false );
	}
	else if ( button )
	{
	    if ( button->isDefaultButton() )
	    {
		if ( _defaultButton && button != _defaultButton )
		{
		    yuiError() << "Too many default buttons: " << button  << endl;
		    yuiError() << "Using old default button: " << _defaultButton << endl;
		}
		else
		{
		    _defaultButton = button;
		}
	    }

	    if ( button->isShownAsDefault() && button != def )
		button->showAsDefault( false );
	}

	if ( (*it)->hasChildren() )
	{
	    YQWizard * wiz = ensureOnlyOneDefaultButton( (*it)->childrenBegin(),
							 (*it)->childrenEnd() );
	    if ( wiz )
		wizard = wiz;
	}
    }

    return wizard;
}


void
YQDialog::ensureOnlyOneDefaultButton()
{
    _defaultButton = 0;
    YQWizard * wizard = ensureOnlyOneDefaultButton( childrenBegin(), childrenEnd() );

    if ( ! _defaultButton && wizard )
    {
	_defaultButton = wizardDefaultButton( wizard );
    }

    if ( _defaultButton )
    {
	YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
	YDialog::setDefaultButton( _defaultButton );
    }


    YQGenericButton * def  = _focusButton ? _focusButton : _defaultButton;

    if ( def )
	def->showAsDefault();
}


YQWizard *
YQDialog::findWizard() const
{
    return findWizard( childrenBegin(), childrenEnd() );
}


YQWizard *
YQDialog::findWizard( YWidgetListConstIterator begin,
		      YWidgetListConstIterator end ) const
{
    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YWidget * widget = *it;
	YQWizard * wizard = dynamic_cast<YQWizard *> (widget);

	if ( wizard )
	    return wizard;

	if ( widget->hasChildren() )
	{
	    wizard = findWizard( widget->childrenBegin(),
				 widget->childrenEnd() );
	    if ( wizard )
		return wizard;
	}
    }

    return 0;
}


YQGenericButton *
YQDialog::wizardDefaultButton( YQWizard * wizard ) const
{
    YQGenericButton * def = 0;

    if ( ! wizard )
	wizard = findWizard();

    if ( wizard )
    {
	// Pick one of the wizard buttons

	if ( wizard->direction() == YQWizard::Backward )
	{
	    if ( wizard->backButton()
		 && wizard->backButton()->isShown()
		 && wizard->backButton()->isEnabled() )
	    {
		def = wizard->backButton();
	    }
	}

	if ( ! def )
	{
	    if ( wizard->nextButton()
		 && wizard->nextButton()->isShown()
		 && wizard->nextButton()->isEnabled() )
	    {
		def = wizard->nextButton();
	    }
	}
    }

    return def;
}


void
YQDialog::setDefaultButton( YPushButton * newDefaultButton )
{
    if ( _defaultButton	  &&
	 newDefaultButton &&
	 newDefaultButton != _defaultButton )
    {
	if ( dynamic_cast<YQWizardButton *>( _defaultButton ) )
	{
	    // Let app defined default buttons override wizard buttons
	    _defaultButton->setDefaultButton( false );
	}
	else
	{
	    yuiError() << "Too many `opt(`default) PushButtons: " << newDefaultButton << endl;
	    newDefaultButton->setDefaultButton( false );
	    return;
	}
    }

    _defaultButton = dynamic_cast<YQGenericButton*>(newDefaultButton);

    if ( _defaultButton )
    {
	_defaultButton->setDefaultButton( true );
	yuiDebug() << "New default button: " << _defaultButton << endl;

	if ( _defaultButton && ! _focusButton )
	{
	    _defaultButton->showAsDefault( true );
	    _defaultButton->setKeyboardFocus();
	}
    }


    YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
    YDialog::setDefaultButton( _defaultButton );
}


bool
YQDialog::activateDefaultButton( bool warn )
{
    // Try the focus button first, if there is any.

    if ( _focusButton 		   &&
	 _focusButton->isEnabled() &&
	 _focusButton->isShownAsDefault() )
    {
	yuiDebug() << "Activating focus button: " << _focusButton << endl;
	_focusButton->activate();
	return true;
    }


    // No focus button - try the default button, if there is any.

    _defaultButton = findDefaultButton();

    if ( _defaultButton 		&&
	 _defaultButton->isEnabled() 	&&
	 _defaultButton->isShownAsDefault() )
    {
	yuiDebug() << "Activating default button: " << _defaultButton << endl;
	_defaultButton->activate();
	return true;
    }
    else
    {
	if ( warn )
	{
	    yuiWarning() << "No default button in this dialog - ignoring [Return]" << endl;
	}
    }

    return false;
}


void
YQDialog::losingFocus( YQGenericButton * button )
{
    if ( button == _focusButton )
    {
	if ( _focusButton && _focusButton != _defaultButton )
	    _focusButton->showAsDefault( false );

	_focusButton = 0;
    }

    if ( ! _focusButton && _defaultButton )
	_defaultButton->showAsDefault( true );
}


void
YQDialog::gettingFocus( YQGenericButton * button )
{
    if ( _focusButton && _focusButton != button )
	_focusButton->showAsDefault( false );

    if ( _defaultButton && _defaultButton != button )
	_defaultButton->showAsDefault( false );

    _focusButton = button;

    if ( _focusButton )
	_focusButton->showAsDefault( true );
}


void
YQDialog::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	if ( event->key() == Qt::Key_Print )
	{
	    YQUI::ui()->makeScreenShot( "" );
	    return;
	}
	else if ( event->key()       == Qt::Key_F4 &&	// Shift-F4: toggle colors for vision impaired users
		  event->modifiers() == Qt::ShiftModifier )
	{
	    YQUI::ui()->toggleVisionImpairedPalette();

	    if ( YQUI::ui()->usingVisionImpairedPalette() )
	    {
		QWidget* parent = 0;
		YDialog * currentDialog = YDialog::currentDialog( false );
		if (currentDialog)
		    parent = (QWidget *) currentDialog->widgetRep();

		yuiMilestone() << "Switched to vision impaired palette" << endl;
		QMessageBox::information( parent,                                       // parent
					  _("Color switching"),  	                // caption
					  _( "Switching to color palette for vision impaired users -\n"
					     "press Shift-F4 again to switch back to normal colors."   ), // text
					  QMessageBox::Ok | QMessageBox::Default,       // button0
					  QMessageBox::NoButton,                        // button1
					  QMessageBox::NoButton );                      // button2
	    }
	    return;
	}
	else if ( event->key()       == Qt::Key_F7 &&	// Shift-F7: toggle debug logging
		  event->modifiers() == Qt::ShiftModifier )
	{
	    YQUI::ui()->askConfigureLogging();
	    return;
	}
	else if ( event->key()       == Qt::Key_F8 &&	// Shift-F8: save y2logs
		  event->modifiers() == Qt::ShiftModifier )
	{
	    YQUI::ui()->askSaveLogs();
	    return;
	}
	else if ( event->modifiers() == Qt::NoModifier )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		(void) activateDefaultButton();
		return;
	    }
	}
	else if ( event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) )
	{
	    // Qt-UI special keys - all with Ctrl-Shift-Alt

	    yuiMilestone() << "Caught YaST2 magic key combination" << endl;

	    if ( event->key() == Qt::Key_M )
	    {
		YQUI::ui()->toggleRecordMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_P )
	    {
		YQUI::ui()->askPlayMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_D )
	    {
		YQUI::ui()->sendEvent( new YDebugEvent() );
		return;
	    }
	    else if ( event->key() == Qt::Key_T )
	    {
		yuiMilestone() << "*** Dumping widget tree ***" << endl;
		dumpWidgetTree();
		yuiMilestone() << "*** Widget tree end ***" << endl;
		return;
	    }
	    else if ( event->key() == Qt::Key_Y )
	    {
		yuiMilestone() << "Opening dialog spy" << endl;
		YDialogSpy::showDialogSpy();
		YQUI::ui()->normalCursor();
	    }
	    else if ( event->key() == Qt::Key_X )
	    {
		yuiMilestone() << "Starting xterm" << endl;
		system( "/usr/bin/xterm &" );
		return;
	    }
	    else if ( event->key() == Qt::Key_S )
	    {
		yuiMilestone() << "Opening style editor" << endl;
                _styleEditor = new QY2StyleEditor(this);
                _styleEditor->show();
                _styleEditor->raise();
                _styleEditor->activateWindow();
		return;
	    }

	}
    }

    QWidget::keyPressEvent( event );
}


void
YQDialog::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button (and WM menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the `id`( `cancel ) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    yuiMilestone() << "Caught window manager close event - returning with YCancelEvent" << endl;
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQDialog::focusInEvent( QFocusEvent * event )
{
    // The dialog itself doesn't need or want the keyboard focus, but obviously
    // (since Qt 2.3?) it needs QFocusPolicy::StrongFocus for the default
    // button mechanism to work. So let's accept the focus and give it to some
    // child widget.

    if ( event->reason() == Qt::TabFocusReason )
    {
	focusNextPrevChild( true );
    }
    else
    {
	if ( _defaultButton )
	    _defaultButton->setKeyboardFocus();
	else
	    focusNextPrevChild( true );
    }
}


YEvent *
YQDialog::waitForEventInternal( int timeout_millisec )
{
    YQUI::ui()->forceUnblockEvents();
    _eventLoop->wakeUp();

    YEvent * event  = 0;

    _waitForEventTimer->stop();

    if ( timeout_millisec > 0 )
	_waitForEventTimer->start( timeout_millisec ); // single shot

    if ( qApp->focusWidget() )
	qApp->focusWidget()->setFocus();

    YQUI::ui()->normalCursor();

    if ( ! _eventLoop->isRunning() )
    {
#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Executing event loop for " << this << endl;
#endif
	_eventLoop->exec();
	
#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Event loop finished for " << this << endl;
#endif
    }
    else
    {
#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Event loop still running for " << this << endl;
#endif
    }
	
    _waitForEventTimer->stop();
    event = YQUI::ui()->consumePendingEvent();

    
    // Prepare a busy cursor if the UI cannot respond to user input within the
    // next 200 milliseconds (if the application doesn't call waitForEvent()
    // within this time again)
    
    YQUI::ui()->timeoutBusyCursor();

    return event;
}


YEvent *
YQDialog::pollEventInternal()
{
    YEvent * event = 0;

    _waitForEventTimer->stop(); // just in case it's still running

    if ( ! YQUI::ui()->pendingEvent() )
    {
	// Very short (10 millisec) event loop
	_eventLoop->processEvents( QEventLoop::AllEvents, 10 );
    }

    if ( YQUI::ui()->pendingEvent() )
	event = YQUI::ui()->consumePendingEvent();

    return event;
}


void
YQDialog::waitForEventTimeout()
{
    if ( ! YQUI::ui()->pendingEvent() )
    {
	// Don't override a pending event with a timeout event
	
	YQUI::ui()->sendEvent( new YTimeoutEvent() );
    }
}


void
YQDialog::center( QWidget * dialog, QWidget * parent )
{
    if ( ! dialog || ! parent )
        return;

    QPoint pos( ( parent->width()  - dialog->width()  ) / 2,
                ( parent->height() - dialog->height() ) / 2 );

    pos += parent->mapToGlobal( QPoint( 0, 0 ) );
    pos = dialog->mapToParent( dialog->mapFromGlobal( pos ) );
    qDebug() << pos;
    dialog->move( pos );
}


void
YQDialog::highlight( YWidget * child )
{
    if ( _highlightedChild && _highlightedChild->isValid() )
    {
	// Un-highlight old highlighted child widget

	QWidget * qw = (QWidget *) _highlightedChild->widgetRep();

	if ( qw )
	{
	    qw->setPalette( _preHighlightPalette );
	    qw->setAutoFillBackground( _preHighlightAutoFill );
	}
    }

    _highlightedChild = child;

    if ( child )
    {
	QWidget * qw = (QWidget *) child->widgetRep();

	if ( qw )
	{
	    _preHighlightPalette  = qw->palette();
	    _preHighlightAutoFill = qw->autoFillBackground();
	    
	    qw->setAutoFillBackground( true );
	    QPalette pal( QColor( 0xff, 0x66, 0x00 ) );	// Button color
	    pal.setBrush( QPalette::Window, QColor( 0xff, 0xaa, 0x00 ) ); // Window background
	    pal.setBrush( QPalette::Base  , QColor( 0xff, 0xee, 0x00 ) ); // Table etc. background 
	    
	    qw->setPalette( pal );
	}
    }
}


#include "YQDialog.moc"
