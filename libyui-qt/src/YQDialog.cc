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

  File:	      YQDialog.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <X11/Xlib.h>

#include "YUIQt.h"
#include "YQDialog.h"


YQDialog::YQDialog( YUIQt *		yuiqt,
		    YWidgetOpt &	opt,
		    QWidget *		qt_parent,
		    bool		default_size )
    : QWidget( qt_parent,
	       0,	// name
	       default_size ? 0 : WType_Modal | WStyle_Dialog )
    , YDialog(opt)
    , yuiqt(yuiqt)
    , penguins(false)
    , _userResized(false)
{
    setWidgetRep(this);
    setCaption( hasDefaultSize() ? "YaST2" : "");
    setFocusPolicy( QWidget::StrongFocus );

    if ( hasWarnColor() || hasInfoColor() )
    {
	QColor normalBackground	  ( 0, 128, 0 );
	QColor inputFieldBackground ( 0,	96, 0 );
	QColor text = white;

	if ( hasInfoColor() )
	{
	    normalBackground = QColor (238, 232, 170); // PaleGoldenrod
	}

	QPalette warnPalette( normalBackground );
	QColorGroup normalColors = warnPalette.normal();
	normalColors.setColor(QColorGroup::Text, text );
	normalColors.setColor(QColorGroup::Base, inputFieldBackground );
	warnPalette.setNormal(normalColors);
	setPalette(warnPalette);
    }

    qt_frame = new QFrame ( this );

    if ( ! hasDefaultSize() && ! yuiqt->hasWM() )
    {
	qt_frame->setFrameStyle ( QFrame::Box | QFrame::Raised );
	qt_frame->setLineWidth( 2 );
	qt_frame->setMidLineWidth( 3 );
    }
    else
    {
	qt_frame->setFrameStyle ( QFrame::NoFrame );
    }
}


YQDialog::~YQDialog()
{
    if ( penguins )
    {
	system("killall xpenguins >/dev/null 2>&1");
    }
}


long YQDialog::nicesize(YUIDimension dim)
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
	    nice = yuiqt->defaultSize(dim);
	}
    }
    else
    {
	nice = YDialog::nicesize(dim) + 2 * decorationWidth(dim);
    }

    long screenSize = dim == YD_HORIZ ? qApp->desktop()->width() : qApp->desktop()->height();

    if ( nice > screenSize )
    {
	y2warning( "Limiting dialog size to screen size (%ld) instead of %ld - check the layout!",
		   screenSize, nice );
    }

    return nice;
}


long YQDialog::decorationWidth(YUIDimension dim)
{
    if ( ! hasDefaultSize() && qt_frame )
	return qt_frame->frameWidth();
    else
	return 0L;
}


void YQDialog::setSize(long newWidth, long newHeight)
{
    y2debug("Resizing dialog to %ld %ld", newWidth, newHeight);

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


    YContainerWidget::child(0)->setSize (newWidth  - 2 * decorationWidth( YD_HORIZ ),
					 newHeight - 2 * decorationWidth( YD_VERT  ) );
    if ( qt_frame )
	qt_frame->resize(newWidth, newHeight);

    resize( newWidth, newHeight );
}



void YQDialog::activate(bool active)
{
    if (active)
    {
	if ( ! yuiqt->hasWM() )
	{
	    if ( yuiqt->autoActivateDialogs() )
		setActiveWindow();
	    else
		y2milestone( "Auto-activating dialog window turned off" );
	}

	ensureOnlyOneDefaultButton();
    }
}


void
YQDialog::resizeEvent ( QResizeEvent *event )
{
    if ( event )
    {
	setSize ( event->size().width(), event->size().height() );
	_userSize = event->size();
	_userResized = true;
    }
}


QPushButton *
YQDialog::findDefaultButton()
{
    QObjectList *list = queryList( "QPushButton" );
    QObjectListIt it( *list );
    QPushButton *button = 0;

    while ( ( button = (QPushButton *) it.current() ) )
    {
	if ( button->isDefault() && button->isVisible() )
	{
	    delete list;
	    return button;
	}
	++it;
    }
    delete list;

    return 0;
}


