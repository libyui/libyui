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

  File:	      YQPkgDiskUsageWarningDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgDiskUsageWarningDialog_h
#define YQPkgDiskUsageWarningDialog_h

#include <qdialog.h>


class YQPkgDiskUsageList;


/**
 * Warning dialog about partitions that are getting full or overflowing.
 **/
class YQPkgDiskUsageWarningDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Static convenience method: Post a disk usage warning with text
     * 'message', a list of partitions that are at least 'thresholdPercent'
     * full and one ( default ) or two buttons.
     *
     * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
     * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
     * the window manager close button ).
     **/
    static bool diskUsageWarning( const QString & 	message,
				  int			thresholdPercent,
				  const QString &	acceptButtonLabel,
				  const QString &	rejectButtonLabel = QString::null );
protected:

    /**
     * Constructor: Creates a disk usage warning dialog with text 'message' on
     * top, a list of partitions that are at least 'thresholdPercent' full and
     * one ( default ) or two buttons.
     *
     * Not meant for public use. Applications should use the static
     * 'diskUsageWarning' method instead.
     **/
    YQPkgDiskUsageWarningDialog( QWidget *		parent,
				 const QString & 	message,
				 int			thresholdPercent,
				 const QString &	acceptButtonLabel,
				 const QString &	rejectButtonLabel = QString::null );
};


#endif // ifndef YQPkgDiskUsageWarningDialog_h
