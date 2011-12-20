
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |*************************************************************************** */

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

