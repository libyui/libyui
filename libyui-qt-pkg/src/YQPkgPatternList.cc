/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*
  File:	      YQPkgPatternList.cc
  Author:     Stefan Hundhammer <shundhammer.de>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

#include <yui/qt/YQUI.h>
#include <yui/qt/YQApplication.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>

#include <QRegExp>
#include <QPainter>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>

#include "YQPackageSelector.h"
#include "YQPkgPatternList.h"
#include "YQIconPool.h"

#define PATTERN_ICON_PATH       "/usr/share/icons/hicolor/scalable/apps/"
#define PATTERN_ICON_EXTENSION  ".svg"


using std::string;
using std::set;


YQPkgPatternList::YQPkgPatternList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    yuiDebug() << "Creating pattern list" << std::endl;

    int numCol = 0;
    QStringList headers;
    headers << "";	_statusCol	= numCol++;
    headers << "";	_iconCol	= numCol++;

    // Translators: "Pattern" refers to so-called "installation patterns",
    // i.e., specific task-oriented groups of packages, like "everything that
    // is needed to run a web server". The idea of patterns is that they also
    // include the configuration workflow needed for that task, such of
    // configuring the web server. For the scope of the package selector, this
    // is only of little relevance, though.

    headers << _( "Pattern" );	_summaryCol	= numCol++;

    setColumnCount( numCol );
    setHeaderLabels( headers );
    setIndentation(0);

    // Can use the same column for "broken" and "satisfied":
    // Both states are mutually exclusive

    _satisfiedIconCol	= -42;
    _brokenIconCol	= -42;

    setSortingEnabled( true );
    sortByColumn( summaryCol(), Qt::AscendingOrder );

    setAllColumnsShowFocus( true );

    header()->setSectionResizeMode( statusCol(),   QHeaderView::Fixed   );
    header()->setSectionResizeMode( summaryCol(),  QHeaderView::Stretch );

    header()->resizeSection( statusCol(), 25 );
    setColumnWidth( statusCol(), 25 );
    setColumnWidth( summaryCol(), 100 );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // bsc#1189550


    if ( autoFilter )
    {
	connect( this, SIGNAL( currentItemChanged( QTreeWidgetItem *,
                                                   QTreeWidgetItem * ) ),
		 this, SLOT  ( filter()		                       ) );
    }

    setIconSize( QSize( 32, 32 ) );
    header()->resizeSection( iconCol(), 34 );

    if ( autoFill )
    {
	fillList();
	selectSomething();
    }

    yuiDebug() << "Creating pattern list done" << std::endl;
}


YQPkgPatternList::~YQPkgPatternList()
{
    // NOP
}


void
YQPkgPatternList::fillList()
{
    _categories.clear();

    clear();
    yuiDebug() << "Filling pattern list" << std::endl;

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
			   << " is not user-visible" << std::endl;
	}
	else
	{
	    yuiError() << "Found non-Pattern selectable" << std::endl;
	}
    }

    yuiDebug() << "Pattern list filled" << std::endl;
    resizeColumnToContents(_iconCol);
    resizeColumnToContents(_statusCol);
}


YQPkgPatternCategoryItem *
YQPkgPatternList::category( const QString & categoryName )
{
    if ( categoryName.isEmpty() )
	return 0;

    YQPkgPatternCategoryItem * cat = _categories[ categoryName ];

    if ( ! cat )
    {
	yuiDebug() << "New pattern category \""<< categoryName << "\"" << std::endl;

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

	    zypp::Pattern::Contents contents( zyppPattern->contents() );

	    for ( zypp::Pattern::Contents::Selectable_iterator it = contents.selectableBegin();
		  it != contents.selectableEnd();
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
	    selection()->setInstalledPackages(installed);
	    selection()->setTotalPackages(total);
	    selection()->resetToolTip();
	}
    }

    emit filterFinished();
    resizeColumnToContents( _statusCol );
}


