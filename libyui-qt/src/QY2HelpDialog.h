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


#ifndef _QY2HelpDialog_h
#define _QY2HelpDialog_h

#include <QDialog>

class QTextCursor;
class Ui_QHelpDialog;

class QY2HelpDialog : public QDialog
{
    Q_OBJECT

public:
    QY2HelpDialog( const QString &helpText, QWidget *parent );
    ~QY2HelpDialog();
    void setHelpText( const QString &helpText );
    void retranslate();

public slots:
    void searchStringChanged( QString );

private:
    Ui_QHelpDialog *_ui;
    QList<QTextCursor> _marks;


};

#endif

