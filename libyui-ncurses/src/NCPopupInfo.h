/*
  Copyright (C) 2000-2012 Novell, Inc
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


/*-/

   File:       NCPopupInfo.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCPopupInfo_h
#define NCPopupInfo_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCComboBox.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCPushButton.h"
#include "NCi18n.h"


/*
  Textdomain "ncurses"
 */



class NCPopupInfo : public NCPopup
{
private:

    NCPopupInfo & operator=( const NCPopupInfo & );
    NCPopupInfo( const NCPopupInfo & );


    NCRichText * helpText;
    NCPushButton * okButton;
    NCPushButton * cancelButton;

    int hDim;
    int vDim;
    bool visible;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPopupInfo( const wpos & at,
		 const std::string & headline,
		 const std::string & text,
		 // the label of an OK button
		 std::string okButtonLabel = _( "&OK" ),
		 std::string cancelButtonLabel = "" );

    virtual ~NCPopupInfo();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const std::string & headline,
		       const std::string & text,
		       std::string okButtonLabel,
		       std::string cancelButtonLabel );

    NCursesEvent & showInfoPopup();

    void popup();

    void popdown();

    bool isVisible() { return visible; }

    void setPreferredSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }

    void focusOkButton() { okButton->setKeyboardFocus(); }

    void focusCancelButton() { cancelButton->setKeyboardFocus(); }

};



#endif // NCPopupInfo_h
