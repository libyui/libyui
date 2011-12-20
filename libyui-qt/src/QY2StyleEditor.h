
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

#ifndef _QY2StyleEditor_h
#define _QY2StyleEditor_h

#include <QDialog>
#include "ui_QStyleEditor.h"


/**
 * @short Stylesheet Editor Dialog
 **/

class QY2StyleEditor : public QDialog
{
    Q_OBJECT

public:

    /**
    * Constructor.
    **/
    QY2StyleEditor( QWidget *parent );

    /**
    * Destructor.
    **/
    ~QY2StyleEditor();

private slots:

    /**
    * sets the text written in the text browser as style sheet
    **/
    void slotApplyStyle();

    /**
    * loads a style sheet form a file
    **/
    void slotLoadFile();

    /**
    * triggers style sheet auto apply if user has selected 
    **/
    void slotTextChanged();

private:
     Ui_QStyleEditor ui;

};

#endif

