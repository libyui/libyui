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

   File:       NCPkgPopupSearch.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupSearch_h
#define NCPkgPopupSearch_h

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

class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupSearch
//
//	DESCRIPTION :
//
class NCPkgPopupSearch : public NCPopup {

    NCPkgPopupSearch & operator=( const NCPkgPopupSearch & );
    NCPkgPopupSearch            ( const NCPkgPopupSearch & );

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

    NCPackageSelector * packager;		// connection to the package selector

    bool getCheckBoxValue( NCCheckBox * checkBox );
    
protected:

    string getSearchExpression() const;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPopupSearch( const wpos at, NCPackageSelector *pkger );
    virtual ~NCPkgPopupSearch();

    virtual long nicesize(YUIDimension dim);

    void createLayout( const string & headline );

    NCursesEvent & showSearchPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupSearch_h
