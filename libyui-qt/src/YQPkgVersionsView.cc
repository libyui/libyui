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

#include <YQZypp.h>
#include <zypp/Repository.h>
#include <ycp/y2log.h>
#include <qtabwidget.h>
#include <qregexp.h>
#include <qheader.h>

#include "YQPkgVersionsView.h"
#include "YQPkgRepoList.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QY2ListView( parent )
{
    _selectable		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;

    _versionCol	= -42;
    _archCol	= -42;
    _productCol	= -42;
    _urlCol	= -42;
    _instSrcCol	= -42;
    _statusCol	= -42;
    _nameCol	= -42;
    _summaryCol = -42;

    int numCol = 0;
    addColumn( _( "Version" 		) );	_versionCol	= numCol++;
    addColumn( _( "Arch." 		) );	_archCol	= numCol++;
    addColumn( _( "Product"		) );	_productCol	= numCol++;
    addColumn( _( "Installation Source"	) );	_instSrcCol	= numCol++;
    addColumn( _( "URL"			) );	_urlCol		= numCol++;
    _statusCol	= _productCol;

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
	showDetailsIfVisible( _selectable );
}


void
YQPkgVersionsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	    showDetails( selectable );
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

    bool installedIsAvailable = false;

    zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

    while ( it != selectable->availableEnd() )
    {
	new YQPkgVersion( this, root, selectable, *it, _userCanSwitch );

	if ( selectable->installedObj() &&
	     selectable->installedObj()->edition() == (*it)->edition() &&
	     selectable->installedObj()->arch()    == (*it)->arch()      )
	    // FIXME: In future releases, also the vendor will make a difference
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

		// Change status of selectable

		ZyppStatus status = _selectable->status();

		if ( _selectable->installedObj() &&
		     _selectable->installedObj()->edition() == newCandidate->edition() )
		{
		    // Switch back to the original instance -
		    // the version that was previously installed
		    status = S_KeepInstalled;
		}
		else
		{
		    switch ( status )
		    {
			case S_KeepInstalled:
			case S_Protected:
			case S_AutoDel:
			case S_AutoUpdate:
			case S_Del:
			case S_Update:

			    status = S_Update;
			    break;

			case S_NoInst:
			case S_Taboo:
			case S_Install:
			case S_AutoInstall:
			    status = S_Install;
			    break;
		    }
		}

		_selectable->setStatus( status );


		// Set candidate

		_selectable->setCandidate( newCandidate );
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






YQPkgVersion::YQPkgVersion( YQPkgVersionsView *	pkgVersionList,
			    QY2CheckListItem * 	parent,
			    ZyppSel		selectable,
			    ZyppObj 		zyppObj,
			    bool		enabled )
    : QY2CheckListItem( parent, "",
			enabled ?
			QCheckListItem::RadioButton :
			QCheckListItem::Controller )	// cheap way to make it read-only
    , _pkgVersionList( pkgVersionList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    setOn( _zyppObj == _selectable->candidateObj() );

    // Translators: %1 is a package version, %2 the package architecture,
    // %3 describes the repository where it comes from,
    // %4 is the repository's priority
    // %5 is the vendor of the package
    // Examples:
    //     2.5.23-i568 from Packman with priority 100 and vendor openSUSE
    //     3.17.4-i386 from openSUSE-11.1 update repository with priority 20 and vendor openSUSE
    //     ^^^^^^ ^^^^      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^               ^^            ^^^^^^^^
    //        %1   %2                %3                                   %4                %5
    setText( 0,_( "%1-%2 from %3 with priority %4 and vendor %5" )
               .arg( fromUTF8( zyppObj->edition().asString().c_str() ) )
               .arg( fromUTF8( zyppObj->arch().asString().c_str() ) )
               .arg( fromUTF8( zyppObj->repository().info().name().c_str() ) )
               .arg( zyppObj->repository().info().priority() )
               .arg( fromUTF8( zyppObj->vendor().c_str() ) ) );

    if ( _selectable->hasInstalledObj() )
    {
	if ( _zyppObj->edition() == _selectable->installedObj()->edition() &&
	     _zyppObj->arch()    == _selectable->installedObj()->arch()      )
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

    if ( zyppObj() == selectable()->installedObj() )
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
	if ( this->zyppObj()->edition() > other->zyppObj()->edition() ) return  1;
	return 0;
    }

    // Fallback: Use parent class method
    return QY2CheckListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgVersionsView.moc"
