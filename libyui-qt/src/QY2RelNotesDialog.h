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

  File:       QY2RelNotesDialog.h

  Author:     Jiri Srain <jsrain@suse.de>

  Textdomain    "qt"

/-*/

#ifndef _QY2RelNotesDialog_h
#define _QY2RelNotesDialog_h

#include <QDialog>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTabBar>

class QTextCursor;

class QY2RelNotesDialog : public QDialog
{
    Q_OBJECT

public:
    QY2RelNotesDialog( QWidget *parent );
    ~QY2RelNotesDialog();
    void setRelNotes( const std::map<std::string,std::string>& relnotes );
    void retranslate();

public slots:
    void tabChanged( int index );

protected:
    QVBoxLayout *vboxLayout;
    QTabBar *tabBar;
    QTextBrowser *textBrowser;
    QHBoxLayout *hboxLayout;
    QPushButton *closeButton;


private:
    QList<QTextCursor> _marks;
    std::map<std::string,std::string> _relnotes;
    std::vector<std::string> _tab_keys;

};

#endif

