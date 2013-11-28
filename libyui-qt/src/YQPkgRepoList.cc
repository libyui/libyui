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

  Textdomain "qt-pkg"

/-*/

#include <algorithm>
#include <qdatetime.h>
#include <qpixmap.h>

#define YUILogComponent "qt-pkg"
#include "ycp/y2log.h"
#include <zypp/RepoManager.h>
#include <zypp/PoolQuery.h>

#include <qlistview.h>
#include "YQPkgRepoList.h"
#include "YQi18n.h"
#include "utf8.h"

using std::string;
using std::list;
using std::set;
using std::vector;


YQPkgRepoList::YQPkgRepoList( QWidget * parent )
    : QY2ListView( parent )
{
    y2debug("Creating repository list");

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    // Column headers for installation source list
    addColumn( _( "Name"       ) );    _nameCol        = numCol++;
    addColumn( _( "URL"                ) );    _urlCol         = numCol++;
    setAllColumnsShowFocus( true );
    setSelectionMode( QListView::Extended );   // allow multi-selection with Ctrl-mouse

    setAllColumnsShowFocus( true );
    setSelectionMode( QListView::Extended );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( itemSelectionChanged() ),
	     this, 	SLOT  ( filterIfVisible()) );
    //setIconSize(QSize(32,32));
    fillList();
    //setSortingEnabled( true );
    //sortByColumn( nameCol(), Qt::AscendingOrder );
    selectSomething();

    y2debug("Creating repository list done");
}


YQPkgRepoList::~YQPkgRepoList()
{
    // NOP
}


void
YQPkgRepoList::fillList()
{
    clear();
    y2debug("Filling repository list");

    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd();
	  ++it )
    {
	addRepo( *it );
    }

    y2debug("Inst repository filled");
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
    emit filterStart();

    y2milestone("Collecting packages in selected repositories...");
    QTime stopWatch;
    stopWatch.start();


    //
    // Collect all packages on this repository
    //

    QListViewItem * item = firstChild();

    while ( item )
    {

        if ( item->isSelected() )
        {
            YQPkgRepoListItem * repoItem = dynamic_cast<YQPkgRepoListItem *> (item);

            if ( repoItem )
            {
                ZyppRepo currentRepo = repoItem->zyppRepo();

	        zypp::PoolQuery query;
	        query.addRepo( currentRepo.info().alias() );
	        query.addKind(zypp::ResKind::package);

    	        for( zypp::PoolQuery::Selectable_iterator it = query.selectableBegin();
	             it != query.selectableEnd(); it++)
    	        {
		    emit filterMatch( *it, tryCastToZyppPkg( (*it)->theObj() ) );
    	        }
	    }
        }
    }

    y2debug("Packages sent to package list. Elapsed time: %f sec", stopWatch.elapsed() / 1000.0);

    emit filterFinished();
}


void
YQPkgRepoList::addRepo( ZyppRepo repo )
{
    new YQPkgRepoListItem( this, repo );
}


YQPkgRepoListItem *
YQPkgRepoList::selection() const
{
    QListViewItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgRepoListItem *> (item);
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
        if ( ! name.empty() )
        {
            setText( nameCol(), fromUTF8( name ));
        }
    }

    std::string infoToolTip;
    infoToolTip += ("<b>" + repo.info().name() + "</b>");

    ZyppProduct product = singleProduct( _zyppRepo );
    if ( product )
    {
        infoToolTip += ("<p>" + product->summary() + "</p>");
    }

    if ( ! repo.info().baseUrlsEmpty() )
    {
        zypp::RepoInfo::urls_const_iterator it;
        infoToolTip += "<ul>";

        for ( it = repo.info().baseUrlsBegin();
              it != repo.info().baseUrlsEnd();
              ++it )
        {
            infoToolTip += ("<li>" + (*it).asString() + "</li>");
        }
        infoToolTip += "</ul>";
     }
    //setToolTip( nameCol(), infoToolTip.c_str() );

    QString iconPath;
    QString iconName = "yast-sw_source";

    if ( ! repo.info().baseUrlsEmpty() )
    {
        zypp::Url repoUrl = *repo.info().baseUrlsBegin();

        if ( urlCol() >= 0 )
        {
            setText( urlCol(), repoUrl.asString().c_str() );
        }

        if (QString(repoUrl.asString().c_str()).contains("KDE") )
            iconName = "pattern-kde";
        if (QString(repoUrl.asString().c_str()).contains("GNOME") )
            iconName = "pattern-gnome";
        if (QString(repoUrl.asString().c_str()).contains("KDE") )
            iconName = "pattern-kde";
        if (QString(repoUrl.asString().c_str()).contains("update") )
            iconName = "yast-update";
        if (QString(repoUrl.asString().c_str()).contains("home:") )
            iconName = "yast-users";
    }

    if ( repo.isSystemRepo() )
        iconName = "yast-host";


    setPixmap( 0, QPixmap( iconPath.sprintf("/usr/share/icons/hicolor/48x48/apps/%s.png", iconName.ascii()) ));
}



YQPkgRepoListItem::~YQPkgRepoListItem()
{
    // NOP
}


ZyppProduct
YQPkgRepoListItem::singleProduct( ZyppRepo zyppRepo )
{
    ZyppProduct product;

    zypp::ResPool::byKind_iterator it = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product );
    zypp::ResPool::byKind_iterator end = zypp::ResPool::instance().byKindEnd( zypp::ResKind::product );

    //
    // Find the first product on this repository
    //

    while ( it != end && ! product )
    {
        if ( it->resolvable()->repoInfo().alias() == zyppRepo.info().alias() )
            product = zypp::asKind<zypp::Product>( it->resolvable() );
	++it;
    }

    //
    // Check if there is another product on this repository
    //

    while ( it != end )
    {
	if ( it->resolvable()->repoInfo().alias() == zyppRepo.info().alias() )
	{
            y2milestone("Multiple products in repository %s", zyppRepo.info().alias().c_str());
	    ZyppProduct null;
	    return null;
	}

	++it;
    }

    if ( ! product )
        y2milestone("No product in repository ", zyppRepo.info().alias().c_str());

    return product;
}

bool
YQPkgRepoListItem::operator< ( const QListViewItem & other ) const
{
    const YQPkgRepoListItem * otherItem = dynamic_cast<const YQPkgRepoListItem *>(&other);

    return zyppRepo().info().name() < otherItem->zyppRepo().info().name();
}

#include "YQPkgRepoList.moc"
