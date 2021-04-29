/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


  File:		NCPackageSelectorPluginImpl.h
  Author:	Hedgehog Painter <kmachalkova@suse.cz>

*/


#ifndef NCPackageSelectorPluginImpl_h
#define NCPackageSelectorPluginImpl_h

#include <yui/ncurses/NCPackageSelectorPluginIf.h>


class NCPackageSelectorPluginImpl : public NCPackageSelectorPluginIf
{

  public:

    virtual ~NCPackageSelectorPluginImpl() {};

    virtual YPackageSelector * createPackageSelector( YWidget *	parent, long modeFlags);

    virtual YEvent *runPkgSelection (YDialog *currentDialog, YWidget *packageSelector );

    virtual YWidget *createPkgSpecial (YWidget *parent, const std::string &subwidget );

};
#endif
