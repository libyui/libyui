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

  File:	      YQPkgYouPatchList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qpopupmenu.h>
#include <qaction.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgYouPatchList.h"



YQPkgYouPatchList::YQPkgYouPatchList( QWidget *parent )
    : YQPkgObjList( parent )
{
    y2debug( "Creating YOU patch list" );

    int numCol = 0;
    addColumn( "" );					_statusCol	= numCol++;
    addColumn( _( "YaST Online Update Patch"	) );	_summaryCol	= numCol++;
    addColumn( _( "Kind"			) );	_kindCol	= numCol++;
    setAllColumnsShowFocus( true );

    connect( this,	SIGNAL( selectionChanged	( QListViewItem * ) ),
	     this,	SLOT  ( filter()				    ) );

    fillList();
    setSorting( kindCol() );
    selectSomething();

    y2debug( "Creating YOU patch list done" );
}


YQPkgYouPatchList::~YQPkgYouPatchList()
{
    // NOP
}


void
YQPkgYouPatchList::fillList()
{
    clear();
    y2milestone( "Filling YOU patch list" );

    PMManager::PMSelectableVec::const_iterator it = Y2PM::youPatchManager().begin();

    while ( it != Y2PM::youPatchManager().end() )
    {
	PMYouPatchPtr patch = (*it)->theObject();

	if ( patch )
	{
	    addYouPatchItem( patch );
	}

	++it;
    }

    y2milestone( "YOU patch list filled" );
}


void
YQPkgYouPatchList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgYouPatchList::filter()
{
    emit filterStart();

    if ( selection() )
    {
	PMYouPatchPtr patch = selection()->pmYouPatch();

	if ( patch )
	{
	    std::list<PMPackagePtr> pkgList = patch->packages();
	    std::list<PMPackagePtr>::const_iterator it = pkgList.begin();

	    while ( it != pkgList.end() )
	    {
		emit filterMatch( (*it) );
		++it;
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgYouPatchList::addYouPatchItem( PMYouPatchPtr pmYouPatch )
{
    if ( ! pmYouPatch )
    {
	y2error( "NULL PMYouPatch!" );
	return;
    }

    new YQPkgYouPatchListItem( this, pmYouPatch);
}


YQPkgYouPatchListItem *
YQPkgYouPatchList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgYouPatchListItem *> (item);
}


void
YQPkgYouPatchList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    _actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    _actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    _actionSetCurrentAutoInstall->addTo( _notInstalledContextMenu );
}


void
YQPkgYouPatchList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    _actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    _actionSetCurrentUpdate->addTo( _installedContextMenu );
    _actionSetCurrentAutoUpdate->addTo( _installedContextMenu );
}







YQPkgYouPatchListItem::YQPkgYouPatchListItem( YQPkgYouPatchList * youPatchList, PMYouPatchPtr youPatch )
    : YQPkgObjListItem( youPatchList, youPatch )
    , _youPatchList( youPatchList )
    , _pmYouPatch( youPatch )
{
    setText( kindCol(), _pmYouPatch->kindLabel() );
    setStatusIcon();

    switch ( _pmYouPatch->kind() )
    {
	case PMYouPatch::kind_yast:		setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case PMYouPatch::kind_security:		setTextColor( Qt::red );		break;
	case PMYouPatch::kind_recommended:	setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case PMYouPatch::kind_optional:		break;
	case PMYouPatch::kind_document:		break;
	default:				break;
    }
}


YQPkgYouPatchListItem::~YQPkgYouPatchListItem()
{
    // NOP
}


void
YQPkgYouPatchListItem::setStatus( PMSelectable::UI_Status newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    
#warning TODO: Activate patch
    
    _youPatchList->sendUpdatePackages();
}


void
YQPkgYouPatchListItem::cycleStatus()
{
    YQPkgObjListItem::cycleStatus();

    if ( status() == PMSelectable::S_Del )	// Can't delete YOU patches
	setStatus( PMSelectable::S_KeepInstalled );
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgYouPatchListItem::compare( QListViewItem * otherListViewItem,
				int		col,
				bool		ascending ) const
{
    YQPkgYouPatchListItem * other = dynamic_cast<YQPkgYouPatchListItem *> (otherListViewItem);

    if ( other )
    {
	if ( col == kindCol() )
	{
	    if ( this->constPMYouPatch()->kind() < other->constPMYouPatch()->kind() ) return -1;
	    if ( this->constPMYouPatch()->kind() > other->constPMYouPatch()->kind() ) return 1;
	    return 0;
	}
    }
    return YQPkgObjListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgYouPatchList.moc.cc"
