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
#include <QTabWidget>
#include <QRegExp>
#include <QHeaderView>

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
    _repoCol	= -42;
    _statusCol	= -42;
    _nameCol	= -42;
    _summaryCol = -42;

    QStringList headers;
    int numCol = 0;
    headers << _( "Version" 	);	_versionCol	= numCol++;
    headers << _( "Arch." 	);	_archCol	= numCol++;
    headers << _( "Product"	);	_productCol	= numCol++;
    headers << _( "Repository"	);	_repoCol	= numCol++;
    headers << _( "URL"		);	_urlCol		= numCol++;

    setHeaderLabels(headers);
    _statusCol	= _productCol;

    _nameCol	= _versionCol;
    _summaryCol = _repoCol;

    setSortByInsertionSequence( true );

    // saveColumnWidths();	// Minimize column widths
    // header()->hide();


    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged(QWidget *) ),
		 this,   SLOT  ( reload        (QWidget *) ) );
    }

    connect( this,	SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ),
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
	if ( _parentTab->currentWidget() == this )  // Is this page the topmost?
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

#if FIXME
    QY2CheckListItem * root = new QY2CheckListItem( this, selectable->theObj()->name().c_str() );
    //FIXME add element
    Q_CHECK_PTR( root );
    root->setExpanded( true );

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

#endif
}


void
YQPkgVersionsView::checkForChangedCandidate()
{
    QTreeWidgetItemIterator iter(this);
    QTreeWidgetItem *first = *iter;

    if ( ! first || ! _selectable )
	return;

    
    QTreeWidgetItemIterator iter_c(first);
    
    while ( *iter_c )
    {
	YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (*iter_c);

	if ( versionItem && (versionItem->checkState(0) == Qt::Checked) )
	{
	    ZyppObj newCandidate = versionItem->zyppObj();

	    if ( newCandidate != _selectable->candidateObj() )
	    {
		y2milestone( "Candidate changed" );

		// Change status of selectable

		ZyppStatus status = _selectable->status();

		if ( _selectable->installedObj() &&
		     _selectable->installedObj()->arch()    == newCandidate->arch() &&
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

		_selectable->set_status( status );


		// Set candidate

		_selectable->setCandidate( newCandidate );
		emit candidateChanged( newCandidate );
		return;
	    }
	}

	++iter_c;
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
    : QY2CheckListItem( parent, "" /*
			enabled ?
			Q3CheckListItem::RadioButton :
			Q3CheckListItem::Controller */ )	// cheap way to make it read-only
    , _pkgVersionList( pkgVersionList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    // FIXME setOn( _zyppObj == _selectable->candidateObj() );

    if ( versionCol() >= 0 )	setText( versionCol(), zyppObj->edition().asString().c_str() );
    if ( archCol()    >= 0 )	setText( archCol(),    zyppObj->arch().asString().c_str() );
    if ( repoCol() >= 0 )	setText( repoCol(), fromUTF8(zyppObj->repository().info().alias()) );
    if ( productCol() >= 0 )
    {
	ZyppProduct product = YQPkgRepoListItem::singleProduct( zyppObj->repository() );

	if ( product )
	    setText( productCol(), QString::fromStdString( product->summary() ) );
    }
    if ( urlCol() >= 0 )
    {
        zypp::Url repoUrl;

	if ( ! zyppObj->repository().info().baseUrlsEmpty() )
	    repoUrl = *zyppObj->repository().info().baseUrlsBegin();

	setText( urlCol(), repoUrl.asString().c_str() );
    }

    if ( _selectable->hasInstalledObj() )
    {
	if ( _zyppObj->edition() == _selectable->installedObj()->edition() &&
	     _zyppObj->arch()    == _selectable->installedObj()->arch()      )
	{
	    // FIXME setPixmap( statusCol(), YQIconPool::pkgKeepInstalled() );
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


bool YQPkgVersion::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgVersion * other = dynamic_cast<const YQPkgVersion *> (&otherListViewItem);

    if ( other )
    {
	return ( this->zyppObj()->edition() < other->zyppObj()->edition() );
    }

    // Fallback: Use parent class method
    return QY2CheckListItem::operator<( otherListViewItem );
}



#include "YQPkgVersionsView.moc"
