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

  File:	      YQPkgConflictList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpainter.h>
#include <qpixmap.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <Y2PM.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMObject.h>

#include "YQPkgConflictList.h"
#include "YQIconPool.h"

#include "YUIQt.h"
#include "YQi18n.h"
#include "utf8.h"


#define LIST_SPLIT_THRESHOLD	8


YQPkgConflictList::YQPkgConflictList( QWidget * parent )
    : QY2ListView( parent )
{
    addColumn( _("Dependency conflict") );
    setRootIsDecorated( true );
}


YQPkgConflictList::~YQPkgConflictList()
{
    // NOP
}


void
YQPkgConflictList::fill( PkgDep::ErrorResultList & badList )
{
    clear();
    std::string text;

    std::list<PkgDep::ErrorResult>::iterator it = badList.begin();

    while ( it != badList.end() )
    {
	new YQPkgConflict( this, *it );
	++it;
    }
}


bool
YQPkgConflictList::choicesComplete()
{
    // TODO
    return true;
}


void
YQPkgConflictList::activateUserChoices()
{
    // TODO
    // TODO
    // TODO
}






YQPkgConflict::YQPkgConflict( YQPkgConflictList *		parentList,
			      const PkgDep::ErrorResult &	errorResult )
    : QY2ListViewItem( parentList )
    , _conflict( errorResult )
    , _parentList( parentList )
{
    std::string name;
    PkgEdition edition;

    _firstResolution	= 0;
    _status		= PMSelectable::S_NoInst;
    _undo_status	= PMSelectable::S_NoInst;
    _pmObj		= _conflict.solvable;

    if ( _pmObj )
    {
	name		= _pmObj->name();
	edition		= _pmObj->edition();

	if ( _pmObj->getSelectable() )
	    _status	= _pmObj->getSelectable()->status();
    }
    else
    {
	name 		= _conflict.name;
	edition		= _conflict.edition;
    }

    _shortName		= name.c_str();
    _fullName 		= _shortName;

    if ( ! edition.is_unspecified() )
    {
	_fullName += " ";
	_fullName += ( PkgEdition::toString( edition ) ).c_str();
    }

    setBackgroundColor( QColor( 0xE0, 0xE0, 0xF8 ) );
    setOpen( true );

    formatLine();
    dumpLists();
    addResolutionSuggestions();
}


void
YQPkgConflict::formatLine()
{
    QString text;
    QPixmap icon = YQIconPool::normalPkgConflict();

    // Generic conflict with package %1
    // text = ( _( "%1 conflict" ) ).arg( _shortName );
    text = ( _( "%1 conflict" ) ).arg( _fullName );

    if ( ! _pmObj )
    {
	if ( needAlternative() )
	{
	    // Select one from a number of functionalities (Window manager etc.)
	    // e.g., "Select window manager"
	    text = ( _( "Select %1" ) ).arg( _shortName );
	    setTextColor( QColor( 0, 0, 0xC0 ) );
	    icon = YQIconPool::selectPkgConflict();
	}
	else
	{
	    // (Pseudo) package / functionality %1 missing, e.g.,
	    // "libfoo.so.1.0 not available"
	    text = ( _( "%1 not available" ) ).arg( _fullName );
	    setTextColor( Qt::red );
	    icon = YQIconPool::unresolvablePkgConflict();
	}
    }
    else
    {
	if ( _conflict.state_change_not_possible )
	{
	    // The solver would have liked to change this package's status,
	    // i.e. to add it automatically, but it couldn't.
	    //
	    // This means that the user has set this package to "remove" or "taboo",
	    // yet other packages still need it.

	    setTextColor( Qt::red );

	    switch ( _status )
	    {
		case PMSelectable::S_Taboo:
		    // Package %1 is set to taboo, yet other packages require it
		    text = ( _( "Taboo package %1 is required by other packages" ) ).arg( _shortName );
		    icon = YQIconPool::tabooPkgConflict();
		    break;

                case PMSelectable::S_AutoDel:
                case PMSelectable::S_Del:
		    // Package %1 is marked for deletion, yet other packages require it
		    text = ( _( "Deleting %1 breaks other packages" ) ).arg( _shortName );
		    icon = YQIconPool::deletePkgConflict();
		    break;

		default: // leave generic text
		    break;
	    }
	}
	else
	{
	    // Conflict while installing or updating package

	    // leave generic text
	}

    }

    setText( 0, text );
    setPixmap( 0, icon );
}


void
YQPkgConflict::dumpLists()
{
    if ( _conflict.state_change_not_possible ||
	 ! _pmObj )
    {
	dumpList( this, _conflict.referers, LIST_SPLIT_THRESHOLD,
		  _( "Required by:" ) );
    }

    dumpList( this, _conflict.unresolvable, LIST_SPLIT_THRESHOLD,
	      _( "Unresolved Reqirements:" ) );

    dumpList( this, _conflict.conflicts_with, LIST_SPLIT_THRESHOLD,
	      _( "Conflicts with:" ) );
}


