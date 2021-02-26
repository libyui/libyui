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
#include <QDateTime>

#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <zypp/RepoManager.h>

#include <QTreeWidget>
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
    yuiDebug() << "Creating repository list" << endl;

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    QStringList headers;

    // Column headers for repository list
    headers <<  _( "Name");	_nameCol	= numCol++;
    //headers << _( "URL");	_urlCol		= numCol++;

    setHeaderLabels(headers);

    //setAllColumnsShowFocus( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( itemSelectionChanged() ),
	     this, 	SLOT  ( filterIfVisible()) );
    setIconSize(QSize(32,32));
    fillList();
    setSortingEnabled( true );
    sortByColumn( nameCol(), Qt::AscendingOrder );
    selectSomething();

    yuiDebug() << "Creating repository list done" << endl;
}


YQPkgRepoList::~YQPkgRepoList()
{
    // NOP
}


void
YQPkgRepoList::fillList()
{
    clear();
    yuiDebug() << "Filling repository list" << endl;

    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd();
	  ++it )
    {
	addRepo( *it );
    }

    yuiDebug() << "Inst repository filled" << endl;
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

    yuiMilestone() << "Collecting packages in selected repositories..." << endl;
    QTime stopWatch;
    stopWatch.start();


    //
    // Collect all packages on this repository
    //

    set<ZyppSel> exactMatches;
    set<ZyppSel> nearMatches;



    QTreeWidgetItem * item;

    QList<QTreeWidgetItem *> items = selectedItems();
    QListIterator<QTreeWidgetItem *> it(items);

    while ( it.hasNext() )
    {
      item = it.next();
      YQPkgRepoListItem * repoItem = dynamic_cast<YQPkgRepoListItem *> (item);

        if ( repoItem )
        {
            ZyppRepo currentRepo = repoItem->zyppRepo();

            for ( ZyppPoolIterator sel_it = zyppPkgBegin();
                  sel_it != zyppPkgEnd();
                  ++sel_it )
            {
                if ( (*sel_it)->installedObj() &&
                     ( currentRepo.isSystemRepo() ) )
                {
                    exactMatches.insert( *sel_it );
                }
                     
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

    yuiDebug() << "Packages sent to package list. Elapsed time: "
	       << stopWatch.elapsed() / 1000.0 << " sec"
	       << endl;

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
    QTreeWidgetItem * item = currentItem();

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

#if SHOW_SINGLE_PRODUCT
        ZyppProduct product = singleProduct( _zyppRepo );

        if ( product )  // only if the repository provides exactly one product
        {               // (which is the most common case)
            name = product->summary();
        }
#endif

        if ( ! name.empty() )
        {
            setText( nameCol(), fromUTF8( name ));
        }

    }
     
    std::string infoToolTip;
    infoToolTip += ("<b>" + repo.info().name() + "</b>");
    
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
    setToolTip( nameCol(), infoToolTip.c_str() );
     
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


    setIcon( 0, QIcon( iconPath.sprintf("/usr/share/icons/hicolor/48x48/apps/%s.png", iconName.toUtf8().data()) ));
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
	    yuiMilestone() << "Multiple products in repository "
			   << zyppRepo.info().alias()
			   << endl;
	    ZyppProduct null;
	    return null;
	}

	++it;
    }

    if ( ! product )
	yuiMilestone() << "No product in repository "
		       << zyppRepo.info().alias()
		       << endl;

    return product;
}

bool
YQPkgRepoListItem::operator< ( const QTreeWidgetItem & other ) const
{
    const YQPkgRepoListItem * otherItem = dynamic_cast<const YQPkgRepoListItem *>(&other);

    return zyppRepo().info().name() < otherItem->zyppRepo().info().name();
}

#include "YQPkgRepoList.moc"
