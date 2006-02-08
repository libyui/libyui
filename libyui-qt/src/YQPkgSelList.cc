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
#include "YQZypp.h"
#include <zypp/ui/ResPoolProxy.h>
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

#ifdef FIXME
    PMManager::SelectableVec::const_iterator it = Y2PM::selectionManager().begin();

    while ( it != Y2PM::selectionManager().end() )
    {
	zypp::Selection::constPtr pkgSel = (*it)->theObj();

	if ( pkgSel )
	{
	    if ( pkgSel->visible() && ! pkgSel->isBase() )
	    {
		addPkgSelItem( selectable, sel );
	    }
	}

	++it;
    }
#endif

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

    if ( selection() )
    {
	zypp::Selection::constPtr sel = selection()->zyppSel();

	if ( sel )
	{
#if 0
	    set<Selectable::Ptr> slcList = sel->inspacks_ptrs();
	    set<Selectable::Ptr>::const_iterator it = slcList.begin();

	    while ( it != slcList.end() )
	    {
		emit filterMatch( (*it), (*it)->theObj() );
		++it;
	    }
#endif
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( zypp::ui::Selectable::Ptr	selectable,
			     zypp::Selection::constPtr	zyppSel )
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
				    zypp::ui::Selectable::Ptr	selectable,
				    zypp::Selection::constPtr 	pkgSel )
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
YQPkgSelListItem::setStatus( zypp::ui::Status newStatus )
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
