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

  File:	      YQTable.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

#include <qstring.h>
#include <qlistview.h>
#include <qheader.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQTable.h"



// Helper class. See Qt docu. We need an own subclass,
// because we want to store the index of the row.

class YQListViewItem : public QListViewItem
{
public:
    int index;

    /**
     * Constructor for auto-sorting
     **/
    YQListViewItem( YQTable *		table,
		    QListView *		list_view,
		    int 		index );

    /**
     * Constructor for maintaining insertion order.
     * 'after' is the item this one is to be inserted after.
     **/
    YQListViewItem( YQTable *		table,
		    QListView *		list_view,
		    YQListViewItem *	after,
		    int 		index );

protected:

    YQTable * table;
};


YQListViewItem::YQListViewItem( YQTable *	table,
				QListView *	list_view,
				int 		index )
    : QListViewItem( list_view )
    , index( index )
    , table( table )
{
}


YQListViewItem::YQListViewItem( YQTable *	 table,
				QListView *	 list_view,
				YQListViewItem * after,
				int 		 index )
    : QListViewItem( list_view, after )
    , index( index )
    , table( table )
{
}


YQTable::YQTable( QWidget * parent, YWidgetOpt & opt, vector<string> header )
    : QVBox( parent )
    , YTable( opt, header.size() )
    , last_item( 0 )
{
    sort_by_insertion_order = true;
    enable_user_sort = ! opt.keepSorting.value();
    setWidgetRep( this );
    setMargin( YQWIDGET_BORDER );

    qt_listview = new QListView(this);


    if ( ! enable_user_sort )
    {
	y2debug( "User sorting disabled" );
	qt_listview->setSorting( -1, true );	// disable sorting by click
    }
    else
    {
	connect( qt_listview->header(), SIGNAL( clicked ( int ) ),
		 this,			SLOT  ( userSort( int ) ) );
	qt_listview->setSorting( 65530, true ); // leave initial sorting
    }

    // The header is a vector of strings, each defining one
    // column. The first string is one of the characters L, R or C
    // denoting left, right or center justification, resp.

    for (unsigned c=0; c < header.size(); c++)
    {
	qt_listview->addColumn(fromUTF8(header[c].substr(1) ) );
	switch (header[c][0])
	{
	    case 'R':
		qt_listview->setColumnAlignment(c, AlignRight);
		break;
	    case 'C':
		qt_listview->setColumnAlignment(c, AlignCenter);
		break;
	    case 'L':
	    default:
		qt_listview->setColumnAlignment(c, AlignLeft);
		break;
	}
    }

    qt_listview->setFont(YUIQt::ui()->currentFont() );
    qt_listview->setAllColumnsShowFocus(true);

    if (opt.immediateMode.value() )
	connect( qt_listview, SIGNAL(selectionChanged ( QListViewItem *) ), this, SLOT(slotSelected(QListViewItem *) ) );
    else
	connect( qt_listview, SIGNAL(doubleClicked ( QListViewItem *) ), this, SLOT(slotSelected(QListViewItem *) ) );

    if (opt.notifyMode.value() )
	connect( qt_listview, SIGNAL(spacePressed ( QListViewItem *) ), this, SLOT(slotSelected(QListViewItem *) ) );
}


void YQTable::userSort( int column )	// column
{
    if ( enable_user_sort )
    {
	sort_by_insertion_order = false;
    }
}


long YQTable::nicesize(YUIDimension dim)
{
    // This value is for debugging only.
    // Use spacings to determine the size of this widget;
    // see the layout HOWTO for details.

    return 30;
}


void YQTable::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}


void YQTable::setEnabling(bool enabled)
{
    qt_listview->setEnabled(enabled);
    qt_listview->triggerUpdate();
}


void YQTable::itemAdded(vector<string> elements, int index)
{
    YQListViewItem * item;

    if ( sort_by_insertion_order && last_item )
	item = new YQListViewItem( this, qt_listview, last_item, index );
    else
	item = new YQListViewItem( this, qt_listview, index );

    last_item = item;
    
    for ( unsigned int i=0; i < elements.size(); i++ )
	item->setText( i, fromUTF8( elements[i] ) );

    if ( ! qt_listview->selectedItem() )
	item->setSelected( true );
}

void YQTable::itemsCleared()
{
    qt_listview->clear();
}


void YQTable::cellChanged(int index, int colnum, const YCPString & newtext)
{
    QListViewItem * item = findItem(index);
    if (item) item->setText(colnum, fromUTF8(newtext->value() ) );
}


int YQTable::getCurrentItem()
{
    YQListViewItem * ci = (YQListViewItem *)(qt_listview->currentItem() );
    if (ci) return ci->index;
    else	   return -1;
}


void YQTable::setCurrentItem(int index)
{
    QListViewItem * item = findItem(index);
    if (item)  // should be always true
    {
	qt_listview->setCurrentItem(item);
	qt_listview->ensureItemVisible(item);
    }
}


QListViewItem * YQTable::findItem(int index)
{
    // Scan all items and look for the one with the right index.
    QListViewItem * item = qt_listview->firstChild();
    while (item)
    {
	if ( ( (YQListViewItem *)item)->index == index)
	    return item;
	item = item->nextSibling();
    }
    return 0;
}


bool YQTable::setKeyboardFocus()
{
    qt_listview->setFocus();

    return true;
}


void YQTable::slotSelected(QListViewItem *)
{
    if (getNotify() ) YUIQt::ui()->returnNow(YUIInterpreter::ET_WIDGET, this);
}

#include "YQTable.moc.cc"
