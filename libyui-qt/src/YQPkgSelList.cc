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

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qheader.h>
#include <qregexp.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgSelList.h"
#include "YUIQt.h"
#include "YQi18n.h"
#include "YQIconPool.h"
#include "utf8.h"



YQPkgSelList::YQPkgSelList( YUIQt *yuiqt, QWidget *parent )
    : QListView( parent )
    , yuiqt( yuiqt )
    , mousePressedItem( 0 )
    , mousePressedCol( -1 )
    , mousePressedButton( NoButton )
{
    y2debug( "Creating selection list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    addColumn( _( "Selection"	) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );


    connect( this, 	SIGNAL( pkgSelClicked		( int, YQPkgSel *, int ) ),
	     this, 	SLOT  ( slotPkgSelClicked	( int, YQPkgSel *, int ) ) );

    connect( this, 	SIGNAL( pkgSelDoubleClicked	( int, YQPkgSel *, int ) ),
	     this, 	SLOT  ( slotPkgSelClicked	( int, YQPkgSel *, int ) ) );

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
    y2milestone( "Filling selection list" );

    PMManager::PMSelectableVec::const_iterator it = Y2PM::selectionManager().begin();

    while ( it != Y2PM::selectionManager().end() )
    {
	PMSelectionPtr sel = (*it)->theObject();

	if ( sel )
	{
	    if ( sel->visible() && ! sel->isBase() )
	    {
#if 0
		// DEBUG
		std::string key = sel->order();
		if ( key.empty() )
		{
		    std::string name = sel->name();
		    y2warning( "Empty sort key for selection %s", name.c_str() );
		}
		// DEBUG
#endif
		new YQPkgSel( this, sel );
	    }
	}

	++it;
    }

    y2milestone( "Selection list filled" );
}


void
YQPkgSelList::selectSomething()
{
    QListViewItem * item = firstChild();

    if ( item )
    {
	setSelected( item, true );
    }
}


void
YQPkgSelList::updateAllItemStates()
{
    QListViewItem * item = firstChild();

    while ( item )
    {
	YQPkgSel * sel = dynamic_cast<YQPkgSel *> ( item );
	
	if ( sel )
	{
	    // Maybe in some future version this list will contain other types
	    // of items, too - so always use a dynamic cast and check for Null.
	    
	    sel->setStatusIcon();
	}
	
	item = item->nextSibling();
    }
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
	PMSelectionPtr sel = selection()->pkgSel();

	if ( sel )
	{
	    std::list<PMSelectablePtr> slcList = sel->inspacks_ptrs();
	    std::list<PMSelectablePtr>::const_iterator it = slcList.begin();

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
YQPkgSelList::addPkgSel( PMSelectionPtr sel )
{
    new YQPkgSel( this, sel);
}


YQPkgSel *
YQPkgSelList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgSel *> ( selectedItem() );
}


void
YQPkgSelList::slotPkgSelClicked( int button, YQPkgSel * sel, int col )
{
    if ( sel )
    {
	if ( button == Qt::LeftButton )
	{
	    if ( col == statusCol() )
		      // || col == summaryCol() )
	    {
		sel->cycleStatus();
	    }
	}
    }
}


void
YQPkgSelList::contentsMousePressEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	mousePressedItem	= item;
	mousePressedCol		= header()->sectionAt( ev->pos().x() );
	mousePressedButton	= ev->button();
    }
    else	// invalidate last click data
    {
	mousePressedItem	= 0;
	mousePressedCol		= -1;
	mousePressedButton	= -1;
    }

    // Call base class method
    QListView::contentsMousePressEvent( ev );
}


void
YQPkgSelList::contentsMouseReleaseEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() && item == mousePressedItem )
    {
	int col = header()->sectionAt( ev->pos().x() );

	if ( item == mousePressedItem	&&
	     col  == mousePressedCol	&&
	     ev->button() == mousePressedButton )
	{
	    emit( pkgSelClicked( ev->button(), (YQPkgSel *) item, col ) );
	}

    }

    // invalidate last click data

    mousePressedItem	= 0;
    mousePressedCol	= -1;
    mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseReleaseEvent( ev );
}


