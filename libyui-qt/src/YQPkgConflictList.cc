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

#define RED			QColor( 0xC0, 0, 0 )
#define BRIGHT_RED		QColor( 0xFF, 0, 0 )
#define BLUE			QColor( 0, 0, 0xC0 )
#define LIGHT_BLUE		QColor( 0xE0, 0xE0, 0xF8 )
#define LIGHT_GREY		QColor( 0xE0, 0xE0, 0xE0 )
#define MAGENTA			Qt::magenta


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
	YQPkgConflict * conflict = new YQPkgConflict( this, *it );
	CHECK_PTR( conflict );

	if ( conflict->isIgnored() )
	{
	    // y2milestone( "Ignoring conflict: %s", (const char *) conflict->text(0) );
	    delete conflict;	// Yes, this is stupid. The solver should handle that.
	}

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
YQPkgConflictList::applyResolutions()
{
    QListViewItem * child = firstChild();

    while ( child )
    {
	YQPkgConflict * conflict = dynamic_cast<YQPkgConflict *> (child);

	if ( conflict )
	    conflict->applyResolution();

	child = child->nextSibling();
    }

    emit updatePackages();
}


void
YQPkgConflictList::resetIgnoredConflicts()
{
    YQPkgConflict::resetIgnoredConflicts();
}






QMap<QString, bool> YQPkgConflict::_ignore;


YQPkgConflict::YQPkgConflict( YQPkgConflictList *		parentList,
			      const PkgDep::ErrorResult &	errorResult )
    : QY2ListViewItem( parentList )
    , _conflict( errorResult )
    , _parentList( parentList )
{
    std::string name;
    PkgEdition edition;

    _resolutionsHeader	= 0;
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

    setBackgroundColor( LIGHT_BLUE );
    setOpen( true );

#if 0
    // DEBUG
    // DEBUG

    if ( _pmObj )
    {
	for ( int i=0; i < 30; i++ )
	    _conflict.remove_to_solve_conflict.push_back( _pmObj );
    }

    // DEBUG
    // DEBUG
#endif

    formatHeading();

    if ( isIgnored() )	// Don't bother creating any child items etc.
	return;

    dumpLists();
    addResolutionSuggestions();
}


