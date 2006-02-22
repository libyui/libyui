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
#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>


#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgSelList.h"


YQPkgSelList::YQPkgSelList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    y2debug( "Creating selection list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    addColumn( _( "Selection" ) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );

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


    for ( ZyppPoolIterator it = zyppSelectionsBegin();
	  it != zyppSelectionsEnd();
	  ++it )
    {
	ZyppSelection zyppSelection = tryCastToZyppSelection( (*it)->theObj() );

	if ( zyppSelection )
	{
	    if ( zyppSelection->visible() && ! zyppSelection->isBase() )
	    {
		addPkgSelItem( *it, zyppSelection );
	    }
	}
	else
	{
	    y2error( "Found non-Selection selectable" );
	}
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

    if ( selection() )	// The seleted QListViewItem
    {
	ZyppSelection zyppSelection = selection()->zyppSelection();

	if ( zyppSelection )
	{
	    set<string> wanted = zyppSelection->install_packages();

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		string name = (*it)->theObj()->name();

		if ( contains( wanted, name ) )
		{
		    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );

		    if ( zyppPkg )
		    {
			emit filterMatch( *it, zyppPkg );
		    }
		}
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgSelList::addPkgSelItem( ZyppSel		selectable,
			     ZyppSelection	zyppSelection )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSelectable!" );
	return;
    }

    new YQPkgSelListItem( this, selectable, zyppSelection );
}


YQPkgSelListItem *
YQPkgSelList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgSelListItem *> (item);
}






YQPkgSelListItem::YQPkgSelListItem( YQPkgSelList *	pkgSelList,
				    ZyppSel		selectable,
				    ZyppSelection	zyppSelection )
    : YQPkgObjListItem( pkgSelList, selectable, zyppSelection )
    , _pkgSelList( pkgSelList )
    , _zyppSelection( zyppSelection )
{
    if ( ! _zyppSelection )
	_zyppSelection = tryCastToZyppSelection( selectable->theObj() );

    QString text = fromUTF8( _zyppSelection->summary() );

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
    ZyppStatus oldStatus = selectable()->status();

    YQPkgObjListItem::setStatus( newStatus );

    if ( oldStatus != selectable()->status() )
    {
	applyChanges();

	_pkgSelList->updateItemStates();
	_pkgSelList->sendUpdatePackages();
    }
}


void
YQPkgSelListItem::applyChanges()
{
    bool install = true;

    switch ( selectable()->status() )
    {
	case S_Del:
	case S_AutoDel:
	case S_NoInst:
	case S_KeepInstalled:
	case S_Taboo:
	case S_Protected:
	    install = false;
	    break;

	case S_Install:
	case S_AutoInstall:
	case S_Update:
	case S_AutoUpdate:
	    install = true;
	    break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }


    ZyppObj obj = install ?	// the other way round as mayb expected:
	selectable()->candidateObj() :	// install the candidate,
	selectable()->installedObj();	// remove the installed

    if ( ! obj )
	obj = selectable()->theObj();

    y2debug( "Transacting selection %s with %s",
	     obj->name().c_str(), install ? "install" : "delete" );

    bool success = zypp::getZYpp()->resolver()->transactResObject( obj, install );

    if ( ! success )
	y2warning( "Couldn't transact selection %s", obj->name().c_str() );
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

    if ( ! _zyppSelection || ! other || ! other->zyppSelection() )
	return 0;

    return _zyppSelection->order().compare( other->zyppSelection()->order() );
}



#include "YQPkgSelList.moc"