void
YQPkgSelList::contentsMouseDoubleClickEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	int col = header()->sectionAt( ev->pos().x() );
	emit( pkgSelDoubleClicked( ev->button(), (YQPkgSel *) item, col ) );
    }

    // invalidate last click data

    mousePressedItem	= 0;
    mousePressedCol	= -1;
    mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseDoubleClickEvent( ev );
}






YQPkgSel::YQPkgSel( YQPkgSelList * pkgSelList, PMSelectionPtr pkgSel )
    : QListViewItem( pkgSelList )
    , _pkgSelList( pkgSelList )
    , _pkgSel( pkgSel )
{
    QString text = fromUTF8( _pkgSel->summary( Y2PM::getPreferredLocale() ) );
    text.replace( QRegExp( "Graphical Basis System" ), "Graphical Base System" );
    setText( summaryCol(), text );

    _isInstalled = pkgSel->hasInstalledObj();
    setStatusIcon();
}



YQPkgSel::~YQPkgSel()
{

}


PMSelectable::UI_Status
YQPkgSel::status() const
{
    if ( ! _pkgSel )
    {
	y2error( "NULL package" );
	return isInstalled() ? PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
    }

    return _pkgSel->getSelectable()->status();
}


void
YQPkgSel::setStatus( PMSelectable::UI_Status newStatus )
{
    _pkgSel->getSelectable()->set_status( newStatus );
    setStatusIcon();
}


void
YQPkgSel::setStatusIcon()
{
    QPixmap icon = YQIconPool::pkgNoInst();

    switch ( status() )
    {
        case PMSelectable::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
        case PMSelectable::S_Del:		icon = YQIconPool::pkgDel();		break;
        case PMSelectable::S_Update:		icon = YQIconPool::pkgUpdate();		break;
        case PMSelectable::S_Install:		icon = YQIconPool::pkgInstall();	break;
        case PMSelectable::S_AutoDel:		icon = YQIconPool::pkgAutoDel();	break;
        case PMSelectable::S_AutoInstall:	icon = YQIconPool::pkgAuto();		break;
        case PMSelectable::S_AutoUpdate:	icon = YQIconPool::pkgAuto();		break;
        case PMSelectable::S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
        case PMSelectable::S_NoInst:		icon = YQIconPool::pkgNoInst();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    setPixmap( statusCol(), icon );
}


void
YQPkgSel::cycleStatus()
{
    PMSelectable::UI_Status oldStatus = status();
    PMSelectable::UI_Status newStatus = oldStatus;

    if ( isInstalled() )
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_KeepInstalled:	newStatus = _pkgSel->hasCandidateObj() ?
						    PMSelectable::S_Update : PMSelectable::S_Del;
						break;
	    case PMSelectable::S_Update:	newStatus = PMSelectable::S_Del;		break;
	    case PMSelectable::S_Del:		newStatus = PMSelectable::S_KeepInstalled;	break;
	    default:				newStatus = PMSelectable::S_KeepInstalled;	break;
	}
    }
    else	// pkg not installed
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_NoInst:	newStatus = PMSelectable::S_Install;	break;
	    default:				newStatus = PMSelectable::S_NoInst;	break;

		// Intentionally NOT cycling through YQPkgTaboo:
		// This status is not common enough for that.
	}
    }

    setStatus( newStatus );

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
YQPkgSel::compare( QListViewItem *	otherListViewItem,
		   int			col,
		   bool			ascending ) const
{
    YQPkgSel * other = (YQPkgSel *) otherListViewItem;

    if ( ! _pkgSel || ! other || ! other->pkgSel() )
	return 0;

    return _pkgSel->order().compare( other->pkgSel()->order() );
}



#include "YQPkgSelList.moc.cc"
