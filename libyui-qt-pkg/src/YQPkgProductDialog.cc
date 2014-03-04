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

  File:	      YQPkgProductDialog.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>
#include <qdesktopwidget.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QTabWidget>
#include <QList>
#include <QBoxLayout>

#include "YQPkgProductDialog.h"
#include "YQPkgProductList.h"
#include "YQPkgDependenciesView.h"
#include "QY2LayoutUtils.h"
#include "YQi18n.h"
#include "YQUI.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgProductDialog::YQPkgProductDialog( QWidget * parent )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Products" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout(layout);
    layout->setSpacing( SPACING );
    layout->setMargin ( MARGIN  );

    // VBox for splitter

    QSplitter * splitter = new QSplitter( Qt::Vertical, this );
    Q_CHECK_PTR( splitter );
    layout->addWidget( splitter );
    layout->setMargin( MARGIN );


    // Product list

    _productList = new YQPkgProductList( splitter );
    Q_CHECK_PTR( _productList );
    _productList->resize( _productList->width(), 80 );

    // Prevent status changes for now - this would require solver runs etc.
    _productList->setEditable( false );


    // Tab widget for details views (looks better even just with one)

    _detailsViews = new QTabWidget( splitter );
    Q_CHECK_PTR( _detailsViews );
    //_detailsViews->setMargin( MARGIN );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    // Dependencies view

    _dependenciesView = new YQPkgDependenciesView( _detailsViews );
    Q_CHECK_PTR( _dependenciesView );
    _detailsViews->addTab( _dependenciesView, _( "Dependencies" ) );
    _dependenciesView->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _productList,	SIGNAL( currentItemChanged    ( ZyppSel ) ),
	     _dependenciesView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    // Button box (to center the single button)

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );

    //addHStretch( hbox );


    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK" ), this );
    hbox->addWidget(button);
    Q_CHECK_PTR( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    //addHStretch( hbox );
}


void
YQPkgProductDialog::polish()
{
    // Delayed initialization after widget is fully created etc.

    // Only now send currentItemChanged() signal so the details views display something
    // (showDetailsIfVisible() shows only something if the widget is visible,
    // as the method name implies)
    _productList->selectSomething();
}


QSize
YQPkgProductDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


void
YQPkgProductDialog::showProductDialog( QWidget* parent)
{
    YQPkgProductDialog dialog( parent );
    dialog.exec();
}




#include "YQPkgProductDialog.moc"
