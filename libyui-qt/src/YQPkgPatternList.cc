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

  File:	      YQPkgPatternList.cc

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
#include "YQPkgPatternList.h"


YQPkgPatternList::YQPkgPatternList( QWidget * parent, bool autoFill, bool autoFilter )
    : YQPkgObjList( parent )
{
    y2debug( "Creating pattern list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;

    // Translators: "Pattern" refers to so-called "installation patterns",
    // i.e., specific task-oriented groups of packages, like "everything that
    // is needed to run a web server". The idea of patterns is that they also
    // include the configuration workflow needed for that task, such of
    // configuring the web server. For the scope of the package selector, this
    // is only of little relevance, though.

    addColumn( _( "Pattern" )	);	_summaryCol	= numCol++;
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

    y2debug( "Creating pattern list done" );
}


YQPkgPatternList::~YQPkgPatternList()
{
    // NOP
}


void
YQPkgPatternList::fillList()
{
    clear();
    y2debug( "Filling pattern list" );


    for ( ZyppPoolIterator it = zyppPatternsBegin();
	  it != zyppPatternsEnd();
	  ++it )
    {
	ZyppPattern zyppPattern = tryCastToZyppPattern( (*it)->theObj() );

	if ( zyppPattern )
	{
	    if ( zyppPattern->userVisible() )
	    {
		y2debug( "Pattern %s is user-visible", zyppPattern->name().c_str() );
		addPatternItem( *it, zyppPattern );
	    }
	    else
		y2debug( "Pattern %s is not user-visible", zyppPattern->name().c_str() );
	}
	else
	{
	    y2error( "Found non-Pattern selectable" );
	}
    }

    y2debug( "Pattern list filled" );
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
	    set<string> wanted = zyppPattern->install_packages();

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
YQPkgPatternList::addPatternItem( ZyppSel	selectable,
				  ZyppPattern	zyppPattern )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSelectable!" );
	return;
    }

    new YQPkgPatternListItem( this, selectable, zyppPattern );
}


YQPkgPatternListItem *
YQPkgPatternList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatternListItem *> (item);
}






YQPkgPatternListItem::YQPkgPatternListItem( YQPkgPatternList *	patternList,
					    ZyppSel		selectable,
					    ZyppPattern		zyppPattern )
    : YQPkgObjListItem( patternList, selectable, zyppPattern )
    , _patternList( patternList )
    , _zyppPattern( zyppPattern )
{
    if ( ! _zyppPattern )
	_zyppPattern = tryCastToZyppPattern( selectable->theObj() );

    QString text = fromUTF8( _zyppPattern->summary() );
    setText( summaryCol(), text );

    setStatusIcon();
}



YQPkgPatternListItem::~YQPkgPatternListItem()
{
    // NOP
}


void
YQPkgPatternListItem::setStatus( ZyppStatus newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    applyChanges();

    _patternList->sendUpdatePackages();
}


void
YQPkgPatternListItem::applyChanges()
{
    if ( selectable()->unmodified() )
    {
	y2milestone( "Pattern %s unmodified", selectable()->theObj()->name().c_str() ) ;
#if 0
	return;
#endif
    }


    bool install = selectable()->toInstall();

#ifdef FIXME
#else
    install = true;
#endif

    ZyppObj obj = install ?	// the other way round as mayb expected:
	selectable()->candidateObj() :	// install the candidate,
	selectable()->installedObj();	// remove the installed

    if ( ! obj )
	obj = selectable()->theObj();

    y2debug( "Transacting pattern %s with %s",
	     obj->name().c_str(), install ? "install" : "delete" );

    bool success = zypp::getZYpp()->resolver()->transactResObject( obj, install );

    if ( ! success )
	y2warning( "Couldn't transact pattern %s", obj->name().c_str() );
}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgPatternListItem::compare( QListViewItem *	otherListViewItem,
			       int		col,
			       bool		ascending ) const
{
    YQPkgPatternListItem * other = ( YQPkgPatternListItem * ) otherListViewItem;

    if ( ! _zyppPattern || ! other || ! other->zyppPattern() )
	return QListViewItem::compare( otherListViewItem, col, ascending );

    return _zyppPattern->order().compare( other->zyppPattern()->order() );
}



#include "YQPkgPatternList.moc"
