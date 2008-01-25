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

#include <QString>
#include <QLabel>
#include <QListWidget>
#include <qnamespace.h>
#include <QPixmap>
#include <QKeyEvent>
#include <QVBoxLayout>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

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
    : QFrame( (QWidget *) parent->widgetRep() )
    , YSelectionBox( parent, label )
{
    setWidgetRep( this );

    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin	);

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_listBox = new QListWidget( this );
    YUI_CHECK_NEW( _qt_listBox );
    layout->addWidget( _qt_listBox );

    _qt_listBox->installEventFilter( this );
    //FIXME _qt_listBox->setVariableHeight( false );
    _qt_listBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
					     QSizePolicy::Expanding ) );
    //FIXME _qt_listBox->setTopItem(0);
    _caption->setBuddy( _qt_listBox );

    connect( _qt_listBox,	SIGNAL( itemSelectionChanged() ),
	     this,		SLOT  ( slotSelectionChanged() ) );

    connect( _qt_listBox,	SIGNAL( itemDoubleClicked( QListWidgetItem * ) ),
	     this,		SLOT  ( slotActivated( QListWidgetItem * ) ) );

    connect( &_timer,		SIGNAL( timeout()	    ),
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
	    yuiWarning() << "Can't load icon " << iconName << endl;
    }

    if ( icon.isNull() )
    {
      _qt_listBox->addItem( fromUTF8( item->label() ) );
    }
    else
    {
      QListWidgetItem *i = new QListWidgetItem(_qt_listBox);
      i->setData(Qt::DisplayRole, fromUTF8( item->label() ) );
      i->setData(Qt::DecorationRole, icon );
      _qt_listBox->addItem( i );
    }

    if ( item->selected() )
    {
	YQSignalBlocker sigBlocker( _qt_listBox );
	_qt_listBox->setCurrentItem( _qt_listBox->item( item->index() ) );
    }
}


void YQSelectionBox::selectItem( YItem * item, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listBox );

    YSelectionBox::selectItem( item, selected );
    _qt_listBox->setCurrentRow( selected ? item->index() : -1 );
}


void YQSelectionBox::selectItem( int index )
{
    YSelectionBox::deselectAllItems();
    YItem * item = YSelectionBox::itemAt( index );

    if ( item )
    {
#ifdef VERBOSE_SELECTION
	yuiDebug() << this << ": Selecting item \"" << item->label() << "\"" << endl;
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

    if ( _qt_listBox->currentRow() > -1 )
    {
	// Some item is selected after all; the Qt documtation says this
	// happens if the QListBox is in single selection mode (which it is)
	// and has the keyboard focus.
	//
	// Synchronize internal "selected" flags with what the QListBox
	// displays. This has a small performance penalty because it calls
	// YSelectionBox::deselectAllItems() again which again iterates over
	// all items.
	selectItem( _qt_listBox->row(_qt_listBox->currentItem()) );
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
    int hintWidth = !_caption->isHidden() ?
	_caption->sizeHint().width() + frameWidth() : 0;

    return max( 80, hintWidth );
}


int YQSelectionBox::preferredHeight()
{
    int hintHeight	 = !_caption->isHidden() ? _caption->sizeHint().height() : 0;
    int visibleLines	 = shrinkable() ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
    hintHeight		+= visibleLines * _qt_listBox->fontMetrics().lineSpacing();
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
    //FIXME needed? _qt_listBox->triggerUpdate( true );
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
	     ( (event->modifiers() & Qt::NoModifier) || (event->modifiers() & Qt::KeypadModifier) ) )
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
	    yuiMilestone() << "Right click in selecton box detected" << endl;
	    YQUI::ui()->maybeLeftHandedUser();
	}
    }

    return QWidget::eventFilter( obj, ev );
}


void YQSelectionBox::slotSelectionChanged()
{
    QList<QListWidgetItem *> items = _qt_listBox->selectedItems ();
    selectItem( _qt_listBox->row( items.first() ) );

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


void YQSelectionBox::slotActivated( QListWidgetItem * qItem )
{
    selectItem( _qt_listBox->row( qItem ) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


void YQSelectionBox::returnImmediately()
{
    if ( ! YQUI::ui()->eventPendingFor( this ) )
    {
	// Avoid overwriting a (more important) Activated event with a
	// SelectionChanged event

	yuiDebug() << "Sending selbox event" << endl;
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
    }
}


void YQSelectionBox::returnDelayed()
{
    yuiDebug() << "Starting selbox timer" << endl;
    _timer.setSingleShot( true );
    _timer.start( 250 ); // millisec
}



#include "YQSelectionBox.moc"
