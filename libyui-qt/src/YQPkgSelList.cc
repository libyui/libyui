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

  File:	      YQPkgSelList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qregexp.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgSelList.h"

using std::set;


YQPkgSelList::YQPkgSelList( QWidget * parent, bool autoFill )
    : YQPkgObjList( parent )
{
    y2debug( "Creating selection list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    addColumn( _( "Selection" ) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

    if ( autoFill )
    {
	fillList();
	selectSomething();
    }
    filter();

    y2debug( "Creating selection list done" );
}


YQPkgSelList::~YQPkgSelList()
{
    // NOP
}


void
YQPkgSelList::fillList()
{
    clear();
    y2debug( "Filling selection list" );


    zypp::ResPoolProxy proxy( zypp::getZYpp()->poolProxy() );
    ZyppPoolIterator it = proxy.byKindBegin<zypp::Selection>();

    while ( it != proxy.byKindEnd<zypp::Selection>() )
    {
	ZyppSelection zyppSel =
	    zypp::dynamic_pointer_cast<const zypp::Selection>( (*it)->theObj() );

	if ( zyppSel )
	{
	    if ( zyppSel->visible() && ! zyppSel->isBase() )
	    {
		addPkgSelItem( *it, zyppSel );
	    }
	}

	++it;
    }

    y2debug( "Selection list filled" );
}


void
YQPkgSelList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgSelList::filter()
{
    emit filterStart();

#ifdef FIXME
    if ( selection() )
    {
	ZyppSelection sel = selection()->zyppSel();

	if ( sel )
	{

	    // FIXME
	}
    }
#else
    {
	{
#endif

	    
	    zypp::ResPoolProxy proxy( zypp::getZYpp()->poolProxy() );
	    ZyppPoolIterator it = proxy.byKindBegin<zypp::Package>();

	    while ( it != proxy.byKindEnd<zypp::Package>() )
	    {
		ZyppPkg zyppPkg =
		    zypp::dynamic_pointer_cast<const zypp::Package>( (*it)->theObj() );

		if ( zyppPkg )
		{
		    emit filterMatch( *it, zyppPkg );
		}

		++it;
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( ZyppSel	selectable,
			     ZyppSelection	zyppSel )
{
    if ( ! selectable )
    {
	y2error( "NULL zypp::ui::Selectable!" );
	return;
    }

    new YQPkgSelListItem( this, selectable, zyppSel );
}


YQPkgSelListItem *
YQPkgSelList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgSelListItem *> (item);
}


void
YQPkgSelList::applyChanges()
{
#ifdef FIXME
    Y2PM::selectionManager().activate( Y2PM::packageManager() );
#endif
    emit updatePackages();
}






YQPkgSelListItem::YQPkgSelListItem( YQPkgSelList * 		pkgSelList,
				    ZyppSel	selectable,
				    ZyppSelection 	pkgSel )
    : YQPkgObjListItem( pkgSelList, selectable, pkgSel )
    , _pkgSelList( pkgSelList )
    , _zyppSel( pkgSel )
{
    if ( ! _zyppSel )
	_zyppSel = zypp::dynamic_pointer_cast<const zypp::Selection>( selectable->theObj() );

#ifdef FIXME
    QString text = fromUTF8( _zyppSel->summary( Y2PM::getPreferredLocale() ) );
#else
    QString text = fromUTF8( _zyppSel->summary() );
#endif

    // You don't want to know why we need this.
    text.replace( QRegExp( "Graphical Basis System" ), "Graphical Base System" );
    text.replace( QRegExp( "Gnome" ), "GNOME" );

    setText( summaryCol(), text );

    setStatusIcon();
}



YQPkgSelListItem::~YQPkgSelListItem()
{
    // NOP
}


void
YQPkgSelListItem::setStatus( ZyppStatus newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    _pkgSelList->applyChanges();
}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgSelListItem::compare( QListViewItem *	otherListViewItem,
			   int			col,
			   bool			ascending ) const
{
    YQPkgSelListItem * other = ( YQPkgSelListItem * ) otherListViewItem;

    if ( ! _zyppSel || ! other || ! other->zyppSel() )
	return 0;

    return _zyppSel->order().compare( other->zyppSel()->order() );
}



#include "YQPkgSelList.moc"
