/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
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
  |****************************************************************************
*/


/*-/

  File:		YQPackageSelectorPluginIf.h

  Author:	kmachalkova@suse.cz


/-*/

#ifndef YQPackageSelectorPluginIf_h
#define YQPackageSelectorPluginIf_h

#include <YDialog.h>
#include <YEvent.h>
#include <YPackageSelectorPlugin.h>

class YQPackageSelectorPluginIf
{

  public:

    virtual ~YQPackageSelectorPluginIf() {}

    virtual YPackageSelector *createPackageSelector( YWidget *parent, long modeFlags ) = 0 ;
    virtual YWidget *createPatternSelector( YWidget * parent, long modeFlags ) = 0;
    virtual YWidget *createSimplePatchSelector( YWidget * parent, long modeFlags ) = 0;
};

#endif
