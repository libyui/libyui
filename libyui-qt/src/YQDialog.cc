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

  Authors:	Mathias Kettner <kettner@suse.de>
		Stefan Hundhammer <sh@suse.de>
		
  Maintainer:	Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <X11/Xlib.h>

#include "YQUI.h"
#include "YEvent.h"
#include "YQDialog.h"
#include "YQPushButton.h"


YQDialog::YQDialog( YWidgetOpt &	opt,
		    QWidget *		qt_parent,
		    bool		default_size )
    : QWidget( qt_parent,
	       0,	// name
	       default_size ? 0 : WType_Modal | WStyle_Dialog )
    , YDialog( opt )
{
    _userResized	= false;
    _focusButton	= 0;
    _defaultButton	= 0;


    setWidgetRep( this );
    setCaption( hasDefaultSize() ? "YaST2" : "" );
    setFocusPolicy( QWidget::StrongFocus );

    if ( hasWarnColor() || hasInfoColor() )
    {
	QColor normalBackground     ( 0, 128, 0 );
	QColor inputFieldBackground ( 0,  96, 0 );
	QColor text = white;

	if ( hasInfoColor() )
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

    _qFrame = new QFrame ( this );

    if ( ! hasDefaultSize() && ! YQUI::ui()->haveWM() )
    {
	_qFrame->setFrameStyle ( QFrame::Box | QFrame::Raised );
	_qFrame->setLineWidth(2);
	_qFrame->setMidLineWidth(3);
    }
    else
    {
	_qFrame->setFrameStyle ( QFrame::NoFrame );
    }
}


YQDialog::~YQDialog()
{
}


long YQDialog::nicesize( YUIDimension dim )
{
    long nice;

    if ( hasDefaultSize() )
    {
	if ( userResized() )
	{
	    nice = dim == YD_HORIZ ? _userSize.width() : _userSize.height();
	}
	else
	{
	    nice = YQUI::ui()->defaultSize( dim );
	}
    }
    else
    {
	nice = YDialog::nicesize( dim ) + 2 * decorationWidth( dim );
    }

    long screenSize = dim == YD_HORIZ ? qApp->desktop()->width() : qApp->desktop()->height();

    if ( nice > screenSize )
    {
	y2warning( "Limiting dialog size to screen size (%ld) instead of %ld - check the layout!",
		   screenSize, nice );
    }

    return nice;
}


long YQDialog::decorationWidth( YUIDimension dim )
{
    if ( ! hasDefaultSize() && _qFrame )
	return _qFrame->frameWidth();
    else
	return 0L;
}


void YQDialog::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


void YQDialog::setSize( long newWidth, long newHeight )
{
    if ( newWidth > qApp->desktop()->width() )
    {
	y2warning( "Limiting dialog width to screen width (%d) instead of %ld - check the layout!",
		   qApp->desktop()->width(), newWidth );

	newWidth = qApp->desktop()->width();
    }

    if ( newHeight > qApp->desktop()->height() )
    {
	y2warning( "Limiting dialog height to screen height (%d) instead of %ld - check the layout!",
		   qApp->desktop()->height(), newHeight );

	newHeight = qApp->desktop()->height();
    }


    if ( numChildren() > 0 )
    {
	YContainerWidget::child(0)->setSize(newWidth  - 2 * decorationWidth( YD_HORIZ ),
					    newHeight - 2 * decorationWidth( YD_VERT  ) );
    }
    
    if ( _qFrame )
	_qFrame->resize( newWidth, newHeight );

    resize( newWidth, newHeight );
}



void YQDialog::activate( bool active )
{
    if ( active )
    {
	if ( ! YQUI::ui()->haveWM() )
	{
	    if ( YQUI::ui()->autoActivateDialogs() )
		setActiveWindow();
	    else
		y2milestone( "Auto-activating dialog window turned off" );
	}

	ensureOnlyOneDefaultButton();
    }
}


void
YQDialog::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	setSize ( event->size().width(), event->size().height() );
	_userSize    = event->size();
	_userResized = true;
    }
}


