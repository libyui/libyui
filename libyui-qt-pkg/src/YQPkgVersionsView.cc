/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"

#include <YQZypp.h>
#include <zypp/Repository.h>
#include "YUILog.h"
#include <QTabWidget>
#include <QRegExp>
#include <QHeaderView>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>


#include "YQPkgVersionsView.h"
#include "YQPkgRepoList.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"

#define ICONOFFSET 3 // the status icons have an asymmetrical transparent border

using std::endl;

YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QScrollArea( parent )
    , _content(0)
    , _layout(0)
{
    _selectable		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;

    _buttons = new QButtonGroup(this);

    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged(QWidget *) ),
		 this,   SLOT  ( reload        (QWidget *) ) );
    }
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
YQPkgVersionsView::slotRefreshDetails( )
{
    emit multiversionSelectionChanged( );
}


void
YQPkgVersionsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    yuiMilestone() << "showDetailsIfVis" << endl;

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
    yuiMilestone() << "showDetails" << endl;

    _selectable = selectable;

    if ( ! selectable )
    {
	// Delete all installed items
	qDeleteAll( _installed );
	_installed.clear();

	_content = new QWidget( this );
	setWidget( _content );
        _content->show();
        return;
    }

    // old widget is autodestroyed by setWidget later
    _content = new QWidget( this );
    _layout = new QVBoxLayout( _content );
    _content->setLayout( _layout );

    // also paint the scrollarea background
    setPalette( QApplication::palette().color( QPalette::Active, QPalette::Base ) );
    setAutoFillBackground( true );

    _content->setPalette( QApplication::palette().color( QPalette::Active, QPalette::Base ) );
    _content->setAutoFillBackground( true );

    QLabel * pkgNameLabel = new QLabel( this );

    if ( ! selectable->theObj() )
        return;

    _layout->addWidget( pkgNameLabel );

    QFont font = pkgNameLabel->font();
    font.setBold( true );

    QFontMetrics fm( font) ;
    font.setPixelSize( (int) ( fm.height() * 1.1 ) );

    pkgNameLabel->setFont( font );
    pkgNameLabel->setText( fromUTF8(selectable->theObj()->name().c_str()) );

    // New scope
    {
        QListIterator<QAbstractButton*> it( _buttons->buttons() );

        while ( it.hasNext() )
        {
            delete it.next();
        }
    }

    // Delete all installed items
    qDeleteAll( _installed );
    _installed.clear();

    if ( selectable->multiversionInstall() )
    {
        //
        // Find installed and available objects ( for multiversion view )
        //
        {
            zypp::ui::Selectable::picklist_iterator it = selectable->picklistBegin();


            while ( it != selectable->picklistEnd() )
            {
                YQPkgMultiVersion * version = new YQPkgMultiVersion( this, selectable, *it, _userCanSwitch );

                _installed.push_back( version );
                _layout->addWidget( version );

		connect (version, SIGNAL(statusChanged()), this, SLOT(slotRefreshDetails()));
		connect (this, SIGNAL(multiversionSelectionChanged()), version, SLOT(update()));

                ++it;
            }
        }
    }
    else
    {
        //
        // Fill installed objects
        //
        {
            zypp::ui::Selectable::installed_iterator it = selectable->installedBegin();

            while ( it != selectable->installedEnd() )
            {
                QString text = _( "%1-%2 from vendor %3 (installed)" )
		    .arg( fromUTF8( (*it)->edition().asString().c_str() ) )
		    .arg( fromUTF8( (*it)->arch().asString().c_str() ) )
		    .arg( fromUTF8( (*it)->vendor().c_str() ) ) ;

                QWidget * installedVersion = new QWidget( this );
	        QHBoxLayout * instLayout = new QHBoxLayout( installedVersion );
	        instLayout->setContentsMargins( 0, 0, 0, 0 );

	        QLabel * icon = new QLabel( installedVersion );
	        icon->setPixmap( YQIconPool::pkgSatisfied() );
	        instLayout->addWidget( icon );

	        QLabel * textLabel = new QLabel( text, installedVersion );
	        instLayout->addWidget( textLabel );
	        instLayout->addStretch();

                _installed.push_back( installedVersion );
                _layout->addWidget( installedVersion );

                ++it;
            }
        }


        //
        // Fill available objects
        //

        {
            zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

            while ( it != selectable->availableEnd() )
            {
                QRadioButton *radioButton = new YQPkgVersion( this, selectable, *it, _userCanSwitch );
                connect( radioButton, SIGNAL( clicked( bool ) ), SLOT( checkForChangedCandidate() ) );

                _buttons->addButton( radioButton );
                _layout->addWidget( radioButton );


                if ( selectable->hasCandidateObj() &&
                     selectable->candidateObj()->edition() == (*it)->edition() &&
                     selectable->candidateObj()->arch()    == (*it)->arch() )
                {
                    radioButton->setChecked(true);
                }

                ++it;
            }
        }
    }

    _layout->addStretch();
    setWidget( _content );
    _content->show();
}


