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

  File:	      YQPkgPatternList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <QRegExp>
#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>
#include <QPainter>
#include <QHeaderView>
#include <zypp/ui/PatternContents.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgPatternList.h"
#include "YQIconPool.h"
#include "YQApplication.h"
#include "YQUI.h"


#define CATEGORY_BACKGROUND	QColor( 0xFF, 0xC0, 0x50 )


YQPkgPatternList::YQPkgPatternList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    y2debug( "Creating pattern list" );

    int numCol = 0;
    QStringList headers;
    headers << "";	_statusCol	= numCol++;

    // Translators: "Pattern" refers to so-called "installation patterns",
    // i.e., specific task-oriented groups of packages, like "everything that
    // is needed to run a web server". The idea of patterns is that they also
    // include the configuration workflow needed for that task, such of
    // configuring the web server. For the scope of the package selector, this
    // is only of little relevance, though.

    headers << _( "Pattern" );	_summaryCol	= numCol++;

    setHeaderLabels(headers);
    // Can use the same colum for "broken" and "satisfied":
    // Both states are mutually exclusive

    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;

//     header()->setStretchEnabled( _statusCol , false );
//     header()->setStretchEnabled( _summaryCol, true  );

    setAllColumnsShowFocus( true );
    //setTreeStepSize( 0 );

    if ( autoFilter )
    {
	connect( this, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ),
		 this, SLOT  ( filter()				   ) );
    }

    if ( autoFill )
    {
	fillList();
	selectSomething();
    }
    y2debug( "Creating pattern list done" );
}


YQPkgPatternList::~YQPkgPatternList()
{
    // NOP
}


void
YQPkgPatternList::fillList()
{
    clear();
    y2debug( "Filling pattern list" );


    for ( ZyppPoolIterator it = zyppPatternsBegin();
	  it != zyppPatternsEnd();
	  ++it )
    {
	ZyppPattern zyppPattern = tryCastToZyppPattern( (*it)->theObj() );

	if ( zyppPattern )
	{
	    if ( zyppPattern->userVisible() )
	    {
		addPatternItem( *it, zyppPattern );
	    }
	    else
		y2debug( "Pattern %s is not user-visible", zyppPattern->name().c_str() );
	}
	else
	{
	    y2error( "Found non-Pattern selectable" );
	}
    }

    y2debug( "Pattern list filled" );
}


YQPkgPatternCategoryItem *
YQPkgPatternList::category( const QString & categoryName )
{
    if ( categoryName.isEmpty() )
	return 0;

    YQPkgPatternCategoryItem * cat = _categories[ categoryName ];

    if ( ! cat )
    {
	y2debug( "New pattern category \"%s\"", qPrintable(categoryName) );

	cat = new YQPkgPatternCategoryItem( this, categoryName );
	Q_CHECK_PTR( cat );
	_categories.insert( categoryName, cat );
    }


    return cat;
}


void
YQPkgPatternList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgPatternList::filter()
{
    emit filterStart();

    if ( selection() )	// The seleted QListViewItem
    {
	ZyppPattern zyppPattern = selection()->zyppPattern();

	if ( zyppPattern )
	{
	    zypp::ui::PatternContents patternContents( zyppPattern );
	    set<string> wanted = patternContents.install_packages();

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		string name = (*it)->theObj()->name();

		if ( contains( wanted, name ) )
		{
		    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );

		    if ( zyppPkg )
		    {
			emit filterMatch( *it, zyppPkg );
		    }
		}
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgPatternList::addPatternItem( ZyppSel	selectable,
				  ZyppPattern	zyppPattern )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSelectable!" );
	return;
    }

    YQPkgPatternCategoryItem * cat = category( fromUTF8( zyppPattern->category() ) );
    YQPkgPatternListItem * item = 0;

    if ( cat )
	item = new YQPkgPatternListItem( this, cat, selectable, zyppPattern );
    else
	item = new YQPkgPatternListItem( this, selectable, zyppPattern );

    addTopLevelItem(item);
    applyExcludeRules( item );
}


YQPkgPatternListItem *
YQPkgPatternList::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatternListItem *> (item);
}


void
YQPkgPatternList::pkgObjClicked( int			button,
				 QTreeWidgetItem *	listViewItem,
				 int			col,
				 const QPoint &		pos )
{
    YQPkgPatternCategoryItem * categoryItem
	= dynamic_cast<YQPkgPatternCategoryItem *> (listViewItem);

    if ( categoryItem )
    {
	if ( button == Qt::LeftButton )
	{
	    if ( col == statusCol() )
	    {
		categoryItem->setExpanded( ! categoryItem->isExpanded() );
	    }
	}
    }
    else
    {
	YQPkgObjList::pkgObjClicked( button, listViewItem, col, pos );
    }
}


