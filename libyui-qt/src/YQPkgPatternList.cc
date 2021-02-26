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


#include <unistd.h>	// access()

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qregexp.h>
#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>
#include <qpainter.h>
#include <qheader.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgPatternList.h"
#include "YQIconPool.h"
#include "YQUI.h"


#define CATEGORY_BACKGROUND	QColor( 0xFF, 0xC0, 0x50 )


YQPkgPatternList::YQPkgPatternList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    y2debug( "Creating pattern list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;

    // Translators: "Pattern" refers to so-called "installation patterns",
    // i.e., specific task-oriented groups of packages, like "everything that
    // is needed to run a web server". The idea of patterns is that they also
    // include the configuration workflow needed for that task, such of
    // configuring the web server. For the scope of the package selector, this
    // is only of little relevance, though.

    addColumn( _( "Pattern" )	);	_summaryCol	= numCol++;

    // Can use the same colum for "broken" and "satisfied":
    // Both states are mutually exclusive
    
    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;
    
    header()->setStretchEnabled( _statusCol , false );
    header()->setStretchEnabled( _summaryCol, true  );

    setAllColumnsShowFocus( true );
    setTreeStepSize( 0 );

    if ( autoFilter )
    {
	connect( this, SIGNAL( selectionChanged( QListViewItem * ) ),
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
	y2debug( "New pattern category \"%s\"", (const char *) categoryName );

	cat = new YQPkgPatternCategoryItem( this, categoryName );
	CHECK_PTR( cat );
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

  if ( selection() )// The seleted QListViewItem
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
	  //selection()->setInstalledPackages(installed);
	  //selection()->setTotalPackages(total);
	  //selection()->resetToolTip();
        }
    }

  emit filterFinished();
  //resizeColumnToContents(_howmanyCol);
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

    if ( cat )
	new YQPkgPatternListItem( this, cat, selectable, zyppPattern );
    else
	new YQPkgPatternListItem( this, selectable, zyppPattern );
}


YQPkgPatternListItem *
YQPkgPatternList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatternListItem *> (item);
}


void
YQPkgPatternList::pkgObjClicked( int			button,
				 QListViewItem *	listViewItem,
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
		categoryItem->setOpen( ! categoryItem->isOpen() );
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
    QListViewItemIterator it( this );

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
    if ( access( FORCE_FULL_SOLVER_RUN_FILE, F_OK ) == -1) // file doesn't exist
	solveResolvableCollections();
    
    // else branch handled the YQPkgObjList::statusChanged() signal handler:
    // connected to YQPackageSelectorBase::resolveDependencies()
    // if FORCE_FULL_SOLVER_RUN_FILE exists (see YQPatternSelector::makeConnections()
}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgPatternListItem::compare( QListViewItem *	otherListViewItem,
			       int		col,
			       bool		ascending ) const
{
    YQPkgPatternListItem * otherPatternListitem	 = dynamic_cast<YQPkgPatternListItem     *>(otherListViewItem);

    if ( _zyppPattern && otherPatternListitem && otherPatternListitem->zyppPattern() )
	return _zyppPattern->order().compare( otherPatternListitem->zyppPattern()->order() );

    YQPkgPatternCategoryItem * otherCategoryItem = dynamic_cast<YQPkgPatternCategoryItem *>(otherListViewItem);

    if ( otherCategoryItem )	// Patterns without category should always be sorted
	return -1;		// before any category

    return QListViewItem::compare( otherListViewItem, col, ascending );
}






YQPkgPatternCategoryItem::YQPkgPatternCategoryItem( YQPkgPatternList *	patternList,
						    const QString &	category	)
    : QY2ListViewItem( patternList )
    , _patternList( patternList )
{
    setText( _patternList->summaryCol(), category );
    setBackgroundColor( CATEGORY_BACKGROUND );
    setOpen( true );
    setTreeIcon();
}


YQPkgPatternCategoryItem::~YQPkgPatternCategoryItem()
{
    // NOP
}


void
YQPkgPatternCategoryItem::paintCell( QPainter *			painter,
				     const QColorGroup &	colorGroup,
				     int			column,
				     int			width,
				     int			alignment )
{
    painter->setFont( YQUI::ui()->headingFont() );
    QY2ListViewItem::paintCell( painter, colorGroup, column, width, alignment );
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
YQPkgPatternCategoryItem::setOpen( bool open )
{
    QListViewItem::setOpen( open );
    setTreeIcon();
}


void
YQPkgPatternCategoryItem::setTreeIcon()
{
    setPixmap( _patternList->statusCol(),
	       isOpen() ?
	       YQIconPool::treeMinus() :
	       YQIconPool::treePlus()   );

}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgPatternCategoryItem::compare( QListViewItem *	otherListViewItem,
				   int			col,
				   bool			ascending ) const
{
    YQPkgPatternCategoryItem * otherCategoryItem = dynamic_cast<YQPkgPatternCategoryItem *>(otherListViewItem);

    if ( _firstPattern && otherCategoryItem && otherCategoryItem->firstPattern() )
	return _firstPattern->order().compare( otherCategoryItem->firstPattern()->order() );


    YQPkgPatternListItem * otherPatternListitem	 = dynamic_cast<YQPkgPatternListItem *>(otherListViewItem);

    if ( otherPatternListitem )	// Patterns without category should always be sorted
	return 1;		// before any category

    return QListViewItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgPatternList.moc"
