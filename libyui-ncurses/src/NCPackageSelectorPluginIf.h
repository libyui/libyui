/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

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

  File:		NCPackageSelectorPluginIf.h

  Author:	Katharina Machalkova <kmachalkova@suse.cz>


/-*/

#ifndef NCPackageSelectorPluginIf_h
#define NCPackageSelectorPluginIf_h

#include <YDialog.h>
#include <YEvent.h>
#include <YPackageSelectorPlugin.h>

class NCPackageSelectorPluginIf
{

public:

    virtual ~NCPackageSelectorPluginIf() {}

    virtual YPackageSelector *createPackageSelector( YWidget *parent, long modeFlags ) = 0 ;

    virtual YEvent *runPkgSelection( YDialog *currentDialog, YWidget *packageSelector ) = 0;

    virtual YWidget *createPkgSpecial( YWidget *parent, const string &subwidget ) = 0;
};


#endif
