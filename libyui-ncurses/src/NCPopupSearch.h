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

   File:       NCPopupSearch.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupSearch_h
#define NCPopupSearch_h

#include <iosfwd>

#include <vector>
#include <string>
using namespace std;

#include "NCPopup.h"
#include "NCComboBox.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCPushButton.h"

class YCPValue;
class PackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupSearch
//
//	DESCRIPTION :
//
class NCPopupSearch : public NCPopup {

    NCPopupSearch & operator=( const NCPopupSearch & );
    NCPopupSearch            ( const NCPopupSearch & );

private:

    NCComboBox * searchExpr;
    NCCheckBox * ignoreCase;
    NCCheckBox * checkName;
    NCCheckBox * checkSummary;
    NCCheckBox * checkDescr;
    NCCheckBox * checkProvides;
    NCCheckBox * checkRequires;
    
    NCPushButton * cancelButton;
    NCPushButton * okButton;

    PackageSelector * packager;		// connection to the package selector

    bool getCheckBoxValue( NCCheckBox * checkBox );
    
protected:

    YCPString getSearchExpression() const;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPopupSearch( const wpos at, PackageSelector *pkger );
    virtual ~NCPopupSearch();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const YCPString & headline );

    NCursesEvent & showSearchPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupSearch_h
