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
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPackageSelectorStart.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPackageSelectorStart_h
#define NCPackageSelectorStart_h

#include <iosfwd>

#include "YNCursesUI.h"
#include "YPackageSelector.h"
#include "YLayoutBox.h"
#include "NCLayoutBox.h"
#include "NCPackageSelector.h"
#include "NCPopupTable.h"


class NCPkgTable;
class NCPackageSelector;
class NCPushButton;

/**
 * @short the package selector widget
 */
class NCPackageSelectorStart : public NCLayoutBox
{

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPackageSelectorStart & OBJ );

  NCPackageSelectorStart & operator=( const NCPackageSelectorStart & );
  NCPackageSelectorStart            ( const NCPackageSelectorStart & );

  private:

    NCPackageSelector *packager;	// packager object contains the data and handles events

  protected:

    virtual const char * location() const
    {
      return primary() == YD_HORIZ ? "NC(H)PackageSelectorStart" : "NC(V)PackageSelectorStart";
    }

  public:

    /**
     * Constructor
     * creates the widget tree of the package selector
     */
    NCPackageSelectorStart( YWidget * 	parent,
			    long 	modeFlags,
			    YUIDimension dimension );

    /**
     * Destructor
     */
    virtual ~NCPackageSelectorStart();

    virtual int preferredWidth() { return NCLayoutBox::preferredWidth(); }
    virtual int preferredHeight() { return NCLayoutBox::preferredHeight(); }

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Fills the package table with packages belonging to the
     * default filter (the filter which is selected when entering the
     * package selection).
     **/
    void showDefaultList();

    /**
     * Pass the event to the handleEvent method of the member variable
     * NCPackageSelector packager.
     * @param event The NCursesEvent
     * @return bool
     */
    bool handleEvent( const NCursesEvent&   event );

};

///////////////////////////////////////////////////////////////////

#endif // NCPackageSelectorStart_h