void
YQDialog::ensureOnlyOneDefaultButton()
{
    QObjectList *list = queryList( "QPushButton" );
    QObjectListIt it( *list );
    QPushButton *button = 0;
    bool have_default_button = false;

    while ( ( button = (QPushButton *) it.current() ) )
    {
	if ( button->isDefault() )
	{
	    if ( have_default_button )
	    {
		y2error( "Too many `opt(`default) PushButtons: \"%s\"",
			 (const char *) button->text() );
	    }

	    button->setDefault( ! have_default_button );
	    have_default_button = true;
	}
	++it;
    }
    delete list;
}


void
YQDialog::makeDefaultButton( QPushButton *new_default_button )
{
    QObjectList *list = queryList( "QPushButton" );
    QObjectListIt it( *list );
    QPushButton *button = 0;

    while ( ( button = (QPushButton *) it.current() ) )
    {
	button->setDefault( button == new_default_button );
	++it;
    }
    delete list;
}


bool
YQDialog::activateDefaultButton( bool warn )
{
    QPushButton * default_button = findDefaultButton();

    if ( default_button && default_button->isEnabled() )
    {
	default_button->animateClick();
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
YQDialog::keyPressEvent( QKeyEvent *event )
{
    unsigned yast2_special_combo = ( Qt::ControlButton | Qt::ShiftButton | Qt::AltButton );

    if ( event )
    {
	if ( event->key() == Qt::Key_Print )
	{
	    yuiqt->makeScreenShot( "" );
	    return;
	}
	else if ( event->state() == 0 )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		(void) activateDefaultButton();
		return;
	    }
	}
	else if ( ( event->state() & yast2_special_combo ) == yast2_special_combo )
	{
	    // Qt-UI special keys - all with Ctrl-Shift-Alt

	    y2milestone( "Caught YaST2 magic key combination" );

	    if ( event->key() == Qt::Key_M )
	    {
		yuiqt->toggleRecordMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_P )
	    {
		yuiqt->askPlayMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_D )
	    {
		yuiqt->returnNow(YUIInterpreter::ET_DEBUG, 0);
		return;
	    }
	}
    }

    QWidget::keyPressEvent( event );
}


void YQDialog::closeEvent(QCloseEvent *event)
{
    // The window manager "close window" button (and menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the `id`(`cancel) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    y2debug("Ignoring window manager close button.");
    event->ignore();
    yuiqt->returnNow(YUIInterpreter::ET_CANCEL, 0);
}


void YQDialog::focusInEvent( QFocusEvent *event)
{

    // The dialog itself doesn't need or want the keyboard focus, but obviously
    // (since Qt 2.3?) it needs QFocusPolicy::StrongFocus for the default
    // button mechanism to work. So let's accept the focus and give it to some
    // child widget.

    if ( event->reason() == QFocusEvent::Tab )
    {
	focusNextPrevChild( true );
    }
    else
    {
	QPushButton *default_button = findDefaultButton();

	if ( default_button )
	{
	    default_button->setFocus();
	}
	else
	{
	    focusNextPrevChild( true );
	}
    }
}


void YQDialog::show()
{
    if ( ! hasDefaultSize() )
    {
	// Center popup widgets relative to the main window

	QWidget *main_win = qApp->mainWidget();
	QPoint dia_pos( ( main_win->width()  - width()  ) / 2,
			( main_win->height() - height() ) / 2 );
	dia_pos += main_win->pos();
	dia_pos = mapToParent( mapFromGlobal( dia_pos ) );
	move( dia_pos );
    }
    QWidget::show();
}


void YQDialog::reject()
{
    // NOP - we don't want Escape to close the dialog
}


void
YQDialog::childAdded ( YWidget *child )
{

    ( (QWidget *) child->widgetRep() )->move ( decorationWidth( YD_HORIZ ),
					       decorationWidth( YD_VERT  ) );
}



#include "YQDialog.moc.cc"
