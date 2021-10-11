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

protected:
    
    /**
     * Constructor: Creates a History dialog for all packages that match
     * 'pkgName'. Use the static 'showHistoryDialog()' method instead.
     **/
    YQPkgHistoryDialog( QWidget * parent );

public:

    /**
     * Static convenience method: Post a History dialog for pkg 'pkgName'.
     **/
    static void showHistoryDialog( QWidget* parent = 0);

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint() const;


protected slots:

    void moveToDate();
    void moveToAction();


protected:

    void initialize();


    // Data members

    QTreeWidget * _dates;
    QTreeWidget * _actions;
};


#endif // ifndef YQPkgHistoryDialog_h