void
YQPkgConflict::dumpList( QListViewItem * 	parent,
			 PkgDep::RelInfoList &	list,
			 int			splitThreshold,
			 const QString & 	header )
{
    if ( ! parent )
    {
	y2error( "Null parent" );
	return;
    }

    if ( list.empty() )
    {
	return;
    }

    if ( ! header.isEmpty() )
    {
	parent = new QY2ListViewItem( parent, header, true );
	CHECK_PTR( parent );
	parent->setOpen( true );
    }

    bool doSplit	= splitThreshold > 1 && list.size() > (unsigned) splitThreshold + 3;
    bool didSplit	= false;
    int  count		= 0;
    PkgDep::RelInfoList_const_iterator it = list.begin();


    while ( it != list.end() )
    {
	if ( doSplit && ! didSplit && ++count > splitThreshold )
	{
	    // Split list

	    QString text = ( _( "%1 more..." ) ).arg( list.size() - count );
	    QY2ListViewItem * sublist = new QY2ListViewItem( parent, text, true );
	    didSplit = true;

	    if ( sublist )
	    {
		sublist->setBackgroundColor( QColor( 0xE0, 0xE0, 0xE0 ) );
		parent = sublist;
	    }
	}

	std::string pkg1 = (*it).name;
	std::string pkg2 = PkgRelation::toString( (*it).rel );
	new QY2ListViewItem( parent,
			     // "somepackage requires libfoo.so > 1.2"
			     // "somepackage requires otherpackage"
			     ( _( "%1 requires %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() ),
			     true );
	++it;
    }
}


void
YQPkgConflict::addResolutionSuggestions()
{
    QY2CheckListItem * header = new QY2CheckListItem( this,
						      // Heading for the choices
						      // how to resolve this conflict
						      _( "Resolution" ),
						      QCheckListItem::Controller,
						      true );
    CHECK_PTR( header );
    header->setOpen( true );

    addUndoResolution  ( header );
    addAlternativesList( header );
    addDeleteResolution( header );
    addIgnoreResolution( header );

    _firstResolution = dynamic_cast<YQPkgConflictResolution *> ( header->firstChild() );
}


void
YQPkgConflict::addUndoResolution( QY2CheckListItem * parent )
{
    if ( ! _pmObj )
	return;

    QString text;

    switch ( _status )
    {
	case PMSelectable::S_Taboo:
	    text = ( _( "Don't set %1 to taboo" ) ).arg( _shortName );
	    _undo_status = _pmObj->hasInstalledObj() ?
		PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_Del:
	case PMSelectable::S_AutoDel:
	    text = ( _( "Don't delete %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoUpdate:
	case PMSelectable::S_Update:
	    text = ( _( "Don't update %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoInstall:
	case PMSelectable::S_Install:
	    text = ( _( "Don't install %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_KeepInstalled:	return;	// shouldn't happen
	case PMSelectable::S_NoInst:		return;	// shouldn't happen
    }

    new YQPkgConflictResolution( parent, text, YQPkgConflictUndo );
}


void
YQPkgConflict::addAlternativesList( QY2CheckListItem * parent )
{

}


void
YQPkgConflict::addDeleteResolution( QY2CheckListItem * parent )
{
    QString text;
    int conflictingPkgsCount = (int) _conflict.remove_to_solve_conflict.size();

    if ( conflictingPkgsCount < 1 )
	return;

    if ( conflictingPkgsCount == 1 )
	text = _( "Remove the conflicting package" );
    else
	text = ( _( "Remove all %1 conflicting packages" ) ).arg( conflictingPkgsCount );

    new YQPkgConflictResolution( parent, text, YQPkgConflictBulkDelete );
}


void
YQPkgConflict::addIgnoreResolution( QY2CheckListItem * parent )
{
    new YQPkgConflictResolution( parent,
				 _( "Ignore this conflict and risk inconsistent system" ),
				 YQPkgConflictIgnore );
}


void
YQPkgConflict::paintCell( QPainter *		painter,
			  const QColorGroup &	colorGroup,
			  int			column,
			  int			width,
			  int			alignment )
{
    painter->setFont( YUIQt::yuiqt()->headingFont() );
    QY2ListViewItem::paintCell( painter, colorGroup, column, width, alignment );
}


bool
YQPkgConflict::isResolved()
{
#warning TODO: isResolved()

    return true;
}



YQPkgConflictResolution::YQPkgConflictResolution( QY2CheckListItem * 			parent,
						  const QString & 			text,
						  YQPkgConflictResolutionType		type )
    : QY2CheckListItem( parent, text, QCheckListItem::RadioButton, true )
    , _type( type )
{
}


YQPkgConflictResolution::YQPkgConflictResolution( QY2CheckListItem *	parent,
						  PMObjectPtr		pmObj )
    : QY2CheckListItem( parent, "", QCheckListItem::RadioButton, true )
    , _type( YQPkgConflictAlternative )
    , _pmObj( pmObj )
{
    std::string name = _pmObj->name();
    setText( 0, ( _( "Install %1" ) ).arg( name.c_str() ) );
}





#include "YQPkgConflictList.moc.cc"
