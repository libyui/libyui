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

  File:	      YQSelectionBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qstring.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qnamespace.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YEvent.h"
#include "YQUI.h"
#include "YQSelectionBox.h"
#include "YQSignalBlocker.h"
#include "YQDialog.h"
#include "YUIException.h"
#include "YQWidgetCaption.h"

#define VERBOSE_SELECTION		1

#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2


YQSelectionBox::YQSelectionBox( YWidget * parent, const string & label )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YSelectionBox( parent, label )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );

    _qt_listBox = new QListBox( this );
    YUI_CHECK_NEW( _qt_listBox );

    _qt_listBox->installEventFilter( this );
    _qt_listBox->setVariableHeight( false );
    _qt_listBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
					     QSizePolicy::Expanding ) );
    _qt_listBox->setTopItem(0);
    _caption->setBuddy( _qt_listBox );

    connect( _qt_listBox,	SIGNAL( highlighted ( int ) ),
	     this,		SLOT  ( slotSelected( int ) ) );

    connect( _qt_listBox,	SIGNAL( doubleClicked( QListBoxItem * ) ),
	     this,		SLOT  ( slotActivated( QListBoxItem * ) ) );

    connect( &_timer,		SIGNAL( timeout()           ),
	     this,		SLOT  ( returnImmediately() ) );
}


YQSelectionBox::~YQSelectionBox()
{
    // NOP
}


void YQSelectionBox::setLabel( const string & label )
{
    _caption->setText( label );
    YSelectionBox::setLabel( label );
}


void YQSelectionBox::addItem( YItem * item )
{
    YSelectionBox::addItem( item );
    QPixmap icon;

    if ( item->hasIconName() )
    {
	string iconName = iconFullPath( item );
	icon = QPixmap( iconName.c_str() );

	if ( icon.isNull() )
	    y2warning( "Can't load icon %s", iconName.c_str() );
    }

    if ( icon.isNull() )
	_qt_listBox->insertItem( fromUTF8( item->label() ) );
    else
	_qt_listBox->insertItem( icon, fromUTF8( item->label() ) );

    if ( item->selected() )
    {
	YQSignalBlocker sigBlocker( _qt_listBox );
	_qt_listBox->setCurrentItem( item->index() );
    }
}


void YQSelectionBox::selectItem( YItem * item, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listBox );

    YSelectionBox::selectItem( item, selected );
    _qt_listBox->setCurrentItem( selected ? item->index() : -1 );
}


void YQSelectionBox::selectItem( int index )
{
    YSelectionBox::deselectAllItems();
    YItem * item = YSelectionBox::itemAt( index );

    if ( item )
    {
#ifdef VERBOSE_SELECTION
	y2debug( "%s \"%s\": Selecting item \"%s\"",
		 widgetClass(),
		 debugLabel().c_str(),
		 item->label().c_str() );
#endif

	item->setSelected( true );
    }
    else
	YUI_THROW( YUIException( "Can't find selected item" ) );
}


void YQSelectionBox::deselectAllItems()
{
    YSelectionBox::deselectAllItems();
    _qt_listBox->clearSelection();

    if ( _qt_listBox->currentItem() > -1 )
    {
	// Some item is selected after all; the Qt documtation says this
	// happens if the QListBox is in single selection mode (which it is)
	// and has the keyboard focus.
	//
	// Synchronize internal "selected" flags with what the QListBox
	// displays. This has a small performance penalty because it calls
	// YSelectionBox::deselectAllItems() again which again iterates over
	// all items.
	selectItem( _qt_listBox->currentItem() );
    }
}


void YQSelectionBox::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listBox );

    _qt_listBox->clear();
    YSelectionBox::deleteAllItems();
}



int YQSelectionBox::preferredWidth()
{
    int hintWidth = _caption->isShown() ?
	_caption->sizeHint().width() + frameWidth() : 0;

    return max( 80, hintWidth );
}


int YQSelectionBox::preferredHeight()
{
    int hintHeight	 = _caption->isShown() ? _caption->sizeHint().height() : 0;
    int visibleLines	 = shrinkable() ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
    hintHeight 		+= visibleLines * _qt_listBox->fontMetrics().lineSpacing();
    hintHeight		+= _qt_listBox->frameWidth() * 2;

    return max( 80, hintHeight );
}


void YQSelectionBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQSelectionBox::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_listBox->setEnabled( enabled );
    _qt_listBox->triggerUpdate( true );
    YWidget::setEnabled( enabled );
}


bool YQSelectionBox::setKeyboardFocus()
{
    _qt_listBox->setFocus();

    return true;
}


bool YQSelectionBox::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * event = ( QKeyEvent * ) ev;

	if ( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
	     ( event->state() == 0 || event->state() == Qt::Keypad ) )
	{
	    YQDialog * dia = (YQDialog *) findDialog();

	    if ( dia )
	    {
		( void ) dia->activateDefaultButton();
		return true;
	    }
	}
    }
    else if ( ev->type() == QEvent::MouseButtonRelease )
    {
	QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (ev);

	if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
	{
	    y2milestone( "Right click in selecton box detected" );
	    YQUI::ui()->maybeLeftHandedUser();
	}
    }

    return QWidget::eventFilter( obj, ev );
}


void YQSelectionBox::slotSelected( int index )
{
    selectItem( index );

    if ( notify() )
    {
	if ( immediateMode() )
	    returnImmediately();
	else
	{
	    if ( ! YQUI::ui()->eventsBlocked() )
	    {
		// Delayed event delivery - only if events are to be delivered
		// right now.
		//
		// An event block that is in effect right now may or may not
		// affect events after the timer delay is expired.
		//
		// This may create nasty side effects such as bug #32510: When
		// an item is initially selected, that initial selection event
		// gets through even though (!) events are blocked during
		// widget creation.

		returnDelayed();
	    }
	}
    }
}


void YQSelectionBox::slotActivated( QListBoxItem * qItem )
{
    selectItem( _qt_listBox->index( qItem ) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


void YQSelectionBox::returnImmediately()
{
    if ( ! YQUI::ui()->eventPendingFor( this ) )
    {
	// Avoid overwriting a (more important) Activated event with a
	// SelectionChanged event

	y2debug( "sending selbox event" );
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
    }
}


void YQSelectionBox::returnDelayed()
{
    y2debug( "Starting selbox timer" );
    _timer.start( 250, true ); // millisec, singleShot
}



#include "YQSelectionBox.moc"
