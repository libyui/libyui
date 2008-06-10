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

   File:       NCPkgPatchSearch.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgPatchSearch_h
#define NCPkgPatchSearch_h

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
//	CLASS NAME : NCPkgPatchSearch
//
//	DESCRIPTION :
//
class NCPkgPatchSearch : public NCPopup {

    NCPkgPatchSearch & operator=( const NCPkgPatchSearch & );
    NCPkgPatchSearch            ( const NCPkgPatchSearch & );

private:

    NCComboBox * searchExpr;
    
    NCPushButton * cancelButton;
    NCPushButton * okButton;

    NCPackageSelector * packager;		// connection to the package selector

protected:

    string getSearchExpression() const;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgPatchSearch( const wpos at, NCPackageSelector *pkger );
    virtual ~NCPkgPatchSearch();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const string & headline );

    NCursesEvent & showSearchPopup( );
    
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPatchSearch_h
