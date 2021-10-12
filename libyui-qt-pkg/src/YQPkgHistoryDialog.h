/*
  Copyright (c) 2000 - 2011 Novell, Inc.
  Copyright (c) 2018 - 2021 SUSE LLC

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

/*
  File:	      YQPkgHistoryDialog.h
  Author:     Stanislav Visnovsky <visnov@suse.com>
*/


#ifndef YQPkgHistoryDialog_h
#define YQPkgHistoryDialog_h

#include <qdialog.h>


class YQPkgList;
class YQPkgHistoryView;
class QTreeWidget;


/**
 * Pkg status and History as a standalone popup dialog.
 **/
class YQPkgHistoryDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a history dialog.
     **/
    static void showHistoryDialog( QWidget* parent = 0);


protected:

    /**
     * Constructor: Creates a dialog for the zypp history.
     **/
    YQPkgHistoryDialog( QWidget * parent );

    /**
     * Fill the trees with content.
     **/
    void populate();

    void showReadHistoryWarning( const QString & message );


protected slots:

    void selectDate();
    void selectAction();


protected:

    // Data members

    QTreeWidget * _datesTree;
    QTreeWidget * _actionsTree;
};


#endif // ifndef YQPkgHistoryDialog_h
