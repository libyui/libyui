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

  File:	      YQPkgRepoList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <algorithm>
#include <QDateTime>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <zypp/RepoManager.h>

#include <QTreeWidget>
#include "YQPkgRepoList.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::set;
using std::vector;



YQPkgRepoList::YQPkgRepoList( QWidget * parent )
    : QY2ListView( parent )
{
    y2debug( "Creating repository list" );

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;
#if FIXME
    // Column headers for repository list
    addColumn( _( "Name"	) );	_nameCol	= numCol++;
    addColumn( _( "URL"		) );	_urlCol		= numCol++;

    setAllColumnsShowFocus( true );
    setSelectionMode( Q3ListView::Extended );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( currentItemChanged() ),
	     this, 	SLOT  ( filterIfVisible()) );

    fillList();
    selectSomething();
#endif

    y2debug( "Creating repository list done" );
}


YQPkgRepoList::~YQPkgRepoList()
{
    // NOP
}


void
YQPkgRepoList::fillList()
{
    clear();
    y2debug( "Filling repository list" );

    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd();
	  ++it )
    {
	addRepo( *it );
    }

    y2debug( "Inst repository filled" );
}


int
YQPkgRepoList::countEnabledRepositories()
{
    return zyppPool().knownRepositoriesSize();
}


void
YQPkgRepoList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgRepoList::filter()
{
#if FIXME
    emit filterStart();

    y2milestone( "Collecting packages in selected repositories..." );
    QTime stopWatch;
    stopWatch.start();


    //
    // Collect all packages on this repository
    //

    set<ZyppSel> exactMatches;
    set<ZyppSel> nearMatches;

    Q3ListViewItem * item = firstChild();	// take multi selection into account

    while ( item )
    {
	if ( item->isSelected() )
	{
	    YQPkgRepoListItem * repoItem = dynamic_cast<YQPkgRepoListItem *> (item);

	    if ( repoItem )
	    {
		ZyppRepo currentRepo = repoItem->zyppRepo();

		for ( ZyppPoolIterator sel_it = zyppPkgBegin();
		      sel_it != zyppPkgEnd();
		      ++sel_it )
		{
		    if ( (*sel_it)->candidateObj() &&
			 (*sel_it)->candidateObj()->repository() == currentRepo )
		    {
			exactMatches.insert( *sel_it );
		    }
		    else
		    {
			zypp::ui::Selectable::available_iterator pkg_it = (*sel_it)->availableBegin();

			while ( pkg_it != (*sel_it)->availableEnd() )
			{
			    if ( (*pkg_it)->repository() == currentRepo )
				nearMatches.insert( *sel_it );

			    ++pkg_it;
			}
		    }
		}

	    }
	}

	item = item->nextSibling();
    }


    //
    // Send all exact matches to the list
    // (emit a filterMatch signal for each one)
    //

    set<ZyppSel>::const_iterator sel_it = exactMatches.begin();

    while ( sel_it != exactMatches.end() )
    {
	emit filterMatch( (*sel_it), tryCastToZyppPkg( (*sel_it)->theObj() ) );
	nearMatches.erase( *sel_it );
	++sel_it;
    }


    //
    // Send all near matches to the list
    // (emit a filterNearMatch signal for each one)
    //

    sel_it = nearMatches.begin();

    while ( sel_it != nearMatches.end() )
    {
	emit filterNearMatch( *sel_it, tryCastToZyppPkg( (*sel_it)->theObj() ) );
	++sel_it;
    }

    y2debug( "Packages sent to package list. Elapsed time: %f sec", stopWatch.elapsed() / 1000.0 );

    emit filterFinished();
#endif
}


void
YQPkgRepoList::addRepo( ZyppRepo repo )
{
    new YQPkgRepoListItem( this, repo );
}


YQPkgRepoListItem *
YQPkgRepoList::selection() const
{
#if FIXME
    Q3ListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgRepoListItem *> (item);
#else
    return 0;
#endif
}






YQPkgRepoListItem::YQPkgRepoListItem( YQPkgRepoList *	repoList,
				      ZyppRepo		repo	)
    : QY2ListViewItem( repoList )
    , _repoList( repoList )
    , _zyppRepo( repo )
{
    if ( nameCol() >= 0 )
    {
        string name = repo.info().name();

#if SHOW_SINGLE_PRODUCT
        ZyppProduct product = singleProduct( _zyppRepo );

        if ( product )  // only if the repository provides exactly one product
        {               // (which is the most common case)
            name = product->summary();
        }
#endif

	if ( ! name.empty() )
	    setText( nameCol(), fromUTF8( name ));
    }

    if ( urlCol() >= 0 )
    {
        zypp::Url repoUrl;

	if ( ! repo.info().baseUrlsEmpty() )
	    repoUrl = *repo.info().baseUrlsBegin();

	setText( urlCol(), repoUrl.asString().c_str() );
    }
}



YQPkgRepoListItem::~YQPkgRepoListItem()
{
    // NOP
}


ZyppProduct
YQPkgRepoListItem::singleProduct( ZyppRepo zyppRepo )
{
    ZyppProduct product;

    zypp::ResStore::iterator it = zyppRepo.resolvables().begin();

    //
    // Find the first product on this repository
    //

    while ( it != zyppRepo.resolvables().end() && ! product )
    {
	product = zypp::dynamic_pointer_cast<zypp::Product>( *it );
	++it;
    }

    //
    // Check if there is another product on this repository
    //

    while ( it != zyppRepo.resolvables().end() )
    {
	if ( zypp::dynamic_pointer_cast<zypp::Product>( *it ) )
	{
	    y2milestone( "Multiple products in repository %s",
			 zyppRepo.info().alias().c_str() );
	    ZyppProduct null;
	    return null;
	}

	++it;
    }

    if ( ! product )
	y2milestone( "No product in repository %s",
		     zyppRepo.info().alias().c_str() );

    return product;
}



#include "YQPkgRepoList.moc"
