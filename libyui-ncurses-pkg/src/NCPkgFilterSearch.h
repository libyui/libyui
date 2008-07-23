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

   File:       NCPkgFilterSearch.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgFilterSearch_h
#define NCPkgFilterSearch_h

#include <iosfwd>

#include <vector>
#include <string>
#include <set>
using namespace std;
using std::string;

#include "NCPopup.h"
#include "NCInputField.h"
#include "NCLabel.h"
#include "NCLayoutBox.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCComboBox.h"
#include "NCPushButton.h"

class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterSearch
//
//	DESCRIPTION :
//
class NCPkgFilterSearch : public NCLayoutBox {

    NCPkgFilterSearch & operator=( const NCPkgFilterSearch & );
    NCPkgFilterSearch            ( const NCPkgFilterSearch & );

private:

    enum SearchMode {
	Contains = 0,
	BeginsWith,
	ExactMatch,
	UseWildcard,
	UseRegexp
    };

    NCInputField * searchExpr;
    NCCheckBox * ignoreCase;
    NCComboBox * searchMode;

    NCPackageSelector * packager;		// connection to the package selector

    bool getCheckBoxValue( NCCheckBox * checkBox );
    
protected:

    string getSearchExpression() const;
    
public:
    
    NCPkgFilterSearch( YWidget *parent, YUIDimension dim, NCPackageSelector *pkger );
    virtual ~NCPkgFilterSearch();

    void createLayout( YWidget *parent );

    virtual bool showSearchResultPackages();

    bool match ( string s1, string s2, bool ignoreCase );
    bool fillSearchList( string & expr,
			 bool ignoreCase,
			 bool checkName,
			 bool checkSummary,
			 bool checkDescr,
			 bool checkProvides,
			 bool checkRequires );


};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterSearch_h
