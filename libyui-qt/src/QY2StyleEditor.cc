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
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		QY2StyleEditor.cc

  Author:	Thomas Goettlicher <tgoettlicher@suse.de>

  Textdomain	"qt"

/-*/

#include "QY2StyleEditor.h"
#include "QY2Styler.h"
#include "ui_QStyleEditor.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextObject>
#include "YQi18n.h"


QY2StyleEditor::QY2StyleEditor( QWidget *parent )
    : QDialog( parent )
{
    ui.setupUi(this);

    ui.textEdit->setPlainText( "/* enter style sheet here  */" );
    ui.closeButton->setAutoDefault(false);
    setWindowTitle( _( "Stylesheet Editor" ) );

    connect( ui.applyButton, SIGNAL( clicked() ),
             this, SLOT( slotApplyStyle() ));

    connect( ui.closeButton, SIGNAL( clicked() ),
             this, SLOT( close() ));

    connect( ui.loadButton, SIGNAL( clicked() ),
             this, SLOT( slotLoadFile() ));

    connect( ui.textEdit, SIGNAL( textChanged() ),
             this, SLOT( slotTextChanged() ));

    connect( ui.autoApply, SIGNAL( stateChanged(int) ),
             this, SLOT( slotTextChanged() ));
}


QY2StyleEditor::~QY2StyleEditor()
{
}

void QY2StyleEditor::slotTextChanged()
{
    if ( ui.autoApply->isChecked() )
        slotApplyStyle();
}

void QY2StyleEditor::slotApplyStyle()
{
    QY2Styler::styler()->setStyleSheet( ui.textEdit->toPlainText() );
}


void QY2StyleEditor::slotLoadFile()
{

    QString fileName = QFileDialog::getOpenFileName( this,      // parent
                                                    QString( "Load stylesheet ..." ), // caption
                                                    QY2Styler::styler()->themeDir(), // dir 
                                                    QString( "*.qss") );              // filter

    if ( fileName.isEmpty() )  
        return;         // user clicked cancel


    QFile file( fileName);

    if ( file.open( QFile::ReadOnly ) )
        ui.textEdit->setPlainText( file.readAll() );
    else
    {
        QMessageBox::warning( this,                     // parent
            QString("Error") ,                          // caption
            QString( "Couldn't load file\n%1" ).arg( fileName ),
            QMessageBox::Ok | QMessageBox::Default,     // button0
            Qt::NoButton,                               // button1
            Qt::NoButton );                             // button2
    }

}



#include "QY2StyleEditor.moc"
