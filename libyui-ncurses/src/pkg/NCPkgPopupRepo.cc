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

   File:       NCPkgPopupRepo.cc

   Author:     Bubli <kmachalkova@suse.cz> 

/-*/

#include "Y2Log.h"
#include "NCPkgPopupRepo.h"

#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"

#include "NCZypp.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTag::NCPkgRepoTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgRepoTag::NCPkgRepoTag ( ZyppRepo repoPtr)
    : NCTableCol (NCstring(" "), SEPARATOR)
      , repo (repoPtr)
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTable::NCPkgRepoTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgRepoTable::NCPkgRepoTable( NCWidget *parent, const YWidgetOpt & opt)
    :NCTable( parent, opt, vector<string> (), false )
{
   fillHeader();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTable::fillHeader
//	METHOD TYPE : void
//
//	DESCRIPTION : Fill header of repositories table (name + URL)
//

void NCPkgRepoTable::fillHeader() 
{
    vector <string> header;

    header.reserve(3);
    header.push_back( "L" );
    header.push_back( "L" + NCPkgNames::PkgName() );
    header.push_back( "L" + NCPkgNames::RepoURL() );

    setHeader( header);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTable::addLine
//	METHOD TYPE : void
//
//	DESCRIPTION : Add one line (with tag) to the repository table
//

void NCPkgRepoTable::addLine ( ZyppRepo r, const vector <string> & cols )
{
    vector <NCTableCol*> items ( cols.size() + 1, 0);

    //place tag (with repo reference) to the 0th column
    items[0] = new NCPkgRepoTag ( r );

    // and append the rest (name, URL and stuff)
    for ( unsigned i = 1; i < cols.size() + 1; ++i ) {
        items[i] = new NCTableCol( YCPString( cols[i - 1] ) );
    }

    myPad()->Append( items);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTable::getTag
//	METHOD TYPE : NCPkgRepoTag *
//
//	DESCRIPTION : Get tag of repository table line on current index,
//		      ( contains repository reference)
//

NCPkgRepoTag* NCPkgRepoTable::getTag (const int & index )
{
   NCTableLine *line = myPad()->ModifyLine( index );

   if ( !line )
   {
	return 0;
   }
   //get actual repo tag from 0th column of the table
   NCPkgRepoTag *tag = static_cast<NCPkgRepoTag *>( line->GetCol( 0 ) );

   return tag;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgRepoTable::getRepo
//	METHOD TYPE : ZyppRepo
//
//	DESCRIPTION : Get repository reference from selected line's tag 
//

ZyppRepo NCPkgRepoTable::getRepo( int index )
{
    NCPkgRepoTag *t = getTag( index );

    if ( !t )
    {
	return ZyppRepo( );
    }
    else 
    {
	 return t->getRepo();
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::NCPkgPopupRepo
//	METHOD TYPE : Constructor 
//
//	DESCRIPTION : 
//

NCPkgPopupRepo::NCPkgPopupRepo (const wpos at, NCPackageSelector *pkg) 
    : NCPopup (at, false)
    , repolist( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    createLayout( YCPString( NCPkgNames::RepoLabel()) );

    fillRepoList( );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::~NCPkgPopupRepo
//	METHOD TYPE : Destructor 
//
//	DESCRIPTION : 
//

NCPkgPopupRepo::~NCPkgPopupRepo()
{
  //NOP
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::createLayout
//	METHOD TYPE : void 
//
//	DESCRIPTION : Create visual layout of the popup window
// 		      (header label, NCPkgRepoTable, OK button)
//

void NCPkgPopupRepo::createLayout( const YCPString & label) 
{
    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );

    opt.notifyMode.setValue( false );

    //the headline
    opt.isHeading.setValue( true );

    NCLabel * head = new NCLabel( split, opt, label );
    split->addChild( head );
   
    // the repositories table
    repolist = new NCPkgRepoTable( split, opt );
    split->addChild( repolist);
    
    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );

    opt.key_Fxx.setValue( 10 );

    //the cute button
    okButton = new NCPushButton( split, opt, YCPString(NCPkgNames::OKLabel()) );
    okButton->setId( NCPkgNames::OkButton () );

    split->addChild( okButton );

    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::fillRepoList
//	METHOD TYPE : bool 
//
//	DESCRIPTION : Add items to the repository list (assoc.
//		      product name, if any, and URL)
//

bool NCPkgPopupRepo::fillRepoList()
{
    NCMIL << "Filling repository list" << endl;

    vector <string> oneLine;

    //iterate through all repositories
    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd(); 
          ++it)
    {
	oneLine.clear();

	// let's find some product for this repository	
	ZyppProduct product = findProductForRepo( (*it) );
        string name = "";

        if ( product ) 
	{
	    name = product->summary();
        }        
      	oneLine.push_back( name ); 

	//and URL as well
        zypp::Url srcUrl;
        if ( ! (*it).info().baseUrlsEmpty() )
           srcUrl = *(*it).info().baseUrlsBegin();

	oneLine.push_back( srcUrl.asString() );
	//add the whole stuff to the table
        repolist->addLine( (*it), oneLine);    
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::findProductForRepo 
//	METHOD TYPE : ZyppProduct 
//
//	DESCRIPTION : Find single zypp::Product for this repository
//		      (null product if multiple products found)
//

ZyppProduct NCPkgPopupRepo::findProductForRepo( ZyppRepo repo)
{

    ZyppProduct product;

    zypp::ResStore::iterator it = repo.resolvables().begin();

    while( it != repo.resolvables().end() && !product )
    {
	//Single product - most common case
        product = zypp::dynamic_pointer_cast<zypp::Product>( *it );
        it++;
    }

    while ( it != repo.resolvables().end() )
    {
        if ( zypp::dynamic_pointer_cast<zypp::Product>( *it ) )
        {
	    //Aw, multiple product found, we don't want those
            NCWAR << "Multiple products in repository " <<
                     repo.info().alias().c_str() << endl;
            ZyppProduct null;
            return null;
        }

        ++it;
    }

   if ( !product )
   {
	//bad luck, nothing found
	NCMIL << "No product in repository " <<
                 repo.info().alias().c_str() << endl;
   } 

   return product;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::nicesize 
//	METHOD TYPE : long 
//
//	DESCRIPTION : default boring setting nice size
//

long NCPkgPopupRepo::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 20 )
        vdim = 20;
    else
        vdim = NCurses::lines()-4;

    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::wHandleInput 
//	METHOD TYPE : NCursesEvent 
//
//	DESCRIPTION : default boring handle-input
//

NCursesEvent NCPkgPopupRepo::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
        return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::postAgain
//	METHOD TYPE : bool 
//
//	DESCRIPTION : default boring postAgain
//

bool NCPkgPopupRepo::postAgain() 
{
    if( !postevent.widget )
        return false;

    postevent.detail = NCursesEvent::NODETAIL;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull()
         && currentId->compare( NCPkgNames::OkButton () ) == YO_EQUAL )
    {
        postevent.detail = NCursesEvent::USERDEF ;
        // return false means: close the popup
        return false;
    }

    if (postevent == NCursesEvent::cancel)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupRepo::showRepoPopup
//	METHOD TYPE : NCursesEvent &
//
//	DESCRIPTION : Do actual work - show popup to the user,
//		      collect packages for selected repository
//		      (calls NCPackageSelector::fillRepoFilterList)
//

NCursesEvent & NCPkgPopupRepo::showRepoPopup()
{
    postevent = NCursesEvent();

    if (!repolist)
	return postevent;

    do {
        popupDialog();
    } while ( postAgain() );

    popdownDialog();

    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	//get current table line and its repository reference
	int index = repolist->getCurrentItem();
	ZyppRepo repo = repolist->getRepo( index );

        NCMIL << "Selected repository " << repo.info().alias().c_str() << endl;

	//and show associated packages to the user
        packager->fillRepoFilterList( repo );

    }
    return postevent;
}
