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

  File:       NCPackageSelectorStart.h

  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPackageSelectorStart_h
#define NCPackageSelectorStart_h

#include <iosfwd>

#include <yui/YPackageSelector.h>
#include <yui/YLayoutBox.h>

#include <yui/ncurses/YNCursesUI.h>
#include <yui/ncurses/NCLayoutBox.h>
#include <yui/ncurses/NCPopupTable.h>

#include "NCPackageSelector.h"


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
