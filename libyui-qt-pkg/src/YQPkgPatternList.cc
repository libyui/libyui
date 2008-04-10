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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <QRegExp>
#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>
#include <QPainter>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QItemDelegate>
#include <zypp/ui/PatternContents.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgPatternList.h"
#include "YQIconPool.h"
#include "YQApplication.h"
#include "YQUI.h"

using std::string;
using std::set;


#define CATEGORY_BACKGROUND	QColor( 0xFF, 0xC0, 0x50 )

class YQPkgPatternItemDelegate : public QItemDelegate
{
     YQPkgPatternList *_view;
public:
    YQPkgPatternItemDelegate( YQPkgPatternList *parent ) : QItemDelegate( parent ), _view( parent ) {
    }

    virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        painter->save();
        QColor background = option.palette.color(QPalette::Window);
        
        YQPkgPatternCategoryItem *citem = dynamic_cast<YQPkgPatternCategoryItem *>(_view->itemFromIndex(index));
        if ( citem )
        {
            QFont f = painter->font();
            f.setWeight(QFont::Bold);
            f.setPointSize(f.pointSize()+1);
            painter->setFont(f);
            
            painter->fillRect(option.rect, CATEGORY_BACKGROUND);
            //painter->setBackground(  );
            //_view->drawRow( painter, option, index  );
        }

        YQPkgPatternListItem *item = dynamic_cast<YQPkgPatternListItem *>(_view->itemFromIndex(index));
        if ( item )
        {
            //_view->drawRow( painter, option, index  );
        }

        QItemDelegate::paint(painter, option, index);
        painter->restore();
    }
    
};

YQPkgPatternList::YQPkgPatternList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    yuiDebug() << "Creating pattern list" << endl;

    int numCol = 0;
    QStringList headers;
    //headers << "";
    headers << "";	_iconCol	= numCol++;
    headers << "";	_statusCol	= numCol++;

    // Translators: "Pattern" refers to so-called "installation patterns",
    // i.e., specific task-oriented groups of packages, like "everything that
    // is needed to run a web server". The idea of patterns is that they also
    // include the configuration workflow needed for that task, such of
    // configuring the web server. For the scope of the package selector, this
    // is only of little relevance, though.

    headers << _( "Pattern" );	_summaryCol	= numCol++;
    headers << "";	_howmanyCol	= numCol++;

    setColumnCount( 4 );
    setHeaderLabels(headers);

    setIndentation(0);
    
    setItemDelegateForColumn( _iconCol, new YQPkgPatternItemDelegate( this ) );
    setItemDelegateForColumn( _statusCol, new YQPkgPatternItemDelegate( this ) );
    setItemDelegateForColumn( _summaryCol, new YQPkgPatternItemDelegate( this ) );
    setItemDelegateForColumn( _howmanyCol, new YQPkgPatternItemDelegate( this ) );

    // Can use the same colum for "broken" and "satisfied":
    // Both states are mutually exclusive

    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;

//     header()->setStretchEnabled( _statusCol , false );
//     header()->setStretchEnabled( _summaryCol, true  );

    setSortingEnabled( true );
    sortByColumn( summaryCol(), Qt::AscendingOrder );

    setAllColumnsShowFocus( true );

    header()->setResizeMode( statusCol(), QHeaderView::Fixed );
    header()->setResizeMode( summaryCol(), QHeaderView::Stretch );
    header()->setResizeMode( howmanyCol(), QHeaderView::Stretch );
    
    header()->resizeSection( statusCol(), 25 );
    setColumnWidth( statusCol(), 25 );
    setColumnWidth( summaryCol(), 100 );
    setColumnWidth( howmanyCol(), 7 );
    
    //header()->resizeSection( 0, 0 );

    //header()->setMinimumSectionSize( 25 );

    if ( autoFilter )
    {
	connect( this, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ),
		 this, SLOT  ( filter()				   ) );
    }

    setIconSize(QSize(32,32));
    header()->resizeSection( iconCol(), 34 );

    if ( autoFill )
    {
	fillList();
	selectSomething();
    }
    yuiDebug() << "Creating pattern list done" << endl;
}


YQPkgPatternList::~YQPkgPatternList()
{
    // NOP
}


void
YQPkgPatternList::fillList()
{
    clear();
    yuiDebug() << "Filling pattern list" << endl;


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
		yuiDebug() << "Pattern " << zyppPattern->name()
			   << " is not user-visible" << endl;
	}
	else
	{
	    yuiError() << "Found non-Pattern selectable" << endl;
	}
    }

    yuiDebug() << "Pattern list filled" << endl;
    resizeColumnToContents(_iconCol);
    resizeColumnToContents(_statusCol);
    resizeColumnToContents(_howmanyCol);
}


YQPkgPatternCategoryItem *
YQPkgPatternList::category( const QString & categoryName )
{
    if ( categoryName.isEmpty() )
	return 0;

    YQPkgPatternCategoryItem * cat = _categories[ categoryName ];

    if ( ! cat )
    {
	yuiDebug() << "New pattern category \""<< categoryName << "\"" << endl;

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
            int total = 0;
            int installed = 0;
            
            zypp::Pattern::Contents  c(zyppPattern->contents());
            for ( zypp::Pattern::Contents::Selectable_iterator it = c.selectableBegin();
                  it != c.selectableEnd();
                  ++it )
            {
                ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
                if ( zyppPkg )
                {
                    if ( (*it)->installedSize() > 0 )
                        ++installed;
                    ++total;
                    
                    emit filterMatch( *it, zyppPkg );
                }
            }
            selection()->setText( _howmanyCol, QString().sprintf("[%d/%d]", installed, total));
        }
    }

    emit filterFinished();
    resizeColumnToContents(_howmanyCol);
}


void
YQPkgPatternList::addPatternItem( ZyppSel	selectable,
				  ZyppPattern	zyppPattern )
{
    if ( ! selectable )
    {
	yuiError() << "NULL ZyppSelectable!" << endl;
	return;
    }

    YQPkgPatternCategoryItem * cat = category( fromUTF8( zyppPattern->category() ) );
    YQPkgPatternListItem * item = 0;

    if ( cat )
    {
        item = new YQPkgPatternListItem( this, cat, selectable, zyppPattern );
    }
    else
    {
        item = new YQPkgPatternListItem( this, selectable, zyppPattern );
        item->setTextAlignment( howmanyCol(), Qt::AlignRight );
        
    }

    resizeColumnToContents(_howmanyCol);
    resizeColumnToContents(_summaryCol);

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
            if ( col == iconCol() )
            {
                categoryItem->setExpanded( ! categoryItem->isExpanded() );
            }
        }
    }
    else
    {
        //QSize size = listViewItem->sizeHint(_iconCol);
        //size.scale(size.width(), size.height()*2, Qt::IgnoreAspectRatio);
        //listViewItem->setSizeHint(_iconCol, size);
        
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

    if (_zyppPattern)
    {
        setIcon(_patternList->iconCol(), QIcon(QString("/usr/share/icons/hicolor/32x32/apps/") + _zyppPattern->icon().c_str() + QString(".png")));
    }

    setStatusIcon();
    setFirstColumnSpanned ( false );
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
    setIcon( _patternList->iconCol(),
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
