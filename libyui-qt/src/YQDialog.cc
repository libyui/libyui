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

  Textdomain	"packages-qt"

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include "YQUI.h"
#include "YQi18n.h"
#include "YEvent.h"
#include "YQDialog.h"
#include "YQGenericButton.h"
#include "YQWizardButton.h"
#include "YQWizard.h"
#include "YQMainWinDock.h"

// Include low-level X headers AFTER Qt headers:
// X.h pollutes the global namespace (!!!) with pretty useless #defines
// like "Above", "Below" etc. that clash with some Qt headers.
#include <X11/Xlib.h>


#define YQMainDialogWFlags	0


#define YQPopupDialogWFlags 		\
    ( Qt::WStyle_Customize 	|	\
      Qt::WStyle_Dialog		|	\
      Qt::WShowModal 		|	\
      WStyle_Title              |       \
      Qt::WStyle_DialogBorder	 )


YQDialog::YQDialog( YDialogType 	dialogType,
		    YDialogColorMode	colorMode )
    : QWidget( chooseParent( dialogType ),
	       0, // name
	       dialogType == YMainDialog ? YQMainDialogWFlags : YQPopupDialogWFlags )
    , YDialog( dialogType, colorMode )
{
    _userResized	= false;
    _focusButton	= 0;
    _defaultButton	= 0;

    setWidgetRep( this );
    setCaption( dialogType == YMainDialog ? "YaST2*" : "" );
    setFocusPolicy( QWidget::StrongFocus );

    if ( colorMode != YDialogNormalColor )
    {
	QColor normalBackground     ( 0, 128, 0 );
	QColor inputFieldBackground ( 0,  96, 0 );
	QColor text = white;

	if ( colorMode == YDialogInfoColor )
	{
	    normalBackground = QColor ( 238, 232, 170 ); // PaleGoldenrod
	}

	QPalette warnPalette( normalBackground );
	QColorGroup normalColors = warnPalette.normal();
	normalColors.setColor( QColorGroup::Text, text );
	normalColors.setColor( QColorGroup::Base, inputFieldBackground );
	warnPalette.setNormal( normalColors );
	setPalette( warnPalette );
    }

    if ( dialogType == YMainDialog &&
	 QWidget::parent() != YQMainWinDock::mainWinDock() )
    {
	setWFlags( YQPopupDialogWFlags );
    }
}


YQDialog::~YQDialog()
{
    if ( dialogType() == YMainDialog )
    {
	YQMainWinDock::mainWinDock()->remove( (QWidget *) widgetRep() );
    }
}


QWidget *
YQDialog::chooseParent( YDialogType dialogType )
{
    QWidget * parent = 0;
    
    if ( dialogType == YMainDialog &&
	 YQMainWinDock::mainWinDock()->couldDock() )
    {
	y2debug( "Adding dialog to mainWinDock" );
	parent = YQMainWinDock::mainWinDock();
    }
    
    return parent;
}


int
YQDialog::preferredWidth()
{
    int preferredWidth;

    if ( dialogType() == YMainDialog )
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
	y2warning( "Limiting dialog width to screen width (%d) instead of %d - check the layout!",
		   screenWidth, preferredWidth );
    }

    return preferredWidth;
}


int
YQDialog::preferredHeight()
{
    int preferredHeight;

    if ( dialogType() == YMainDialog )
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
	y2warning( "Limiting dialog height to screen height (%d) instead of %d - check the layout!",
		   screenHeight, preferredHeight );
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
    // y2debug( "Resizing dialog to %d x %d", newWidth, newHeight );

    if ( newWidth > qApp->desktop()->width() )
	newWidth = qApp->desktop()->width();

    if ( newHeight > qApp->desktop()->height() )
	newHeight = qApp->desktop()->height();

    if ( hasChildren() )
    {
	firstChild()->setSize( newWidth, newHeight );
    }

    resize( newWidth, newHeight );
}


void
YQDialog::activate( bool active )
{
    if ( active )
    {
	ensureOnlyOneDefaultButton();
    }
}


