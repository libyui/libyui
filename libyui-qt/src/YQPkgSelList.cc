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


#define SHOW_SUMMARY	0

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qheader.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgSelList.h"
#include "YUIQt.h"
#include "YQi18n.h"
#include "YQIconPool.h"



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
    addColumn( _( "Selection"	) );	_nameCol	= numCol++;
#if SHOW_SUMMARY
    addColumn( _( "Summary"	) );	_summaryCol	= numCol++;
#else
    _summaryCol = -1;
#endif
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
	    y2milestone( "Found selection" );
	    
	    if ( sel->visible() && ! sel->isBase() )
	    {
		new YQPkgSel( this, sel );
	    }
	    else
		y2milestone( "Selection invisible or base selection" );
	}
	else
	    y2error( "NULL pointer selection" );
	
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
	    std::list<PMPackagePtr> pkgList = sel->inspacks_ptrs();
	    std::list<PMPackagePtr>::const_iterator it = pkgList.begin();

	    while ( it != pkgList.end() )
	    {
#if 0
		// DEBUG
		std::string name = (*it)->name();
		y2debug( "Found match for pkg '%s'", name.c_str() );
		// DEBUG
#endif
		emit filterMatch( *it );
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
		      // || col == nameCol() )
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
    y2debug( "New YQPkgSel" );
    
    _pkgSel->startRetrieval();	// Just a hint to speed things up a bit
    
    setText( nameCol(),		_pkgSel->name()		  );

#if SHOW_SUMMARY
    setText( summaryCol(),	_pkgSel->summary("")	  );
#endif
    

#if 0
    _isInstalled = pkgSel->hasInstalledObj();
#else
    _isInstalled = false;
#endif
    
    setStatus( _isInstalled ? YQPkgSelKeepInstalled : YQPkgSelNoInst );
    
    pkgSel->stopRetrieval();
}



YQPkgSel::~YQPkgSel()
{

}


void
YQPkgSel::setText( int column, const std::string text )
{
    QListViewItem::setText( column, QString::fromUtf8( text.c_str() ) );
}


void
YQPkgSel::setStatus( YQPkgSelStatus newStatus )
{
    _status = newStatus;
    QPixmap icon = YQIconPool::pkgNoInst();
    const char *badStatus = 0;

    switch ( status() )
    {
        case YQPkgSelTaboo:		icon = YQIconPool::pkgTaboo();		break;
        case YQPkgSelDel:		icon = YQIconPool::pkgDel();		break;
        case YQPkgSelUpdate:		icon = YQIconPool::pkgUpdate();		break;
        case YQPkgSelInstall:		icon = YQIconPool::pkgInstall();	break;
        case YQPkgSelAuto:		icon = YQIconPool::pkgAuto();		break;
        case YQPkgSelKeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
        case YQPkgSelNoInst:		icon = YQIconPool::pkgNoInst();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    if ( badStatus )
    {
	_status = YQPkgSelNoInst;
	icon = YQIconPool::pkgNoInst();
	y2error( "Ignoring invalid package selection status %s", badStatus );
    }

    setPixmap( statusCol(), icon );
}


void
YQPkgSel::cycleStatus()
{
    YQPkgSelStatus newStatus = status();

    if ( isInstalled() )
    {
	switch ( status() )
	{
	    case YQPkgSelKeepInstalled:	newStatus = YQPkgSelUpdate;		break;
	    case YQPkgSelUpdate:	newStatus = YQPkgSelDel;		break;
	    case YQPkgSelDel:		newStatus = YQPkgSelKeepInstalled;	break;
	    default:			newStatus = YQPkgSelKeepInstalled;	break;
	}
    }
    else	// pkgSel not installed
    {
	switch ( status() )
	{
	    case YQPkgSelNoInst:	newStatus = YQPkgSelInstall;		break;
	    case YQPkgSelInstall:	newStatus = YQPkgSelNoInst;		break;
	    case YQPkgSelAuto:		newStatus = YQPkgSelNoInst;		break;
	    default:			newStatus = YQPkgSelNoInst;		break;

		// Intentionally NOT cycling through YQPkgSelTaboo:
		// This status is not common enough for that.
	}
    }

    setStatus( newStatus );
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