YQPushButton *
YQDialog::findDefaultButton()
{
    if ( _defaultButton )
	return _defaultButton;
    
    YWidgetList widgetList   = YDialog::widgets();

    for ( YWidgetListIterator it = widgetList.begin(); it != widgetList.end(); ++it )
    {
	YQPushButton * button = dynamic_cast<YQPushButton *> ( *it );

	if ( button && button->isDefault() )
	{
	    _defaultButton = button;
	    
	    return _defaultButton;
	}
    }

    _defaultButton = 0;
    
    return _defaultButton;
}


void
YQDialog::ensureOnlyOneDefaultButton()
{
    YQPushButton * def     = _focusButton ? _focusButton : _defaultButton;
    YWidgetList widgetList = YDialog::widgets();

    for ( YWidgetListIterator it = widgetList.begin(); it != widgetList.end(); ++it )
    {
	YQPushButton * button = dynamic_cast<YQPushButton *> ( *it );

	if ( button )
	{
	    if ( button->isDefault() )
	    {
		if ( _defaultButton && button != _defaultButton )
		{
		    y2error( "Too many `opt( `default ) PushButtons: [%s]",
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
    }
    
    def = _focusButton ? _focusButton : _defaultButton;
    
    if ( def )
	def->showAsDefault();
}


void
YQDialog::setDefaultButton( YQPushButton * newDefaultButton )
{
    if ( _defaultButton	  &&
	 newDefaultButton &&
	 newDefaultButton != _defaultButton )
    {
	y2error( "Too many `opt( `default ) PushButtons: [%s]", (const char *) newDefaultButton->text() );
	newDefaultButton->setDefault( false );
	return;
    }

    _defaultButton = newDefaultButton;

    if ( _defaultButton )
    {
	_defaultButton->setDefault( true );
	y2debug( "New default button: [%s]", (const char *) _defaultButton->text() );
	
	if ( _defaultButton && ! _focusButton )
	    _defaultButton->showAsDefault( true );
    }
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


void YQDialog::losingFocus( YQPushButton * button )
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


void YQDialog::gettingFocus( YQPushButton * button )
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
    unsigned yast2_special_combo = ( Qt::ControlButton | Qt::ShiftButton | Qt::AltButton );

    if ( event )
    {
	if ( event->key() == Qt::Key_Print )
	{
	    YQUI::ui()->makeScreenShot( "" );
	    return;
	}
	else if ( event->key() == Qt::Key_F5 )	// No matter if Ctrl/Alt/Shift pressed
	{
	    YQUI::ui()->easterEgg();
	    return;
	}
	else if ( event->state() == 0 )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		( void ) activateDefaultButton();
		return;
	    }
	}
	else if ( ( event->state() & yast2_special_combo ) == yast2_special_combo )
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
	}
    }

    QWidget::keyPressEvent( event );
}


void YQDialog::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button ( and menu, e.g. Alt-F4 ) will be
    // handled just like the user had clicked on the `id`( `cancel ) button in
    // that dialog. It's up to the YCP application to handle this ( if desired ).

    y2debug( "Ignoring window manager close button." );
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void YQDialog::focusInEvent( QFocusEvent * event )
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
YQDialog::show()
{
    if ( ! hasDefaultSize()
	 && qApp->mainWidget()->isVisible() )
    {
	center( this, qApp->mainWidget() );
    }
    
    QWidget::show();
}


void
YQDialog::center( QWidget * dialog, QWidget * parent )
{
    if ( ! dialog )
	return;
    
    if ( ! parent )
    {
	if ( dialog == qApp->mainWidget() )
	    parent = qApp->desktop();
	else
	    parent = qApp->mainWidget();
    }

    QPoint pos( ( parent->width()  - dialog->width()  ) / 2,
		( parent->height() - dialog->height() ) / 2 );

    pos += parent->mapToGlobal( QPoint( 0, 0 ) );
    pos = dialog->mapToParent( dialog->mapFromGlobal( pos ) );
    dialog->move( pos );
}


void
YQDialog::childAdded( YWidget * child )
{

    ( (QWidget *) child->widgetRep() )->move ( decorationWidth( YD_HORIZ ),
					       decorationWidth( YD_VERT  ) );
}



#include "YQDialog.moc"
