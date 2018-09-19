/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		QY2RelNotesDialog.cc

  Author:	Jiri Srain <jsrain@suse.de>

  Textdomain	"qt"

/-*/

#include "QY2RelNotesDialog.h"
#include <QDebug>
#include <QTextObject>
#include "YQi18n.h"
#include "YQUI.h"
#include "QY2Styler.h"
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>


#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "qt"


QY2RelNotesDialog::QY2RelNotesDialog( QWidget *parent )
    : QDialog( parent )
{
    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("QRelNotesDialog"));
    this->resize(581, 388); // same size as help pop-up, proven over time
    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

    tabBar = new QTabBar( this );
    Q_CHECK_PTR( tabBar );

    tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    setFocusProxy( tabBar );
    setFocusPolicy( Qt::TabFocus );

    QObject::connect( tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged( int )));

    vboxLayout->addWidget(tabBar);

    textBrowser = new QTextBrowser(this);
    textBrowser->setObjectName(QStringLiteral("textBrowser"));

    vboxLayout->addWidget(textBrowser);

    hboxLayout = new QHBoxLayout();
    closeButton = new QPushButton(this);
    closeButton->setObjectName(QStringLiteral("closeButton"));
    hboxLayout->addStretch();
    hboxLayout->addWidget(closeButton);
    hboxLayout->addStretch();

    vboxLayout->addLayout(hboxLayout);

    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    YQUI::setTextdomain( TEXTDOMAIN );

    // Window title for help wizard window
    setWindowTitle( _( "Release Notes" ) );

    // Close button for wizard help window
    closeButton->setText( _( "&Close" ) );

    QY2Styler::styler()->registerWidget( this );
    // the stylesheet for the displayed text is set separately
    textBrowser->document()->setDefaultStyleSheet( QY2Styler::styler()->textStyle() );
}

void QY2RelNotesDialog::setRelNotes( const std::map<std::string,std::string>& relnotes )
{
    while (tabBar->count() > 0)
    {
        tabBar->removeTab( 0 );
    }

    _relnotes = relnotes;
    _tab_keys = std::vector<std::string>();
    for(std::map<std::string,std::string>::const_iterator it = relnotes.begin(); it != relnotes.end(); ++it) {
        _tab_keys.push_back(it->first);
        tabBar->addTab( it->first.c_str() );
    }
    if (_relnotes.size() > 1)
    {
        tabBar->show();
    }
    else
    {
	tabBar->hide();
    }
    textBrowser->setText( relnotes.begin()->second.c_str() );
}

QY2RelNotesDialog::~QY2RelNotesDialog()
{
    QY2Styler::styler()->unregisterWidget( this );
}

void QY2RelNotesDialog::tabChanged( int index )
{
    if (index < 0 || _tab_keys.empty() || _relnotes.empty())
    {
        return;
    }
    textBrowser->setText( _relnotes[_tab_keys[index]].c_str() );
}

void QY2RelNotesDialog::retranslate()
{
    setWindowTitle( _( "Release Notes" ) );
    closeButton->setText( _( "&Close" ) );
}




