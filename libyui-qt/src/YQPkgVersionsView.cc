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

  File:	      YQPkgVersionsView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qtabwidget.h>
#include <qregexp.h>
#include <qheader.h>

#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>

#include "YQPkgVersionsView.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QY2ListView( parent )
{
    _pmObj		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;


    int numCol = 0;
    addColumn( _( "Version" ) );		_versionCol	= numCol++;
    addColumn( _( "Arch."   ) );		_archCol	= numCol++;
    addColumn( _( "Installation Source" ) );	_instSrcCol	= numCol++;
    _statusCol	= _instSrcCol;

    _nameCol	= _versionCol;
    _summaryCol = _instSrcCol;

    // saveColumnWidths();	// Minimize column widths
    // header()->hide();


    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged( QWidget * ) ),
		 this,   SLOT  ( reload        ( QWidget * ) ) );
    }

    connect( this,	SIGNAL( selectionChanged	() ),
	     this,	SLOT  ( checkForChangedCandidate() ) );
}


YQPkgVersionsView::~YQPkgVersionsView()
{
    // NOP
}


void
YQPkgVersionsView::reload( QWidget * newCurrent )
{
    if ( newCurrent == this )
    {
	showDetailsIfVisible( _pmObj );
    }
}


void
YQPkgVersionsView::showDetailsIfVisible( PMObjectPtr pmObj )
{
    _pmObj = pmObj;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showDetails( pmObj );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( pmObj );
    }
}


void
YQPkgVersionsView::showDetails( PMObjectPtr pmObj )
{
    clear();

    if ( ! pmObj )
	return;

    QY2CheckListItem * root = new QY2CheckListItem( this, pmObj->name().asString().c_str(),
						    QCheckListItem::Controller, true );
    CHECK_PTR( root );
    root->setOpen( true );

#if 0
    root->setText( _summaryCol, fromUTF8( pmObj->summary() ) );
#endif

    if ( ! pmObj->getSelectable() )
    {
	y2error( "%s doesn't have a PMSelectable parent!", pmObj->name().asString().c_str() );
	return;
    }

    bool installedIsAvailable = false;
    PMSelectable::PMObjectList::const_iterator it = pmObj->getSelectable()->av_begin();

    while ( it != pmObj->getSelectable()->av_end() )
    {
	new YQPkgVersion( this, root, *it, _userCanSwitch );

	if ( pmObj->getInstalledObj() &&
	     pmObj->getInstalledObj()->edition() == (*it)->edition() )
	    installedIsAvailable = true;

#if 0
	// DEBUG
	new YQPkgVersion( this, root, *it, _userCanSwitch );
	new YQPkgVersion( this, root, *it, _userCanSwitch );
	new YQPkgVersion( this, root, *it, _userCanSwitch );
	// DEBUG
#endif
	++it;
    }


    if ( pmObj->hasInstalledObj() && ! installedIsAvailable )
	new YQPkgVersion( this, root, pmObj->getInstalledObj(), false );
}


void
YQPkgVersionsView::checkForChangedCandidate()
{
    if ( ! firstChild() || ! _pmObj )
	return;

    QListViewItem * item = firstChild()->firstChild();

    while ( item )
    {
	YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (item);

	if ( versionItem && versionItem->isOn() )
	{
	    PMObjectPtr newCandidate = versionItem->pmObj();

	    if ( newCandidate != _pmObj->getCandidateObj() )
	    {
		PMSelectablePtr sel = newCandidate->getSelectable();

		if ( sel )
		{
		    y2milestone( "Candidate changed" );
		    sel->setUserCandidate( newCandidate );
		    emit candidateChanged( newCandidate );
		    return;
		}
	    }
	}

	item = item->nextSibling();
    }
}


QSize
YQPkgVersionsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}






YQPkgVersion::YQPkgVersion( YQPkgVersionsView *	pkgVersionList,
			    QY2CheckListItem * 	parent,
			    PMObjectPtr 	pmObj,
			    bool		enabled )
    : QY2CheckListItem( parent, "",
			enabled ?
			QCheckListItem::RadioButton :
			QCheckListItem::Controller )	// cheap way to make it read-only
    , _pkgVersionList( pkgVersionList )
    , _pmObj( pmObj )
{
    setText( versionCol(), pmObj->edition().asString().c_str() );
    setText( archCol(),    pmObj->arch().asString().c_str() );
    setText( instSrcCol(), pmObj->instSrcLabel().c_str() );
    setOn( pmObj->isCandidateObj() );

    if ( pmObj->hasInstalledObj() )
    {
	if ( pmObj->edition() == pmObj->getInstalledObj()->edition() )
	{
	    setPixmap( statusCol(), YQIconPool::pkgKeepInstalled() );
	    setBackgroundColor( QColor( 0xF0, 0xF0, 0xF0 ) ); 	// light grey
	    setTextColor( QColor( 0, 0x90, 0 ) );		// green
	}
    }
}


YQPkgVersion::~YQPkgVersion()
{
    // NOP
}


QString
YQPkgVersion::toolTip( int )
{
    QString tip;

    if ( _pmObj->hasInstalledObj() )
    {
	tip = _( "This version is installed in your system." );
    }

    return tip;
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgVersion::compare( QListViewItem *	otherListViewItem,
		       int		col,
		       bool		ascending ) const
{
    YQPkgVersion * other = dynamic_cast<YQPkgVersion *> (otherListViewItem);

    if ( other )
    {
	if ( this->constPMObj()->edition() < other->constPMObj()->edition() ) return -1;
	if ( this->constPMObj()->edition() > other->constPMObj()->edition() ) return 1;
	return 0;
    }

    // Fallback: Use parent class method
    return QY2CheckListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgVersionsView.moc.cc"
