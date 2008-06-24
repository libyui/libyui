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

    NCPopupInfo( const wpos at,
		 const string & headline,
		 const string & text,
		 // the label of an OK button
		 string okButtonLabel = _( "&OK" ),
		 string cancelButtonLabel = "" );

    virtual ~NCPopupInfo();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const string & headline,
		       const string & text,
		       string okButtonLabel,
		       string cancelButtonLabel );

    NCursesEvent & showInfoPopup( );

    void popup();

    void popdown();

    bool isVisible() { return visible; }

    void setPreferredSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }

    void focusOkButton() { okButton->setKeyboardFocus(); }

    void focusCancelButton() { cancelButton->setKeyboardFocus(); }
 
};



#endif // NCPopupInfo_h
