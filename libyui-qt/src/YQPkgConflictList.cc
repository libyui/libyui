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

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <Y2PM.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMObject.h>

#include "YQPkgConflictList.h"

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

    _firstAlternative	= 0;
    _firstResolution	= 0;
    _status		= PMSelectable::S_NoInst;
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
    formatLine();
    dumpLists();
    setOpen( true );
}


void
YQPkgConflict::formatLine()
{
    QString text;

    // Generic conflict with package %1
    // text = ( _( "%1 conflict" ) ).arg( _shortName );
    text = ( _( "%1 conflict" ) ).arg( _fullName );

    if ( ! _pmObj )
    {
	if ( needAlternative() )
	{
	    // Select one from a number of functionalities (Window manager etc.)
	    text = ( _( "Select %1" ) ).arg( _shortName );
	    setTextColor( QColor( 0, 0, 0xC0 ) );
	}
	else
	{
	    // (Pseudo) package / functionality %1 missing, e.g.,
	    // "libfoo.so.1.0 not available", "Window manager not available"
	    text = ( _( "%1 not available" ) ).arg( _fullName );
	    setTextColor( Qt::red );
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
		    break;

                case PMSelectable::S_AutoDel:
                case PMSelectable::S_Del:
		    // Package %1 is marked for deletion, yet other packages require it
		    text = ( _( "Deleting %1 breaks other packages" ) ).arg( _shortName );
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
}


void
YQPkgConflict::dumpLists()
{
    if ( _conflict.state_change_not_possible )
    {
	dumpList( this, _conflict.referers, LIST_SPLIT_THRESHOLD, "",
		  _( "Required by:" ) );
    }

    dumpList( this, _conflict.unresolvable, LIST_SPLIT_THRESHOLD, "", 
	      _( "Unresolved Reqirements:" ) );

    dumpList( this, _conflict.conflicts_with, LIST_SPLIT_THRESHOLD, "",
	      _( "Conflicts with:" ) );
}


void
YQPkgConflict::dumpList( QListViewItem * 	parent,
			 PkgDep::RelInfoList &	list,
			 int			splitThreshold,
			 const QString &	itemPrefix,
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
    int count		= 0;
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
	    
	QString contents = itemPrefix;
	std::string rel = (*it).name;
	rel += " " + _( "requires" ) + " ";
	rel += PkgRelation::toString( (*it).rel );
	
	contents += rel.c_str();
	new QY2ListViewItem( parent, contents, true );

	++it;
    }
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



#include "YQPkgConflictList.moc.cc"
