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

#include "YQPkgVersionsView.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QY2ListView( parent )
{
    _selectable		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;


    int numCol = 0;
    addColumn( _( "Version" 		) );	_versionCol	= numCol++;
    addColumn( _( "Arch." 		) );	_archCol	= numCol++;
    addColumn( _( "Installation Source"	) );	_instSrcCol	= numCol++;
    _statusCol	= _instSrcCol;

    _nameCol	= _versionCol;
    _summaryCol = _instSrcCol;

    // saveColumnWidths();	// Minimize column widths
    // header()->hide();


    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged(QWidget *) ),
		 this,   SLOT  ( reload        (QWidget *) ) );
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
	showDetailsIfVisible( _selectable );
    }
}


void
YQPkgVersionsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showDetails( selectable );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( selectable );
    }
}


void
YQPkgVersionsView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;
    clear();

    if ( ! selectable )
	return;

    QY2CheckListItem * root = new QY2CheckListItem( this, selectable->theObj()->name().c_str(),
						    QCheckListItem::Controller, true );
    CHECK_PTR( root );
    root->setOpen( true );

#if 0
    root->setText( _summaryCol, fromUTF8( zyppObj->summary() ) );
#endif

    bool installedIsAvailable = false;
#ifdef FIXME
    zypp::ui::Selectable::ResObjectList::const_iterator it = selectable->av_begin();

    while ( it != selectable->av_end() )
    {
	new YQPkgVersion( this, root, selectable, *it, _userCanSwitch );

	if ( selectable->installedObj() &&
	     selectable->installedObj()->edition() == ( *it)->edition() )
	    installedIsAvailable = true;

#if 0
	// DEBUG
	new YQPkgVersion( this, root, selectable, *it, _userCanSwitch );
	new YQPkgVersion( this, root, selectable, *it, _userCanSwitch );
	new YQPkgVersion( this, root, selectable, *it, _userCanSwitch );
	// DEBUG
#endif
	++it;
    }
#endif

    if ( selectable->hasInstalledObj() && ! installedIsAvailable )
	new YQPkgVersion( this, root, selectable, selectable->installedObj(), false );
}


void
YQPkgVersionsView::checkForChangedCandidate()
{
    if ( ! firstChild() || ! _selectable )
	return;

    QListViewItem * item = firstChild()->firstChild();

    while ( item )
    {
	YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (item);

	if ( versionItem && versionItem->isOn() )
	{
	    ZyppObj newCandidate = versionItem->zyppObj();

	    if ( newCandidate != _selectable->candidateObj() )
	    {
		y2milestone( "Candidate changed" );
#ifdef FIXME
		selectable->setUserCandidate( newCandidate );
#endif
		emit candidateChanged( newCandidate );
		return;
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






YQPkgVersion::YQPkgVersion( YQPkgVersionsView *		pkgVersionList,
			    QY2CheckListItem * 		parent,
			    ZyppSel	selectable,
			    ZyppObj 	zyppObj,
			    bool			enabled )
    : QY2CheckListItem( parent, "",
			enabled ?
			QCheckListItem::RadioButton :
			QCheckListItem::Controller )	// cheap way to make it read-only
    , _pkgVersionList( pkgVersionList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    setText( versionCol(), zyppObj->edition().asString().c_str() );
    setText( archCol(),    zyppObj->arch().asString().c_str() );
#ifdef FIXME
    setText( instSrcCol(), zyppObj->instSrcLabel().c_str() );
#endif
    setOn( _zyppObj == _selectable->installedObj() );

    if ( _selectable->hasInstalledObj() )
    {
	if ( _zyppObj->edition() == _selectable->installedObj()->edition() )
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
YQPkgVersion::toolTip(int)
{
    QString tip;

    if ( _zyppObj == _selectable->installedObj() )
	tip = _( "This version is installed in your system." );

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
	if ( this->zyppObj()->edition() < other->zyppObj()->edition() ) return -1;
	if ( this->zyppObj()->edition() > other->zyppObj()->edition() ) return 1;
	return 0;
    }

    // Fallback: Use parent class method
    return QY2CheckListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgVersionsView.moc"