void
YQPkgVersionsView::checkForChangedCandidate()
{
    QListIterator<QAbstractButton*> it( _buttons->buttons() );

    while ( it.hasNext() )
    {
        YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (it.next());

        if ( versionItem && versionItem->isChecked() )
        {
            ZyppObj newCandidate = versionItem->zyppObj();

            if ( _selectable && newCandidate != _selectable->candidateObj() )
            {
                yuiMilestone() << "Candidate changed" << endl;

                // Change status of selectable

                ZyppStatus status = _selectable->status();

                if ( !_selectable->installedEmpty() &&
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

                _selectable->setStatus( status );


                // Set candidate

                _selectable->setCandidate( newCandidate );
                emit candidateChanged( newCandidate );
                return;
            }
        }

    }
}


QSize
YQPkgVersionsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}






YQPkgVersion::YQPkgVersion( QWidget *	parent,
			    ZyppSel	selectable,
			    ZyppObj 	zyppObj,
			    bool	enabled )
    : QRadioButton( parent )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    // Translators: %1 is a package version, %2 the package architecture,
    // %3 describes the repository where it comes from,
    // %4 is the repository's priority
    // %5 is the vendor of the package
    // Examples:
    //     2.5.23-i568 from Packman with priority 100 and vendor openSUSE
    //     3.17.4-i386 from openSUSE-11.1 update repository with priority 20 and vendor openSUSE
    //     ^^^^^^ ^^^^      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^               ^^            ^^^^^^^^
    //        %1   %2                %3                                   %4                %5
    setText( _( "%1-%2 from %3 with priority %4 and vendor %5" )
	     .arg( fromUTF8( zyppObj->edition().asString().c_str() ) )
	     .arg( fromUTF8( zyppObj->arch().asString().c_str() ) )
	     .arg( fromUTF8( zyppObj->repository().info().name().c_str() ) )
	     .arg( zyppObj->repository().info().priority() )
	     .arg( fromUTF8( zyppObj->vendor().c_str() ) ) );
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




YQPkgMultiVersion::YQPkgMultiVersion( QWidget *	parent,
			    ZyppSel	selectable,
			    ZyppPoolItem zyppPoolItem,
			    bool	enabled )
    : QCheckBox( parent )
    , _selectable( selectable )
    , _zyppPoolItem( zyppPoolItem )
{
    setText (_( "%1-%2 from %3 with priority %4 and vendor %5" )
	     .arg( fromUTF8( zyppPoolItem->edition().asString().c_str() ) )
    	     .arg( fromUTF8( zyppPoolItem->arch().asString().c_str() ) )
	     .arg( fromUTF8( zyppPoolItem->repository().info().name().c_str() ) )
	     .arg( zyppPoolItem->repository().info().priority() )
	     .arg( fromUTF8( zyppPoolItem->vendor().c_str() ) ));


    connect( this, SIGNAL (toggled(bool)), this, SLOT( slotIconClicked()));
}


