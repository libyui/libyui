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
#include <Y2PM.h>
#include <y2pm/InstSrc.h>
#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgInstSrcList.h"

#define SHOW_NAME_COL	1


YQPkgInstSrcList::YQPkgInstSrcList( QWidget * parent )
    : QY2ListView( parent )
{
    y2debug( "Creating inst source list" );

    _statusCol	= -1;
    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    // Column headers for installation source list
#if SHOW_NAME_COL
    addColumn( ""		);	_statusCol	= numCol++;
#endif
    addColumn( _( "Name"	) );	_nameCol	= numCol++;
    addColumn( _( "URL"		) );	_urlCol		= numCol++;

    setAllColumnsShowFocus( true );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

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

    InstSrcManager::ISrcIdList activeSources( Y2PM::instSrcManager().getSources( true ) ); // enabled only
    InstSrcManager::ISrcIdList::const_iterator it = activeSources.begin();

    while ( it != activeSources.end() )
    {
	InstSrcManager::ISrcId instSrcId = (*it);
	
	if ( instSrcId &&
	     instSrcId->enabled() )	// should always be true if querying only enabled sources
	{
	    addInstSrc( instSrcId );
	}

	++it;
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

    if ( selection() )
    {
#if 0
	PMLanguagePtr lang = selection()->pmLang();

	if ( lang )
	{
	    PMLanguageManager::PkgSelectables selectables =
		Y2PM::languageManager().getLangPackagesFor( lang );

	    PMLanguageManager::PkgSelectables::const_iterator it = selectables.begin();
	    while ( it != selectables.end() )
	    {
		emit filterMatch( ( *it)->theObject() );
		++it;
	    }
	}
#endif
    }

    emit filterFinished();
}


void
YQPkgInstSrcList::addInstSrc( InstSrcManager::ISrcId instSrcId )
{
    if ( ! instSrcId )
    {
	y2error( "NULL instSrcId !" );
	return;
    }

    new YQPkgInstSrcListItem( this, instSrcId );
}


YQPkgInstSrcListItem *
YQPkgInstSrcList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgInstSrcListItem *> (item);
}






YQPkgInstSrcListItem::YQPkgInstSrcListItem( YQPkgInstSrcList *		instSrcList,
					    InstSrcManager::ISrcId 	instSrcId )
    : QY2ListViewItem( instSrcList )
    , _instSrcList( instSrcList )
    , _instSrcId( instSrcId )
    , _status( YQPkgInstSrcOff )
{
    if ( ! instSrcId )
    {
	y2error( "Null instSrcId" );
	return;
    }

    if ( instSrcDescr() )
    {
	if ( nameCol() > 0 )
	{
	    setText( nameCol(), instSrcDescr()->shortlabel().c_str() );
	}
    
	if ( urlCol() > 0 )
	{
	    setText( urlCol(), instSrcDescr()->url().asString( true, false, false ).c_str() );
	}
    }
}



YQPkgInstSrcListItem::~YQPkgInstSrcListItem()
{
    // NOP
}


void
YQPkgInstSrcListItem::setStatus( YQPkgInstSrcStatus newStatus )
				
{
    // TODO
    // TODO
    // TODO
    
    _instSrcList->sendUpdatePackages();
}


#include "YQPkgInstSrcList.moc"