void
YQPkgPatternList::addPatternItem( ZyppSel	selectable,
				  ZyppPattern	zyppPattern )
{
    if ( ! selectable )
    {
	yuiError() << "NULL ZyppSelectable!" << std::endl;
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
    }

    resizeColumnToContents( _statusCol  );
    resizeColumnToContents( _summaryCol );

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
	    if ( col == 0 )
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


YQPkgPatternListItem::YQPkgPatternListItem( YQPkgPatternList *	patternList,
					    ZyppSel		selectable,
					    ZyppPattern		zyppPattern )
    : YQPkgObjListItem( patternList, selectable, zyppPattern )
    , _patternList( patternList )
    , _zyppPattern( zyppPattern )
    , _total(0), _installed(0)
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
    , _total(0), _installed(0)
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
	string iconName = _zyppPattern->icon().basename();

	if ( iconName.empty() )
	    iconName = "pattern-generic";

	setIcon( _patternList->iconCol(), loadIcon( iconName.c_str() ) );
    }

    setStatusIcon();
    resetToolTip();
    setFirstColumnSpanned ( false );
}



YQPkgPatternListItem::~YQPkgPatternListItem()
{
    // NOP
}


QIcon
YQPkgPatternListItem::loadIcon( const string & iconName )
{
    string iconPath( PATTERN_ICON_PATH );
    iconPath += iconName;
    iconPath += PATTERN_ICON_EXTENSION;

    if ( access( iconPath.c_str(), R_OK ) == 0 )
    {
        yuiDebug() << "Loading pattern icon " << iconName << " from " << iconPath << endl;
        return YQUI::ui()->loadIcon( iconPath );
    }
    else
    {
        yuiWarning() << "Can't load pattern icon " << iconPath << endl;
        return QIcon();
    }
}


void
YQPkgPatternListItem::cycleStatus()
{
    if ( ! _editable || ! _pkgObjList->editable() )
	return;

    ZyppStatus oldStatus = status();
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
	case S_Install:
	    newStatus = S_NoInst;
	    break;

// see: bnc 476965
//	case S_KeepInstalled:
//	    newStatus = S_Install;
//	    break;

	case S_NoInst:
	    newStatus = S_Install;
	    break;

	case S_AutoInstall:
	    newStatus =	 S_NoInst;
	    break;

	default:
	    break;
    }

    if ( oldStatus != newStatus )
    {
	setStatus( newStatus );

	if ( showLicenseAgreement() )
	{
	    showNotifyTexts( newStatus );
	}
	else // License not confirmed?
	{
	    // Status is now S_Taboo or S_Del - update status icon
	    setStatusIcon();
	}

	_patternList->sendStatusChanged();
    }
}


void
YQPkgPatternListItem::resetToolTip()
{
    std::string infoToolTip;
    infoToolTip +=  ("<p>" + zyppPattern()->description() + "</p>");

    if ( totalPackages() > 0 )
    {
	infoToolTip += ("<p>" + zypp::str::form("%d / %d", installedPackages(), totalPackages() ) + "</p>");
    }

    setToolTip(_patternList->summaryCol(), fromUTF8(infoToolTip));
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
    {
	if ( _zyppPattern->order() != otherPatternListitem->zyppPattern()->order() )
	    return _zyppPattern->order() < otherPatternListitem->zyppPattern()->order();
	else
	    return _zyppPattern->name() < otherPatternListitem->zyppPattern()->name();
    }

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

    QFont categoryFont = font( _patternList->summaryCol() );
    categoryFont.setBold(true);
    QFontMetrics metrics( categoryFont );
    categoryFont.setPixelSize(int (metrics.height() * 1.1));
    setFont( _patternList->summaryCol(), categoryFont );
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
    setIcon( 0,
	     isExpanded() ?
	     YQIconPool::arrowDown() :
	     YQIconPool::arrowRight()	);

}


bool YQPkgPatternCategoryItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPatternCategoryItem * otherCategoryItem = dynamic_cast<const YQPkgPatternCategoryItem *>(&otherListViewItem);

    if ( _firstPattern && otherCategoryItem && otherCategoryItem->firstPattern() )
	return _firstPattern->order() < otherCategoryItem->firstPattern()->order();


    const YQPkgPatternListItem * otherPatternListitem	 = dynamic_cast<const YQPkgPatternListItem *>(&otherListViewItem);

    if ( otherPatternListitem )	// Patterns without category should always be sorted
	return false;		// before any category

    return QTreeWidgetItem::operator<( otherListViewItem );
}



