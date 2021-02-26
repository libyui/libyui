/**************************************************************************
Copyright (C) 2000 - 2011 Novell, Inc.
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

  File:	      YQPkgHistoryDialog.cc

  Author:     Stanislav Visnovsky <visnov@suse.com>

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QList>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QMessageBox>

#include <zypp/parser/HistoryLogReader.h>
#include <zypp/Date.h>
#include <zypp/Edition.h>

#include <boost/ref.hpp>

#include "YQPkgHistoryDialog.h"
#include "YQPkgList.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "YQIconPool.h"
#include "utf8.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget

#define FILENAME	"/var/log/zypp/history"

using std::endl;

YQPkgHistoryDialog::YQPkgHistoryDialog( QWidget * parent )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Package History" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout(layout);
    layout->setMargin(MARGIN);
    layout->setSpacing(SPACING);

    setMinimumSize (300,400);

    QLabel * label = new QLabel ( _("Show History (/var/log/zypp/history)" ), this );
    label->setFixedHeight (label->sizeHint ().height ());
    layout->addWidget (label);

    // VBox for splitter
    QSplitter * splitter = new QSplitter( Qt::Horizontal, this );
    Q_CHECK_PTR( splitter );
    layout->addWidget( splitter );

    // History view
    _dates = new QTreeWidget (splitter);
    _dates->setColumnCount (1);
    _dates->setHeaderLabels ( QStringList( _("Date") ) );

    _actions = new QTreeWidget (splitter);
    _actions->setColumnCount (2);
    _actions->setHeaderLabels ( QStringList( _("Action") ) << _("Version/URL") );
    _actions->setColumnWidth (0, 200);

    splitter->setStretchFactor (0, 1);
    splitter->setStretchFactor (1, 2);

    // Button box (to center the single button)

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );
    hbox->addStretch();

    // "OK" button

    QPushButton * button = new QPushButton( _( "&Close" ), this );
    Q_CHECK_PTR( button );
    hbox->addWidget(button);
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    connect( _dates,	SIGNAL( itemSelectionChanged () ),
	    this, 	SLOT ( moveToDate () ) );

    connect( _actions,	SIGNAL( itemSelectionChanged() ),
	    this, 	SLOT ( moveToAction () ) );

    initialize ();
}

QSize
YQPkgHistoryDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


void
YQPkgHistoryDialog::showHistoryDialog( QWidget* parent)
{
    YQPkgHistoryDialog dialog( parent );
    dialog.exec();
}

QPixmap actionIcon (zypp::HistoryActionID id)
{
    switch (id.toEnum ()) {
	case zypp::HistoryActionID::INSTALL_e : return YQIconPool::pkgInstall ();
	case zypp::HistoryActionID::REMOVE_e : return YQIconPool::pkgDel ();
	case zypp::HistoryActionID::REPO_REMOVE_e : return YQIconPool::treeMinus ();
	case zypp::HistoryActionID::REPO_ADD_e : return YQIconPool::treePlus ();
	default: return QPixmap ();
    }

    return QPixmap ();
}

struct HistoryItemCollector
{
    QTreeWidget* actions, * dates;
    QString _last;
    QTreeWidgetItem* date_start;

    bool operator()( const zypp::HistoryLogData::Ptr & item_ptr )
    {
	QString d = fromUTF8( item_ptr->date().form("%e %B %Y"));
	if (d != _last)
	{
	    _last = d;
	    date_start = new QTreeWidgetItem (actions, QStringList(d));
	    date_start->setExpanded (true);
	    actions-> insertTopLevelItem ( 0, date_start );
	    dates-> insertTopLevelItem ( 0, new QTreeWidgetItem (dates, QStringList(d)));
	}

	QStringList columns;
	if ( item_ptr->action() == zypp::HistoryActionID::INSTALL_e )
	{
	    zypp::HistoryLogDataInstall* item = static_cast <zypp::HistoryLogDataInstall *> (item_ptr.get());

	    columns << fromUTF8(item->name());
	    columns << fromUTF8(item->edition().version());
	} else
	if ( item_ptr->action() == zypp::HistoryActionID::REMOVE_e )
	{
	    zypp::HistoryLogDataRemove* item = static_cast <zypp::HistoryLogDataRemove *> (item_ptr.get());

	    columns << fromUTF8(item->name());
	    columns << fromUTF8(item->edition().version());
	} else
	if ( item_ptr->action() == zypp::HistoryActionID::REPO_ADD_e )
	{
	    zypp::HistoryLogDataRepoAdd* item = static_cast <zypp::HistoryLogDataRepoAdd *> (item_ptr.get());

	    columns << fromUTF8(item->alias());
	    columns << fromUTF8(item->url().asString());
	} else
	if ( item_ptr->action() == zypp::HistoryActionID::REPO_REMOVE_e )
	{
	    zypp::HistoryLogDataRepoRemove* item = static_cast <zypp::HistoryLogDataRepoRemove *> (item_ptr.get());

	    columns << fromUTF8(item->alias());
	} else
	if ( item_ptr->action() == zypp::HistoryActionID::REPO_CHANGE_ALIAS_e )
	{
	    zypp::HistoryLogDataRepoAliasChange* item = static_cast <zypp::HistoryLogDataRepoAliasChange *> (item_ptr.get());

	    columns << fromUTF8(item->oldAlias()) + " -> " + fromUTF8(item->newAlias());
	} else
	if ( item_ptr->action() == zypp::HistoryActionID::REPO_CHANGE_URL_e )
	{
	    zypp::HistoryLogDataRepoUrlChange* item = static_cast <zypp::HistoryLogDataRepoUrlChange *> (item_ptr.get());

	    columns << fromUTF8(item->alias());
	    columns << fromUTF8(item->newUrl().asString());
	}

	QTreeWidgetItem *action = new QTreeWidgetItem (date_start, columns);
	action->setIcon (0,actionIcon (item_ptr->action()) );
	return true;
    }
};


void
YQPkgHistoryDialog::initialize()
{
    HistoryItemCollector ic;

    ic.actions = _actions;
    ic.dates = _dates;
    zypp::parser::HistoryLogReader reader(FILENAME, zypp::parser::HistoryLogReader::Options(), boost::ref(ic));

    try
    {
	reader.readAll();
    }
    catch (const zypp::Exception & exception)
    {
        yuiWarning() << "CAUGHT zypp exception: " << exception.asUserHistory() << endl;

        QMessageBox msgBox;

        // Translators: This is a (short) text indicating that something went
        // wrong while trying to read history file.

        QString heading = _( "Unable to read history" );

        if ( heading.length() < 25 )    // Avoid very narrow message boxes
        {
            QString blanks;
            blanks.fill( ' ', 50 - heading.length() );
            heading += blanks;
        }

        msgBox.setText( heading );
        msgBox.setIcon( QMessageBox::Warning );
        msgBox.setInformativeText( fromUTF8( exception.asUserHistory() ) );
        msgBox.exec();
    }
}


void
YQPkgHistoryDialog::moveToDate ()
{
    if (signalsBlocked() ) return;
    QString item = _dates->selectedItems().first()->text(0);
    QList<QTreeWidgetItem *> items = _actions->findItems (item, Qt::MatchExactly, 0);

    if( items.size () > 0 )
    {
	blockSignals(true);
	_actions->expandItem (items.first());
	_actions->setCurrentItem (items.first());
	_actions->scrollToItem( items.first(), QAbstractItemView::PositionAtTop );
	blockSignals(false);
    }
}

void
YQPkgHistoryDialog::moveToAction ()
{
    if (signalsBlocked() ) return;
    QTreeWidgetItem* item = _actions->selectedItems().first();

    // if this is not top-level item, better pick top-level one
    if (item->parent ())
	item = item->parent ();

    QList<QTreeWidgetItem *> items = _dates->findItems (item->text(0), Qt::MatchExactly | Qt::MatchRecursive, 0);

    if( items.size () > 0 )
    {
	blockSignals(true);
	_dates->setCurrentItem (items.first());
	blockSignals(false);
    }
}