void
YQDialog::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	// y2debug( "Resize event: %d x %d", event->size().width(), event->size().height() );
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
		    y2error( "Too many default buttons: [%s]",
			     (const char *) button->text() );
		    y2error( "Using old default button: [%s]",
			     (const char *) _defaultButton->text() );
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
YQDialog::setDefaultButton( YQGenericButton * newDefaultButton )
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
	    y2error( "Too many `opt(`default) PushButtons: [%s]",
		     (const char *) newDefaultButton->text() );
	    newDefaultButton->setDefaultButton( false );
	    return;
	}
    }

    _defaultButton = newDefaultButton;

    if ( _defaultButton )
    {
	_defaultButton->setDefaultButton( true );
	y2debug( "New default button: [%s]", (const char *) _defaultButton->text() );

	if ( _defaultButton && ! _focusButton )
	    _defaultButton->showAsDefault( true );
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
	y2debug( "Activating focus button: [%s]", (const char *) _focusButton->text() );
	_focusButton->activate();
	return true;
    }


    // No focus button - try the default button, if there is any.

    _defaultButton = findDefaultButton();

    if ( _defaultButton 		&&
	 _defaultButton->isEnabled() 	&&
	 _defaultButton->isShownAsDefault() )
    {
	y2debug( "Activating default button: [%s]", (const char *) _defaultButton->text() );
	_defaultButton->activate();
	return true;
    }
    else
    {
	if ( warn )
	{
	    y2warning( "No default button in this dialog - ignoring [Return]" );
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
	else if ( event->key()   == Qt::Key_F4 &&	// Shift-F4: toggle colors for vision impaired users
		  event->state() == Qt::ShiftButton )
	{
	    YQUI::ui()->toggleVisionImpairedPalette();

	    if ( YQUI::ui()->usingVisionImpairedPalette() )
	    {
		y2milestone( "Switched to vision impaired palette" );
		QMessageBox::information( 0,                                            // parent
					  _("Color switching"),  	                // caption
					  _( "Switching to color palette for vision impaired users -\n"
					     "press Shift-F4 again to switch back to normal colors."   ), // text
					  QMessageBox::Ok | QMessageBox::Default,       // button0
					  QMessageBox::NoButton,                        // button1
					  QMessageBox::NoButton );                      // button2
	    }
	    return;
	}
	else if ( event->key()   == Qt::Key_F7 &&	// Shift-F7: toggle y2debug logging
		  event->state() == Qt::ShiftButton )
	{
	    YQUI::ui()->askConfigureLogging();
	    return;
	}
	else if ( event->key()   == Qt::Key_F8 &&	// Shift-F8: save y2logs
		  event->state() == Qt::ShiftButton )
	{
	    YQUI::ui()->askSaveLogs();
	    return;
	}
	else if ( event->state() == 0 )			// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		( void ) activateDefaultButton();
		return;
	    }
	}
	else if ( event->state() == ( Qt::ControlButton | Qt::ShiftButton | Qt::AltButton ) )
	{
	    // Qt-UI special keys - all with Ctrl-Shift-Alt

	    y2milestone( "Caught YaST2 magic key combination" );

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
	    else if ( event->key() == Qt::Key_X )
	    {
		y2milestone( "Starting xterm" );
		system( "/usr/bin/xterm &" );
		return;
	    }
	}
    }

    QWidget::keyPressEvent( event );
}


#if 0
void
YQDialog::showEvent( QShowEvent * event )
{
    y2debug( "Showing dialog %p", this );
    // y2debug( "Size: %d x %d", size().width(), size().height() );

    QWidget::showEvent( event );
}
#endif


#if 0
void
YQDialog::paintEvent( QPaintEvent * event )
{
    if ( event )
    {
	QRect rect = event->rect();
	y2debug( "Repainting dialog %p: rect (%d, %d) w: %d h: %d",
		 this, rect.x(), rect.y(), rect.width(), rect.height() );
    }

    QWidget::paintEvent( event );
}
#endif


void
YQDialog::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button (and WM menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the `id`( `cancel ) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    y2milestone( "Caught window manager close event - returning with YCancelEvent" );
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQDialog::focusInEvent( QFocusEvent * event )
{

    // The dialog itself doesn't need or want the keyboard focus, but obviously
    // ( since Qt 2.3? ) it needs QFocusPolicy::StrongFocus for the default
    // button mechanism to work. So let's accept the focus and give it to some
    // child widget.

    if ( event->reason() == QFocusEvent::Tab )
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


void
YQDialog::center( QWidget * dialog, QWidget * parent )
{
    if ( ! dialog || ! parent )
	return;

    QPoint pos( ( parent->width()  - dialog->width()  ) / 2,
		( parent->height() - dialog->height() ) / 2 );

    pos += parent->mapToGlobal( QPoint( 0, 0 ) );
    pos = dialog->mapToParent( dialog->mapFromGlobal( pos ) );
    dialog->move( pos );
}



#include "YQDialog.moc"
