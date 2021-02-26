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

