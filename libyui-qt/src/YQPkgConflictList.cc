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

  Textdomain "packages-qt"

/-*/


#include <qaction.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qmessagebox.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <Y2PM.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMPackageManager.h>

#include "YQPkgConflictList.h"
#include "YQPkgConflictDialog.h"
#include "YQIconPool.h"

#include "YQUI.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


#define LIST_SPLIT_THRESHOLD	8

#define IGNORED_CONFLICT_FILE	"/var/adm/YaST/ignored-conflicts"

#define RED			QColor( 0xC0, 0, 0 )
#define BRIGHT_RED		QColor( 0xFF, 0, 0 )
#define BLUE			QColor( 0, 0, 0xC0 )
#define LIGHT_BLUE		QColor( 0xE0, 0xE0, 0xF8 )
#define LIGHT_GREY		QColor( 0xE0, 0xE0, 0xE0 )
#define MAGENTA			Qt::magenta
#define DEEP_ORANGE		QColor( 0xFF, 0x80, 0x20 )
#define LIGHT_ORANGE		QColor( 0xFF, 0xC0, 0x50 )


// The conflict ignore list

QMap<QString, bool>	YQPkgConflict::_ignore;
QAction *		YQPkgConflict::_actionResetIgnoredConflicts = 0;


YQPkgConflictList::YQPkgConflictList( QWidget * parent )
    : QY2ListView( parent )
{
    addColumn( _( "Dependency Conflict" ) );
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
    string text;

    list<PkgDep::ErrorResult>::iterator it = badList.begin();

    while ( it != badList.end() )
    {
	YQPkgConflict * conflict = new YQPkgConflict( this, *it );
	CHECK_PTR( conflict );

	if ( conflict->isIgnored() )
	{
	    // y2debug( "Ignoring conflict: %s", (const char *) conflict->text(0) );
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
YQPkgConflictList::ignoreAll()
{
    QListViewItem * child = firstChild();

    while ( child )
    {
	YQPkgConflict * conflict = dynamic_cast<YQPkgConflict *> (child);

	if ( conflict )
	    conflict->ignore();

	child = child->nextSibling();
    }

    clear();
}


void
YQPkgConflictList::askSaveToFile() const
{
    QString filename = YQUI::ui()->askForSaveFileName( "conflicts.txt",	// startsWith
							   "*.txt",		// filter
							   _( "Save Conflicts List" ) );
    if ( ! filename.isEmpty() )
	saveToFile( filename, true );
}


void
YQPkgConflictList::saveToFile( const QString filename, bool interactive ) const
{
    // Open file

    FILE * file = fopen( (const char *) filename, "w" );

    if ( ! file )
    {
	y2error( "Can't open file %s", (const char *) filename );

	if ( interactive )
	{
	    // Post error popup.

	    QMessageBox::warning( 0,						// parent
				  _( "Error" ),					// caption
				  _( "Cannot open file %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
	return;
    }


    // Write header

    QString header = "#### YaST2 conflicts list - generated ";
    header += QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" );
    header += " ####\n\n";

    fputs( (const char *) header, file );


    // Recursively write all items

    const QListViewItem * item = firstChild();

    while ( item )
    {
	saveItemToFile( file, item );
	item = item->nextSibling();
    }


    // Write footer

    fprintf( file, "\n#### YaST2 conflicts list END ###\n" );


    // Clean up

    if ( file )
	fclose( file );
}


void
YQPkgConflictList::saveItemToFile( FILE * 			file,
				   const QListViewItem * 	item ) const
{
    if ( ! item || ! file )
	return;

    // Write indentation

    for ( int level = 0; level < item->depth(); level++ )
	fprintf( file, "    " );


    // Write item

    const QCheckListItem * checkListItem = dynamic_cast<const QCheckListItem *> (item);

    if ( checkListItem )
    {
	switch ( checkListItem->type() )
	{
	    case QCheckListItem::CheckBox:
		fprintf( file, "[%c] ", checkListItem->isOn() ? 'x' : ' ' );
		break;
	    case QCheckListItem::RadioButton:
		fprintf( file, "(%c) ", checkListItem->isOn() ? 'x' : ' ' );
		break;
	    default:
		break;
	}
    }

    fprintf( file, "%s\n", (const char *) item->text(0) );


    if ( item->isOpen() )
    {
	// Recursively write children

	const QListViewItem * child = item->firstChild();

	while ( child )
	{
	    saveItemToFile( file, child );
	    child = child->nextSibling();
	}
    }
}




YQPkgConflict::YQPkgConflict( YQPkgConflictList *		parentList,
			      const PkgDep::ErrorResult &	errorResult )
    : QY2ListViewItem( parentList )
    , _conflict( errorResult )
    , _parentList( parentList )
{
    string name;
    PkgEdition edition;

    _resolutionsHeader	= 0;
    _status		= PMSelectable::S_NoInst;
    _undo_status	= PMSelectable::S_NoInst;
    _pmObj		= _conflict.solvable;
    _isPkg		= true;
    _canIgnore		= true;

    if ( _pmObj )
    {
	if ( ! PMPackagePtr( _pmObj ) )
	    _isPkg	= false;

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
	_fullName += edition.asString().c_str();
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
	    // Select one from a number of functionalities ( Window manager etc. )
	    // e.g., "Select window manager"
	    text = ( _( "Select %1" ) ).arg( _shortName );
	    setTextColor( BLUE );
	    icon = YQIconPool::selectPkgConflict();
	}
	else
	{
	    // ( Pseudo ) package / functionality %1 missing, e.g.,
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

		    if ( _isPkg )
			// Package %1 is set to taboo, yet other packages require it
			text = ( _( "Taboo package %1 is required by other packages" ) ).arg( _shortName );
		    else
			text = ( _( "Taboo selection %1 is required by other selections" ) ).arg( _shortName );

		    icon = YQIconPool::tabooPkgConflict();
		    break;

                case PMSelectable::S_AutoDel:
                case PMSelectable::S_Del:
		    if ( _isPkg )
			// Package %1 is marked for deletion, yet other packages require it
			text = ( _( "Deleting %1 breaks other packages" ) ).arg( _shortName );
		    else
			text = ( _( "Deleting %1 breaks other selections" ) ).arg( _shortName );
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
		if ( more < 20 )	sublist->setBackgroundColor( LIGHT_ORANGE );
		else			sublist->setBackgroundColor( DEEP_ORANGE  );
		parent = sublist;
	    }
	}

	string pkg1 = ( *it ).name;
	string pkg2 = ( *it).rel.asString();
	QString text;

	switch ( ( *it ).kind )
	{
	    case PkgDep::RelInfo::REQUIREMENT:
		// "somepackage requires libfoo.so > 1.2"
		// "somepackage requires otherpackage"
		text =( _( "%1 requires %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() );
		break;
		
	    case PkgDep::RelInfo::CONFLICT:
		// "somepackage conflicts with otherpackage"
		text = ( _( "%1 conflicts with %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() );
		break;
			
	    case PkgDep::RelInfo::OBSOLETION:
		// "somepackage obsoletes otherpackage"
		text = ( _( "%1 obsoletes %2" ) ).arg( pkg1.c_str() ).arg( pkg2.c_str() );

		// RPM will always do nasty things if one package obsoletes another.
		// This kind of conflict cannot be ignored.
		_canIgnore = false;
		break;
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
						      _( "Conflict Resolution:" ),
						      QCheckListItem::Controller,
						      true );
    CHECK_PTR( header );
    header->setOpen( true );
    header->setBackgroundColor( LIGHT_GREY );

    addUndoResolution  ( header );
    addAlternativesList( header );
    addDeleteConflictsResolution( header );
    addDeleteReferersResolution( header );

    if ( _canIgnore )
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
	    text = ( _( "Do Not Set %1 to Taboo" ) ).arg( _shortName );
	    _undo_status = _pmObj->hasInstalledObj() ?
		PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_Protected:
	    text = ( _( "Do Not Set %1 to Protected" ) ).arg( _shortName );
	    _undo_status = _pmObj->hasInstalledObj() ?
		PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_Del:
	case PMSelectable::S_AutoDel:
	    text = ( _( "Do Not Delete %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoUpdate:
	case PMSelectable::S_Update:
	    text = ( _( "Do Not Update %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_KeepInstalled;
	    break;

	case PMSelectable::S_AutoInstall:
	case PMSelectable::S_Install:
	    text = ( _( "Do Not Install %1" ) ).arg( _shortName );
	    _undo_status = PMSelectable::S_NoInst;
	    break;

	case PMSelectable::S_KeepInstalled:
	    if(!_conflict.is_downgrade_from.is_unspecified())
	    {
		// %1 package name, %2 version
		text = ( _( "Downgrade %1 to Version %2" ) ).arg(
		    _shortName+"-"+_conflict.is_downgrade_from.asString().c_str() ).arg(
			_conflict.edition.asString().c_str());
		_undo_status = PMSelectable::S_Update;
		break;
	    }
	    else
		return;	// shouldn't happen
	case PMSelectable::S_NoInst:		return;	// shouldn't happen
    }

    new YQPkgConflictResolution( parent, text, YQPkgConflictUndo );
}


void
YQPkgConflict::addAlternativesList( QY2CheckListItem * parent )
{
    if ( _conflict.alternatives.empty() )
	return;

    list<PkgDep::Alternative>::const_iterator it = _conflict.alternatives.begin();

    while ( it != _conflict.alternatives.end() )
    {
	PMObjectPtr pkg = ( *it ).solvable;

	if ( pkg )
	    new YQPkgConflictResolution( parent, pkg );

	++it;
    }
}


void
YQPkgConflict::addDeleteConflictsResolution( QY2CheckListItem * parent )
{
    if ( _conflict.remove_to_solve_conflict.empty() )
	return;

    QString text;

    if ( _conflict.remove_to_solve_conflict.size() == 1 )
    {
	if ( _isPkg )
	    text = _( "Remove the Conflicting Package" );
	else
	    text = _( "Remove the Conflicting Selection" );
    }
    else
    {
	if ( _isPkg )
	    text = ( _( "Remove All %1 Conflicting Packages" ) ).arg( _conflict.remove_to_solve_conflict.size() );
	else
	    text = ( _( "Remove All %1 Conflicting Selections" ) ).arg( _conflict.remove_to_solve_conflict.size() );
    }

    YQPkgConflictResolution * res =
	new YQPkgConflictResolution( parent, text, YQPkgConflictDeleteConflictors );

    CHECK_PTR( res );
    res->setOpen( true );

    dumpDeleteList( res, _conflict.remove_to_solve_conflict );
}


void
YQPkgConflict::addDeleteReferersResolution( QY2CheckListItem * parent )
{
    if ( _conflict.remove_referers.empty() )
	return;

    QString text;

    if ( _conflict.remove_referers.size() == 1 )
    {
	if ( _isPkg )
	    text = _( "Remove the Referring Package" );
	else
	    text = _( "Remove the Referring Selection" );
    }
    else
    {
	if ( _isPkg )
	    text = ( _( "Remove All %1 Referring Packages" ) ).arg( _conflict.remove_referers.size() );
	else
	    text = ( _( "Remove All %1 Referring Selections" ) ).arg( _conflict.remove_referers.size() );
    }

    YQPkgConflictResolution * res =
	new YQPkgConflictResolution( parent, text, YQPkgConflictDeleteReferers );

    CHECK_PTR( res );
    res->setOpen( true );

    dumpDeleteList( res, _conflict.remove_referers );
}


void
YQPkgConflict::dumpDeleteList( QListViewItem * parent, PkgDep::SolvableList& solvablelist )
{
    if ( ! parent )
    {
	y2error( "Null parent" );
	return;
    }

    int	 splitThreshold = LIST_SPLIT_THRESHOLD;
    bool doSplit	= solvablelist.size() > (unsigned) splitThreshold + 3;
    bool didSplit	= false;
    int  count		= 0;
    list<PMSolvablePtr>::const_iterator it = solvablelist.begin();

    while ( it != solvablelist.end() )
    {
	if ( doSplit && ! didSplit && ++count > splitThreshold )
	{
	    // Split list

	    int more = solvablelist.size() - count + 1;
	    QString text = ( _( "%1 more..." ) ).arg( more );
	    QY2ListViewItem * sublist = new QY2ListViewItem( parent, text, true );
	    didSplit = true;

	    if ( sublist )
	    {
		if ( more < 20 )	sublist->setBackgroundColor( LIGHT_ORANGE );
		else			sublist->setBackgroundColor( DEEP_ORANGE  );
		parent = sublist;
	    }
	}

	PMObjectPtr pkg = ( *it );

	if ( pkg )
	{
	    QString name = pkg->name().asString().c_str();
	    QString text;

	    if ( pkg->hasInstalledObj() )
		text = ( _( "Delete %1" ) ).arg( name );
	    else
		text = ( _( "Do Not Install %1" ) ).arg( name );


	    QY2ListViewItem * item = new QY2ListViewItem( parent, text, true );
	    CHECK_PTR( item );
	    item->setTextColor( BLUE );
	}

	++it;
    }
}


void
YQPkgConflict::addIgnoreResolution( QY2CheckListItem * parent )
{
    new YQPkgConflictResolution( parent,
				 _( "Ignore Conflict and Risk System Inconsistencies" ),
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
    updateActions();
}


bool
YQPkgConflict::isIgnored( const QString & conflictHeader )
{
    return _ignore.contains( conflictHeader );
}


void
YQPkgConflict::ignore( const QString & conflictHeader )
{
    y2milestone( "Ignoring dependency conflict: %s", (const char *) conflictHeader );
    _ignore.insert( conflictHeader, true );
}


void
YQPkgConflict::resetIgnoredConflicts()
{
    y2milestone( "Resetting all ignored dependency conflicts" );
    _ignore.clear();
    updateActions();
}


void
YQPkgConflict::loadIgnoredConflicts()
{
    char buffer[ 1024 ];
    FILE * file = fopen( IGNORED_CONFLICT_FILE, "r" );

    if ( ! file )
    {
	y2milestone( "No ignored dependency conflicts" );
	// This is not a warning since this should be the most usual case.

	return;
    }

    int count = 0;

    while ( ! feof( file ) )
    {
	buffer[0] = '\0';
	fgets( buffer, sizeof( buffer ), file );
	int len = strlen( buffer );

	if ( len > 0 )
	{
	    // Chop off trailing newline ( left over by fgets() )

	    if ( buffer[ len-1 ] == '\n' )
		buffer[ len-1 ] = '\0';

	    _ignore.insert( QString( buffer ), true );
	    y2milestone( "Ignoring dependency conflict: \"%s\"", buffer );
	    count++;
	}
    }

    if ( count > 0 )
    {
	y2milestone( "Loaded %d ignored dependency conflict%s from %s",
		     count, count > 1 ? "s" : "", IGNORED_CONFLICT_FILE );
    }

    fclose( file );
    updateActions();
}


void
YQPkgConflict::saveIgnoredConflicts()
{
    if ( _ignore.empty() )
    {
	// Remove any previous ignore file, but don't create a new one with zero length

	remove( IGNORED_CONFLICT_FILE );
	return;
    }

    FILE * file = fopen( IGNORED_CONFLICT_FILE, "w" );

    if ( ! file )
    {
	y2error( "Can't save ignored conflicts to %s: %s",
		 IGNORED_CONFLICT_FILE, strerror( errno ) );
	return;
    }

    QMap<QString, bool>::const_iterator it = _ignore.begin();

    while ( it != _ignore.end() )
    {
	fprintf( file, "%s\n", (const char *) it.key() );
	++it;
    }

    fclose( file );
}


bool
YQPkgConflict::haveIgnoredConflicts()
{
    return ! _ignore.empty();
}


QAction *
YQPkgConflict::actionResetIgnoredConflicts( YQPkgConflictDialog * dialog )
{
    if ( ! _actionResetIgnoredConflicts )
    {
	QString label = _( "Reset &Ignored Dependency Conflicts" );
	_actionResetIgnoredConflicts = new QAction( label,		// text
						    label,		// menu text
						    ( QKeySequence ) 0,	// accel
						    ( QObject * ) 0 ); 	// parent
	
	_actionResetIgnoredConflicts->setEnabled( ! _ignore.empty() );
    }

    if ( _actionResetIgnoredConflicts && dialog )
    {
	QObject::connect( _actionResetIgnoredConflicts,	SIGNAL( activated() ),
			  dialog,			SLOT  ( resetIgnoredConflicts() ) );
    }
	
    return _actionResetIgnoredConflicts;
}


void
YQPkgConflict::updateActions()
{
    if ( _actionResetIgnoredConflicts )
    {
	_actionResetIgnoredConflicts->setEnabled( ! _ignore.empty() );
    }
}


void
YQPkgConflict::paintCell( QPainter *		painter,
			  const QColorGroup &	colorGroup,
			  int			column,
			  int			width,
			  int			alignment )
{
    painter->setFont( YQUI::ui()->headingFont() );
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

		case YQPkgConflictDeleteConflictors:
		    bruteForceDelete(_conflict.remove_to_solve_conflict);
		    return;

		case YQPkgConflictDeleteReferers:
		    bruteForceDelete(_conflict.remove_referers);
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
YQPkgConflict::bruteForceDelete(PkgDep::SolvableList& solvablelist)
{
    if ( solvablelist.empty() )
	return;

    list<PMSolvablePtr>::const_iterator it = solvablelist.begin();

    while ( it != solvablelist.end() )
    {
	PMObjectPtr pkg = ( *it );

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
    setText( 0, ( _( "Install %1" ) ).arg( _pmObj->name().asString().c_str() ) );
}


const char *
YQPkgConflictResolution::typeString() const
{
    const char * text = "<unknown>";

    switch ( _type )
    {
	case YQPkgConflictUndo:			text = "Undo";			break;
	case YQPkgConflictIgnore:		text = "Ignore";		break;
	case YQPkgConflictDeleteConflictors:	text = "DeleteConflictors";	break;
	case YQPkgConflictDeleteReferers:	text = "DeleteReferers";	break;
	case YQPkgConflictAlternative:		text = "Alternative";		break;
    };

    return text;
}


#include "YQPkgConflictList.moc"
