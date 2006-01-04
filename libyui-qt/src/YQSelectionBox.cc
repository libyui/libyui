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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

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
#include "YQDialog.h"


#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2
#define MIN_WIDTH			80
#define MIN_HEIGHT			80

#define ICON_DIR			ICONDIR "/icons/22x22/apps/"


YQSelectionBox::YQSelectionBox( QWidget *		parent,
				const YWidgetOpt &	opt,
				const YCPString &	label )
    : QVBox( parent )
    , YSelectionBox( opt, label )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8(label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( _qt_label->text().isEmpty() )
	_qt_label->hide();

    _qt_listbox = new QListBox( this );
    _qt_listbox->installEventFilter( this );
    _qt_listbox->setVariableHeight( false );
    _qt_listbox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_listbox->setTopItem(0);
    _qt_label->setBuddy( _qt_listbox );

    _shrinkable 	= opt.isShrinkable.value();
    _immediateMode	= opt.immediateMode.value();

    if ( getNotify() )
    {
	connect( _qt_listbox,	SIGNAL( highlighted ( int ) ),
		 this,		SLOT  ( slotSelected( int ) ) );

	connect( _qt_listbox,	SIGNAL( doubleClicked( QListBoxItem * ) ),
		 this,		SLOT  ( slotActivated( QListBoxItem * ) ) );

	connect( &_timer,	SIGNAL( timeout()           ),
		 this,		SLOT  ( returnImmediately() ) );
    }
}


void YQSelectionBox::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );

    if ( _qt_label->text().isEmpty() )
    {
	if ( _qt_label->isVisible() )
	    _qt_label->hide();
    }
    else
    {
	if ( _qt_label->isHidden() )
	    _qt_label->show();
    }
    
    YSelectionBox::setLabel( label );
}


long YQSelectionBox::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	int hintWidth = _qt_label->sizeHint().width() + frameWidth();

	return max( MIN_WIDTH, hintWidth );
    }
    else
    {
	int hintHeight	 = _qt_label->sizeHint().height();
	int visibleLines = _shrinkable ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
	hintHeight 	+= visibleLines * _qt_listbox->fontMetrics().lineSpacing();
	hintHeight	+= _qt_listbox->frameWidth() * 2;

	return max( MIN_HEIGHT, hintHeight );
    }
}


void YQSelectionBox::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQSelectionBox::setEnabling( bool enabled )
{
    _qt_label->setEnabled( enabled );
    _qt_listbox->setEnabled( enabled );
    _qt_listbox->triggerUpdate( true );
}


void YQSelectionBox::itemAdded( const YCPString & string, int index, bool selected )
{
    QPixmap icon;
    
    if ( hasIcons() )
    {
	QString iconName( itemIcon( index )->value().c_str() );
	iconName.stripWhiteSpace();

	if ( ! iconName.isEmpty() )
	{
	    if ( ! iconName.startsWith( "/" ) )
		iconName.prepend( ICON_DIR );

	    icon = QPixmap( iconName );

	    if ( icon.isNull() )
		y2error( "Can't load icon %s", (const char *) iconName );
	}
	    
    }

    if ( icon.isNull() )
    {
	_qt_listbox->insertItem( fromUTF8(string->value() ) );
    }
    else
    {
	_qt_listbox->insertItem( icon, fromUTF8(string->value() ) );
    }
    
    if ( selected )
	setCurrentItem( index );
}


int YQSelectionBox::getCurrentItem()
{
    return _qt_listbox->currentItem();
}


void YQSelectionBox::setCurrentItem( int index )
{
    _qt_listbox->blockSignals( true );
    _qt_listbox->setCurrentItem( index );
    _qt_listbox->blockSignals( false );
}


bool YQSelectionBox::setKeyboardFocus()
{
    _qt_listbox->setFocus();

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
	    YQDialog * dia = ( YQDialog * ) yDialog();

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
    if ( _immediateMode )
	returnImmediately();
    else
    {
	if ( ! YQUI::ui()->eventsBlocked() )
	{
	    // Delayed event delivery - only if events are to be delivered right now.
	    //
	    // An event block that is in effect right now may or may not affect
	    // events after the timer delay is expired.
	    //
	    // This may create nasty side effects such as bug #32510: When an
	    // item is initially selected, that initial selection event gets
	    // through even though (!) events are blocked during widget
	    // creation.

	    returnDelayed();
	}
    }
}


void YQSelectionBox::slotActivated( QListBoxItem * )
{
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


void YQSelectionBox::returnImmediately()
{
    if ( ! YQUI::ui()->eventPendingFor( this ) )
    {
	// Avoid overwriting a (more important) Activated event with a SelectionChanged event

	y2milestone( "sending selbox event" );
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
    }
}


void YQSelectionBox::returnDelayed()
{
    y2milestone( "Starting selbox timer" );
    _timer.start( 250, true ); // millisec, singleShot
}


void YQSelectionBox::deleteAllItems()
{
    _qt_listbox->blockSignals( true );
    _qt_listbox->clear();
    YSelectionWidget::deleteAllItems();
    _qt_listbox->blockSignals( false );
}




#include "YQSelectionBox.moc"
