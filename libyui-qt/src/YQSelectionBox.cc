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
#include "YUIQt.h"
#include "YQSelectionBox.h"
#include "YQDialog.h"

#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2
#define MIN_WIDTH			80
#define MIN_HEIGHT			80
#define SPACING				4	// between subwidgets
#define MARGIN				4	// around the widget


YQSelectionBox::YQSelectionBox( QWidget * parent, YWidgetOpt & opt,
			       const YCPString & label )
    : QVBox( parent )
    , YSelectionBox( opt, label )
{
    setWidgetRep( this );

    setSpacing( SPACING );
    setMargin( MARGIN );

    _qt_label = new QLabel( fromUTF8(label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );

    _qt_listbox = new QListBox( this );
    _qt_listbox->installEventFilter( this );
    _qt_listbox->setVariableHeight( false );
    _qt_listbox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_listbox->setTopItem(0);
    _qt_label->setBuddy( _qt_listbox );

    shrinkable 		= opt.isShrinkable.value();
    immediateMode	= opt.immediateMode.value();

    connect( _qt_listbox, SIGNAL( highlighted ( int ) ),
	     this, 	 SLOT  ( slotSelected( int ) ) );

    if ( getNotify() )
    {
	connect( &timer, SIGNAL( timeout() ),
		 this,	 SLOT  ( returnImmediately() ) );
    }
}


void YQSelectionBox::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
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
	int visibleLines	 = shrinkable ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
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
    _qt_listbox->insertItem( fromUTF8(string->value() ) );
    if ( selected ) _qt_listbox->setCurrentItem( index );
}


int YQSelectionBox::getCurrentItem()
{
    return _qt_listbox->currentItem();
}


void YQSelectionBox::setCurrentItem( int index )
{
    _qt_listbox->setCurrentItem( index );
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

    return QWidget::eventFilter( obj, ev );
}


void YQSelectionBox::slotSelected( int index )
{
    if ( getNotify() )
    {
	if ( immediateMode )	returnImmediately();
	else			returnDelayed();
    }
}


void YQSelectionBox::returnImmediately()
{
    YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


void YQSelectionBox::returnDelayed()
{
    timer.start( 250, true ); // millisec, singleShot
}


#include "YQSelectionBox.moc.cc"