YQPkgMultiVersion::~YQPkgMultiVersion()
{
    // NOP
}


void YQPkgMultiVersion::slotIconClicked()
{
    // prevent checkmark, we draw the status icons ourselves
    bool oldBlock = blockSignals( true );
    setChecked( false );
    blockSignals( oldBlock );
    cycleStatus();
}


void YQPkgMultiVersion::cycleStatus()
{

    ZyppStatus oldStatus = _selectable->pickStatus( _zyppPoolItem );
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
        case S_Install:
            newStatus = S_NoInst;
            break;

        case S_Protected:
            newStatus = _selectable->identicalAvailable( _zyppPoolItem ) ?
                S_KeepInstalled: S_NoInst;
            break;

        case S_Taboo:
            newStatus = _selectable->identicalInstalled( _zyppPoolItem ) ?
                S_KeepInstalled : S_NoInst;
            break;

        case S_KeepInstalled:
            newStatus = _selectable->identicalAvailable( _zyppPoolItem ) ?
                S_Update : S_Del;
            break;

        case S_Update:
            newStatus = S_Del;
            break;

        case S_AutoUpdate:
            newStatus = S_KeepInstalled;
            break;

        case S_Del:
        case S_AutoDel:
            newStatus = S_KeepInstalled;
            break;

        case S_NoInst:
            if ( _selectable->identicalAvailable( _zyppPoolItem ) )
            {
                newStatus = S_Install;
            }
            else
            {
                yuiWarning() << "No candidate for " << _selectable->theObj()->name() << endl;
                newStatus = S_NoInst;
            }
            break;

        case S_AutoInstall:
            // this used to be taboo before, but now ZYpp supports
            // saving weak locks (unselected packages)
            newStatus =  S_NoInst;
            break;
    }


    setStatus( newStatus );
    yuiMilestone() << "oldStatus:" << oldStatus << endl;
    yuiMilestone() << "newStatus:" << newStatus << endl;

}


void YQPkgMultiVersion::setStatus( ZyppStatus newStatus )
{
    _selectable->setPickStatus( _zyppPoolItem, newStatus );

    emit statusChanged();
    update();
}


void YQPkgMultiVersion::paintEvent(QPaintEvent *)
{
    // draw the usual checkbox
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    p.drawControl(QStyle::CE_CheckBox, opt);


    // calculate position and draw the status icon
    QRect elementRect = style()->subElementRect ( QStyle::SE_CheckBoxIndicator, &opt);
    QPixmap icon = statusIcon( _selectable->pickStatus(_zyppPoolItem) );

    QPoint start = elementRect.center() - icon.rect().center();
    QRect rect = QRect(start.x() - ICONOFFSET, start.y(), icon.width(), icon.height());

    p.drawItemPixmap( rect, 0, icon );
}


QPixmap YQPkgMultiVersion::statusIcon( ZyppStatus status )
{
    QPixmap icon = YQIconPool::pkgNoInst();

    switch ( status )
    {
        case S_Del:                 icon = YQIconPool::pkgDel();            break;
        case S_Install:             icon = YQIconPool::pkgInstall();        break;
        case S_KeepInstalled:       icon = YQIconPool::pkgKeepInstalled();  break;
        case S_NoInst:              icon = QPixmap();                       break;
        case S_Protected:           icon = YQIconPool::pkgProtected();      break;
        case S_Taboo:               icon = YQIconPool::pkgTaboo();          break;
        case S_Update:              icon = YQIconPool::pkgUpdate();         break;

        case S_AutoDel:             icon = YQIconPool::pkgAutoDel();        break;
        case S_AutoInstall:         icon = YQIconPool::pkgAutoInstall();    break;
        case S_AutoUpdate:          icon = YQIconPool::pkgAutoUpdate();     break;

        // Intentionally omitting 'default' branch so the compiler can
        // catch unhandled enum states
    }
    return icon;
}



#include "YQPkgVersionsView.moc"