void
YQPkgConflict::formatHeading()
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
	    setTextColor( BLUE );
	    icon = YQIconPool::selectPkgConflict();
	}
	else
	{
	    // (Pseudo) package / functionality %1 missing, e.g.,
	    // "libfoo.so.1.0 not available"
	    text = ( _( "%1 not available" ) ).arg( _fullName );
	    setTextColor( BRIGHT_RED );
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

	    setTextColor( BRIGHT_RED );

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
#if 0
    if ( _conflict.state_change_not_possible ||
	 ! _pmObj )
#endif
    {
	dumpList( this, _conflict.referers, LIST_SPLIT_THRESHOLD,
		  _( "Required by:" ) );
    }

    dumpList( this, _conflict.unresolvable, LIST_SPLIT_THRESHOLD,
	      _( "Unresolved Requirements:" ) );

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

	    int more = list.size() - count + 1;
	    QString text = ( _( "%1 more..." ) ).arg( more );
	    QY2ListViewItem * sublist = new QY2ListViewItem( parent, text, true );
	    didSplit = true;

	    if ( sublist )
	    {
		if ( more < 20 )	sublist->setTextColor( BLUE );
		else			sublist->setTextColor( MAGENTA );
		parent = sublist;
	    }
	}

	std::string pkg1 = (*it).name;
	std::string pkg2 = PkgRelation::toString( (*it).rel );
	QString text;

	if ( (*it).is_conflict )
	{
	    // "somepackage conflicts with otherpackage"
	    text = ( _( "%1 conflicts with %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() );
	}
	else
	{
	    // "somepackage requires libfoo.so > 1.2"
	    // "somepackage requires otherpackage"
	    text =( _( "%1 requires %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() );
	}

	new QY2ListViewItem( parent, text, true );
	++it;
    }
}


void
YQPkgConflict::addResolutionSuggestions()
{
    QY2CheckListItem * header = new QY2CheckListItem( this,
						      // Heading for the choices
						      // how to resolve this conflict
						      _( "Conflict resolution:" ),
						      QCheckListItem::Controller,
						      true );
    CHECK_PTR( header );
    header->setOpen( true );
    header->setBackgroundColor( LIGHT_GREY );

    addUndoResolution  ( header );
    addAlternativesList( header );
    addDeleteResolution( header );
    addIgnoreResolution( header );

    _resolutionsHeader = header;
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
	    text = ( _( "Do not set %1 to taboo" ) ).arg( _shortName );
	    _undo_status = _pmObj->hasInstalledObj() ?
		PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_Del:
	case PMSelectable::S_AutoDel:
	    text = ( _( "Do not delete %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoUpdate:
	case PMSelectable::S_Update:
	    text = ( _( "Do not update %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoInstall:
	case PMSelectable::S_Install:
	    text = ( _( "Do not install %1" ) ).arg( _shortName );
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
    if ( _conflict.alternatives.empty() )
	return;

    std::list<PkgDep::Alternative>::const_iterator it = _conflict.alternatives.begin();

    while ( it != _conflict.alternatives.end() )
    {
	PMObjectPtr pkg = (*it).solvable;

	if ( pkg )
	    new YQPkgConflictResolution( parent, pkg );

	++it;
    }
}


void
YQPkgConflict::addDeleteResolution( QY2CheckListItem * parent )
{
    if ( _conflict.remove_to_solve_conflict.empty() )
	return;

    QString text;

    if ( _conflict.remove_to_solve_conflict.size() == 1 )
	text = _( "Remove the conflicting package" );
    else
	text = ( _( "Remove all %1 conflicting packages" ) ).arg( _conflict.remove_to_solve_conflict.size() );

    YQPkgConflictResolution * res =
	new YQPkgConflictResolution( parent, text, YQPkgConflictBruteForceDelete );

    CHECK_PTR( res );
    res->setOpen( true );

    dumpDeleteList( res );
}


void
YQPkgConflict::dumpDeleteList( QListViewItem * parent )
{
    if ( ! parent )
    {
	y2error( "Null parent" );
	return;
    }

    int	 splitThreshold = LIST_SPLIT_THRESHOLD;
    bool doSplit	= _conflict.remove_to_solve_conflict.size() > (unsigned) splitThreshold + 3;
    bool didSplit	= false;
    int  count		= 0;
    std::list<PMSolvablePtr>::const_iterator it = _conflict.remove_to_solve_conflict.begin();

    while ( it != _conflict.remove_to_solve_conflict.end() )
    {
	if ( doSplit && ! didSplit && ++count > splitThreshold )
	{
	    // Split list

	    int more = _conflict.remove_to_solve_conflict.size() - count + 1;
	    QString text = ( _( "%1 more..." ) ).arg( more );
	    QY2ListViewItem * sublist = new QY2ListViewItem( parent, text, true );
	    didSplit = true;

	    if ( sublist )
	    {
		if ( more < 20 )	sublist->setTextColor( BLUE );
		else			sublist->setTextColor( MAGENTA );
		parent = sublist;
	    }
	}

	PMObjectPtr pkg = (*it);

	if ( pkg )
	{
	    std::string name = pkg->name();
	    QString text;

	    if ( pkg->hasInstalledObj() )
		text = ( _( "Delete %1" ) ).arg( name.c_str() );
	    else
		text = ( _( "Do not install %1" ) ).arg( name.c_str() );


	    new QY2ListViewItem( parent, text, true );
	}

	++it;
    }
}


void
YQPkgConflict::addIgnoreResolution( QY2CheckListItem * parent )
{
    new YQPkgConflictResolution( parent,
				 _( "Ignore this conflict and risk system inconsistencies" ),
				 YQPkgConflictIgnore );
}


bool
YQPkgConflict::isIgnored()
{
    return isIgnored( text(0) );
}


void
YQPkgConflict::ignore()
{
    ignore( text(0) );
}


bool
YQPkgConflict::isIgnored( const QString & conflictHeader )
{
    return _ignore.contains( conflictHeader );
}


void
YQPkgConflict::ignore( const QString & conflictHeader )
{
    _ignore.insert( conflictHeader, true );
}


void
YQPkgConflict::resetIgnoredConflicts()
{
    _ignore.clear();
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


void
YQPkgConflict::applyResolution()
{
    QListViewItem * item = _resolutionsHeader->firstChild();

    while ( item )
    {
	YQPkgConflictResolution * res = dynamic_cast<YQPkgConflictResolution *> (item);

	if ( ! res )
	    y2error( "Non-resolution item in resolution list!" );

	if ( res && res->isOn() )
	{
	    y2milestone( "Resolution %s selected for %s", res->typeString(), (const char *) _shortName );

	    switch ( res->type() )
	    {
		case YQPkgConflictUndo:
		    if ( _pmObj && _pmObj->getSelectable() )
			_pmObj->getSelectable()->set_status( _undo_status );
		    return;

		case YQPkgConflictIgnore:
		    ignore();
		    return;

		case YQPkgConflictBruteForceDelete:
		    bruteForceDelete();
		    return;

		case YQPkgConflictAlternative:
		    if ( res->pmObj() && res->pmObj()->getSelectable() )
		    {
			if ( res->pmObj()->hasInstalledObj() )
			    res->pmObj()->getSelectable()->set_status( PMSelectable::S_Update );
			else
			    res->pmObj()->getSelectable()->set_status( PMSelectable::S_Install );
		    }
		    return;
	    }
	}

	item = item->nextSibling();
    }

    y2milestone( "Conflict %s unresolved", (const char *) _shortName );
}


void
YQPkgConflict::bruteForceDelete()
{
    if ( _conflict.remove_to_solve_conflict.empty() )
	return;

    std::list<PMSolvablePtr>::const_iterator it = _conflict.remove_to_solve_conflict.begin();

    while ( it != _conflict.remove_to_solve_conflict.end() )
    {
	PMObjectPtr pkg = (*it);

	if ( pkg && pkg->getSelectable() )
	{
	    pkg->getSelectable()->set_status( pkg->hasInstalledObj() ?
					      PMSelectable::S_Del : PMSelectable::S_NoInst );
	}

	++it;
    }
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


const char *
YQPkgConflictResolution::typeString() const
{
    const char * text = "<unknown>";

    switch ( _type )
    {
	case YQPkgConflictUndo:			text = "Undo";			break;
	case YQPkgConflictIgnore:		text = "Ignore";		break;
	case YQPkgConflictBruteForceDelete:	text = "BruteForceDelete";	break;
	case YQPkgConflictAlternative:		text = "Alternative";		break;
    };

    return text;
}


#include "YQPkgConflictList.moc.cc"
