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

    PMManager::SelectableVec::const_iterator it = Y2PM::selectionManager().begin();

    while ( it != Y2PM::selectionManager().end() )
    {
	zypp::Selection::constPtr sel = ( *it)->theObj();

	if ( sel )
	{
	    if ( sel->visible() && ! sel->isBase() )
	    {
		addPkgSelItem( sel );
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

    if ( selection() )
    {
	zypp::Selection::constPtr sel = selection()->zyppSel();

	if ( sel )
	{
	    set<Selectable::Ptr> slcList = sel->inspacks_ptrs();
	    set<Selectable::Ptr>::const_iterator it = slcList.begin();

	    while ( it != slcList.end() )
	    {
		emit filterMatch( ( *it)->theObj() );
		++it;
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( zypp::Selection::constPtr zyppSel )
{
    if ( ! zyppSel )
    {
	y2error( "NULL zypp::Selection!" );
	return;
    }

    new YQPkgSelListItem( this, zyppSel );
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
    Y2PM::selectionManager().activate( Y2PM::packageManager() );
    emit updatePackages();
}






YQPkgSelListItem::YQPkgSelListItem( YQPkgSelList * pkgSelList, zypp::Selection::constPtr pkgSel )
    : YQPkgObjListItem( pkgSelList, pkgSel )
    , _pkgSelList( pkgSelList )
    , _zyppSel( pkgSel )
{
    QString text = fromUTF8( _zyppSel->summary( Y2PM::getPreferredLocale() ) );
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
