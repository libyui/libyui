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

  File:	      YQMultiSelectionBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <limits.h>
#include <qstring.h>
#include <qlabel.h>
#include <qheader.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YUIQt.h"
#include "YQMultiSelectionBox.h"

#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2
#define MIN_WIDTH			80
#define MIN_HEIGHT			80
#define SPACING				4	// between subwidgets
#define MARGIN				4	// around the widget


YQMultiSelectionBox::YQMultiSelectionBox( QWidget *		parent,
					  YWidgetOpt &		opt,
					  const YCPString &	label )
    : QVBox( parent )
    , YMultiSelectionBox( opt, label )
{
    setWidgetRep( this );

    setSpacing( SPACING );
    setMargin( MARGIN );

    _qt_label = new QLabel( fromUTF8(label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YUIQt::ui()->currentFont() );

    _qt_listview = new QListView( this );
    _qt_listview->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_listview->addColumn( "" );	// we need at least one column - yes, QListView is too dumb to do without that. :-(
    _qt_listview->setSorting( 0, false );
    _qt_listview->header()->hide();
    _qt_label->setBuddy( _qt_listview );

    // Very small default size if specified
    _shrinkable = opt.isShrinkable.value();

    connect( _qt_listview, SIGNAL( selectionChanged() ),
	     this, 	  SLOT  ( slotSelected() ) );
}


void
YQMultiSelectionBox::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
    YMultiSelectionBox::setLabel( label );
}


long
YQMultiSelectionBox::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	int hintWidth = _qt_label->sizeHint().width() + frameWidth();

	return max( MIN_WIDTH, hintWidth );
    }
    else
    {
	int hintHeight	 	 = _qt_label->sizeHint().height();
	int visibleLines	 = _shrinkable ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
	hintHeight 		+= visibleLines * _qt_listview->fontMetrics().lineSpacing();
	hintHeight		+= _qt_listview->frameWidth() * 2;

	return max( MIN_HEIGHT, hintHeight );
    }
}


void
YQMultiSelectionBox::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void
YQMultiSelectionBox::setEnabling( bool enabled )
{
    _qt_label->setEnabled( enabled );
    _qt_listview->setEnabled( enabled );
    _qt_listview->triggerUpdate();
}


void
YQMultiSelectionBox::itemAdded( const YCPString & label, bool selected )
{
    YQMultiSelectionBoxItem * item = new YQMultiSelectionBoxItem( _qt_listview, fromUTF8( label->value() ) );
    
    if ( item && selected )
    {
	item->setOn( true );
    }
}


int
YQMultiSelectionBox::getCurrentItem()
{
    int index = 0;
    QListViewItem * child = _qt_listview->firstChild();

    while ( child )
    {
	if ( child->isSelected() )	// don't confuse this with isOn() !
	{
	    return index;
	}
	index++;
	child = child->nextSibling();
    }
    
    return 0;
}


void
YQMultiSelectionBox::setCurrentItem( int index )
{
    _qt_listview->clearSelection();
    QListViewItem * child = _qt_listview->firstChild();

    for ( int i = 0; i < index; i++ )
    {
	child = child->nextSibling();

	if ( ! child )
	{
	    y2error( "Don't have that many items" );
	    return;
	}
    }

    child->setSelected( true );
}


bool
YQMultiSelectionBox::setKeyboardFocus()
{
    _qt_listview->setFocus();

    return true;
}


void
YQMultiSelectionBox::deleteAllItems()
{
    _qt_listview->clear();
    YMultiSelectionBox::deleteAllItems();
}


bool
YQMultiSelectionBox::itemIsSelected( int index )
{
    QListViewItem * child = _qt_listview->firstChild();

    for ( int i = 0; i < index; i++ )
    {
	child = child->nextSibling();

	if ( ! child )
	{
	    y2error( "Don't have that many items" );
	    return false;
	}
    }

    QCheckListItem * item = ( QCheckListItem * ) child;

    return item->isOn();
}


void
YQMultiSelectionBox::selectItem( int index )
{
    QListViewItem * child = _qt_listview->firstChild();

    for ( int i = 0; i < index; i++ )
    {
	child = child->nextSibling();

	if ( ! child )
	{
	    y2error( "Don't have that many items" );
	    return;
	}
    }

    QCheckListItem * item = ( QCheckListItem * ) child;

    item->setOn( true );
}


void
YQMultiSelectionBox::deselectAllItems()
{
    QListViewItem * child = _qt_listview->firstChild();

    while ( child )
    {
	QCheckListItem * item = ( QCheckListItem * ) child;
	item->setOn( false );
	child = child->nextSibling();
    }
}


void YQMultiSelectionBox::slotSelected()
{
    if ( getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}




int YQMultiSelectionBoxItem::_item_count = 0;


YQMultiSelectionBoxItem::YQMultiSelectionBoxItem( QListView * 		parent,
						  const QString &	text )
    : QCheckListItem( parent, text, QCheckListItem::CheckBox )
{
    _serial = _item_count++;
}


QString
YQMultiSelectionBoxItem::key( int, bool ) const
{
    /*
     * Return a sort key that depends on creation ( i.e. insertion ) order.
     */
    
    static QString sortKey;
    sortKey.sprintf( "%010d", INT_MAX - _serial );

    return sortKey;
}


#include "YQMultiSelectionBox.moc.cc"
