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

  File:	      YQPkgInstSrcList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgInstSrcList.h"
#include <zypp/SourceManager.h>

using std::list;
using std::set;



YQPkgInstSrcList::YQPkgInstSrcList( QWidget * parent )
    : QY2ListView( parent )
{
    y2debug( "Creating inst source list" );

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    // Column headers for installation source list
    addColumn( _( "Name"	) );	_nameCol	= numCol++;
    addColumn( _( "URL"		) );	_urlCol		= numCol++;

    setAllColumnsShowFocus( true );
    setSelectionMode( QListView::Extended );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( selectionChanged() ),
	     this, 	SLOT  ( filter()           ) );

    fillList();
    selectSomething();

    y2debug( "Creating inst source list done" );
}


YQPkgInstSrcList::~YQPkgInstSrcList()
{
    // NOP
}


void
YQPkgInstSrcList::fillList()
{
    clear();
    y2debug( "Filling inst source list" );

    zypp::SourceManager_Ptr sourceManager = zypp::SourceManager::sourceManager();
    list<unsigned int> sources = sourceManager->allSources();

    for ( list<unsigned int>::const_iterator it = sources.begin();
	  it != sources.end();
	  ++it )
    {
	zypp::Source_Ref src = sourceManager->findSource( *it );

	if ( src && src.enabled() )
	    addInstSrc( src );
    }

    y2debug( "Inst source list filled" );
}


void
YQPkgInstSrcList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgInstSrcList::filter()
{
    emit filterStart();

    
    //
    // Collect all packages on this inst source
    //
    
    set<ZyppObj> currentSrcPkg;
    QListViewItem * item = firstChild();	// take multi selection into account

    while ( item )
    {
	if ( item->isSelected() )
	{
	    YQPkgInstSrcListItem * instSrcItem = dynamic_cast<YQPkgInstSrcListItem *> (item);

	    if ( instSrcItem )
	    {
		zypp::ResStore::iterator pkg_it = instSrcItem->zyppSrc().resolvables().begin();

		while ( pkg_it != instSrcItem->zyppSrc().resolvables().end() )
		{
		    ZyppPkg pkg = tryCastToZyppPkg( *pkg_it );

		    if ( pkg )
			currentSrcPkg.insert( pkg );

		    ++pkg_it;
		}
	    }
	}

	item = item->nextSibling();
    }


    //
    // Collect matching selectables for all selected inst sources
    // and store them in sets to avoid duplicates in the resulting list
    //
    
    set<ZyppSel> exactMatches;
    set<ZyppSel> nearMatches;

    for ( ZyppPoolIterator sel_it = zyppPkgBegin();
	  sel_it != zyppPkgEnd();
	  ++sel_it )
    {
	if ( contains( currentSrcPkg, (*sel_it)->candidateObj() ) ||
	     contains( currentSrcPkg, (*sel_it)->installedObj() )   )
	{
	    exactMatches.insert( *sel_it );
	}
	else // Maybe one of the other available objects is on this inst src
	{
	    zypp::ui::Selectable::available_iterator pkg_it = (*sel_it)->availableBegin();
	    
	    while ( pkg_it != (*sel_it)->availableEnd() )
	    {
		if ( contains( currentSrcPkg, *pkg_it ) )
		    nearMatches.insert( *sel_it );
		    
		++pkg_it;
	    }
	}
    }


    //
    // Send all members of the resulting sets to the list
    // (emit a filterMatch signal for each one)
    //

    set<ZyppSel>::const_iterator sel_it = exactMatches.begin();

    while ( sel_it != exactMatches.end() )
    {
	emit filterMatch( (*sel_it), tryCastToZyppPkg( (*sel_it)->theObj() ) );
	++sel_it;
    }


    //
    // Send all leftovers to the package list with filterNearMatch
    // to be displayed dimmed in the package list
    //

    sel_it = nearMatches.begin();

    while ( sel_it != nearMatches.end() )
    {
	emit filterNearMatch( *sel_it, tryCastToZyppPkg( (*sel_it)->theObj() ) );
	++sel_it;
    }

    emit filterFinished();
}


void
YQPkgInstSrcList::addInstSrc( ZyppSrc src )
{
    new YQPkgInstSrcListItem( this, src );
}


YQPkgInstSrcListItem *
YQPkgInstSrcList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgInstSrcListItem *> (item);
}






YQPkgInstSrcListItem::YQPkgInstSrcListItem( YQPkgInstSrcList *	instSrcList,
					    ZyppSrc		src 		)
    : QY2ListViewItem( instSrcList )
    , _instSrcList( instSrcList )
    , _zyppSrc( src )
{
    if ( nameCol() >= 0 )
    {
	string name;
	ZyppProduct product = singleProduct();

	if ( product )	// only if the source provides exactly one product
	{		// (which is the most common case)
	    name = product->displayName();
	}

	if ( ! name.empty() )
	    setText( nameCol(), name.c_str() );
    }

    if ( urlCol() >= 0 )
    {
	setText( urlCol(), src.url().asString().c_str() );
    }
}



YQPkgInstSrcListItem::~YQPkgInstSrcListItem()
{
    // NOP
}


ZyppProduct
YQPkgInstSrcListItem::singleProduct()
{
    ZyppProduct product;

    zypp::ResStore::iterator it = zyppSrc().resolvables().begin();

    //
    // Find the first product on this inst src
    //

    while ( it != zyppSrc().resolvables().end() && ! product )
    {
	product = zypp::dynamic_pointer_cast<zypp::Product>( *it );
	++it;
    }

    //
    // Check if there is another product on this inst src
    //

    while ( it != zyppSrc().resolvables().end() )
    {
	if ( zypp::dynamic_pointer_cast<zypp::Product>( *it ) )
	{
	    y2milestone( "Multiple products on installation source %s",
			 zyppSrc().alias().c_str() );
	    ZyppProduct null;
	    return null;
	}

	++it;
    }

    if ( ! product )
	y2milestone( "No product on installation source %s",
		     zyppSrc().alias().c_str() );

    return product;
}



#include "YQPkgInstSrcList.moc"