void
YQPkgPatternList::selectSomething()
{
#if FIXME
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);
	YQPkgPatternCategoryItem * categoryItem =
	    dynamic_cast<YQPkgPatternCategoryItem *> (*it);

	if ( item && item->isSelectable() && ! categoryItem )
	{
	    setSelected( item, true ); // emits signal, too
	    return;
	}

	++it;
    }
#endif
}

void YQPkgPatternList::drawRow ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->setFont( YQUI::yqApp()->headingFont() );
    QTreeWidget::drawRow ( painter, option, index );
}

YQPkgPatternListItem::YQPkgPatternListItem( YQPkgPatternList *	patternList,
					    ZyppSel		selectable,
					    ZyppPattern		zyppPattern )
    : YQPkgObjListItem( patternList, selectable, zyppPattern )
    , _patternList( patternList )
    , _zyppPattern( zyppPattern )
{
    init();
}


YQPkgPatternListItem::YQPkgPatternListItem( YQPkgPatternList *		patternList,
					    YQPkgPatternCategoryItem *	parentCategory,
					    ZyppSel			selectable,
					    ZyppPattern			zyppPattern )
    : YQPkgObjListItem( patternList, parentCategory, selectable, zyppPattern )
    , _patternList( patternList )
    , _zyppPattern( zyppPattern )
{
    init();
    parentCategory->addPattern( _zyppPattern );
}


void
YQPkgPatternListItem::init()
{
    if ( ! _zyppPattern )
	_zyppPattern = tryCastToZyppPattern( selectable()->theObj() );

    setStatusIcon();
}



YQPkgPatternListItem::~YQPkgPatternListItem()
{
    // NOP
}


void
YQPkgPatternListItem::applyChanges()
{
    solveResolvableCollections();
}


bool YQPkgPatternListItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPatternListItem * otherPatternListitem	 = dynamic_cast<const YQPkgPatternListItem     *>(&otherListViewItem);

    if ( _zyppPattern && otherPatternListitem && otherPatternListitem->zyppPattern() )
	return _zyppPattern->order().compare( otherPatternListitem->zyppPattern()->order() );

    const YQPkgPatternCategoryItem * otherCategoryItem = dynamic_cast<const YQPkgPatternCategoryItem *>(&otherListViewItem);

    if ( otherCategoryItem )	// Patterns without category should always be sorted
	return true;		// before any category

    return QTreeWidgetItem::operator<( otherListViewItem );
}






YQPkgPatternCategoryItem::YQPkgPatternCategoryItem( YQPkgPatternList *	patternList,
						    const QString &	category	)
    : QY2ListViewItem( patternList )
    , _patternList( patternList )
{
    setText( _patternList->summaryCol(), category );
    setBackgroundColor( CATEGORY_BACKGROUND );
    setExpanded( true );
    setTreeIcon();
}


YQPkgPatternCategoryItem::~YQPkgPatternCategoryItem()
{
    // NOP
}

void
YQPkgPatternCategoryItem::addPattern( ZyppPattern pattern )
{
    if ( ! _firstPattern )
    {
	_firstPattern = pattern;
    }
    else
    {
	if ( _firstPattern->order().compare( pattern->order() ) < 0 )
	    _firstPattern = pattern;
    }
}


void
YQPkgPatternCategoryItem::setExpanded( bool open )
{
    QTreeWidgetItem::setExpanded( open );
    setTreeIcon();
}


void
YQPkgPatternCategoryItem::setTreeIcon()
{
    setData( _patternList->statusCol(), Qt::DecorationRole,
	       isExpanded() ?
	       YQIconPool::treeMinus() :
	       YQIconPool::treePlus()   );

}


bool YQPkgPatternCategoryItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPatternCategoryItem * otherCategoryItem = dynamic_cast<const YQPkgPatternCategoryItem *>(&otherListViewItem);

    if ( _firstPattern && otherCategoryItem && otherCategoryItem->firstPattern() )
	return _firstPattern->order().compare( otherCategoryItem->firstPattern()->order() );


    const YQPkgPatternListItem * otherPatternListitem	 = dynamic_cast<const YQPkgPatternListItem *>(&otherListViewItem);

    if ( otherPatternListitem )	// Patterns without category should always be sorted
	return true;		// before any category

    return QTreeWidgetItem::operator<( otherListViewItem );
}



#include "YQPkgPatternList.moc"
