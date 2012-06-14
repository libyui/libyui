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

  File:	      NCPackageSelectorPluginStub.h

  Author:     Katharina Machalkova <kmachalkova@suse.cz>

/-*/

#ifndef NCPackageSelectorPluginStub_h
#define NCPackageSelectorPluginStub_h

#include <yui/YPackageSelectorPlugin.h>
#include <yui/YDialog.h>
#include <yui/YEvent.h>

#include "NCPackageSelectorPluginIf.h"

/**
 * Simplified access to the ncurses UI's package selector plugin.
 **/

class NCPackageSelectorPluginIf;

class NCPackageSelectorPluginStub: public YPackageSelectorPlugin
{
public:

    /**
     * Constructor: Load the plugin library for the NCurses package selector.
     **/
    NCPackageSelectorPluginStub();

    /**
     * Destructor. Calls dlclose() which will unload the plugin library if it
     * is no longer used, i.e. if the reference count dlopen() uses reaches 0.
     **/
    virtual ~NCPackageSelectorPluginStub();

    /**
     * Create a package selector.
     * Implemented from YPackageSelectorPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YPackageSelector * createPackageSelector( YWidget *		parent,
						      long		modeFlags );

    /**
     * Fills the PackageSelector widget (runs the package selection).
     */
    virtual YEvent * runPkgSelection( YDialog *currentDialog,
				      YWidget * packageSelector );

    /**
     * Create a special widget
     */
    virtual YWidget * createPkgSpecial( YWidget *parent,
					const std::string &subwidget );

    NCPackageSelectorPluginIf *impl ;
};


#endif // NCPackageSelectorPluginStub_h
