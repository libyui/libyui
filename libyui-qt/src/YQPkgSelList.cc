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

#include <Y2PM.h>
#include <y2pm/PMSelectionManager.h>

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgSelList.h"



YQPkgSelList::YQPkgSelList( QWidget *parent )
    : YQPkgObjList( parent )
{
    y2debug( "Creating selection list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    addColumn( _( "Selection" ) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

    fillList();
    selectSomething();

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

    PMManager::PMSelectableVec::const_iterator it = Y2PM::selectionManager().begin();

    while ( it != Y2PM::selectionManager().end() )
    {
	PMSelectionPtr sel = (*it)->theObject();

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
	PMSelectionPtr sel = selection()->pmSel();

	if ( sel )
	{
	    std::set<PMSelectablePtr> slcList = sel->inspacks_ptrs();
	    std::set<PMSelectablePtr>::const_iterator it = slcList.begin();

	    while ( it != slcList.end() )
	    {
		emit filterMatch( (*it)->theObject() );
		++it;
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( PMSelectionPtr pmSel )
{
    if ( ! pmSel )
    {
	y2error( "NULL PMSelection!" );
	return;
    }

    new YQPkgSelListItem( this, pmSel);
}


YQPkgSelListItem *
YQPkgSelList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgSelListItem *> (item);
}






YQPkgSelListItem::YQPkgSelListItem( YQPkgSelList * pkgSelList, PMSelectionPtr pkgSel )
    : YQPkgObjListItem( pkgSelList, pkgSel )
    , _pkgSelList( pkgSelList )
    , _pmSel( pkgSel )
{
    QString text = fromUTF8( _pmSel->summary( Y2PM::getPreferredLocale() ) );
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
YQPkgSelListItem::setStatus( PMSelectable::UI_Status newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    Y2PM::selectionManager().activate( Y2PM::packageManager() );
    _pkgSelList->sendUpdatePackages();
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
    YQPkgSelListItem * other = (YQPkgSelListItem *) otherListViewItem;

    if ( ! _pmSel || ! other || ! other->pmSel() )
	return 0;

    return _pmSel->order().compare( other->pmSel()->order() );
}



#include "YQPkgSelList.moc.cc"
