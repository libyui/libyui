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
#include "NCLayoutBox.h"
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
    : YTableCell(string(" "))
    //: NCTableCol (NCstring(" "), SEPARATOR)
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

NCPkgRepoTable::NCPkgRepoTable( YWidget *parent, YTableHeader *tableHeader )
    :NCTable( parent, tableHeader )
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
    //use default ctor, add cell in the next step 
    YTableItem *tabItem = new YTableItem();	

    //place tag (with repo reference) to the 0th column
    tabItem->addCell( new NCPkgRepoTag ( r ) );

    // and append the rest (name, URL and stuff)
    for ( unsigned i = 1; i < cols.size() + 1; ++i ) {
	tabItem->addCell( cols[ i-1 ]);
    }

    // this is NCTable::addItem( tabItem );
    //it actually appends the line to the table
    addItem( tabItem );   


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

   YTableItem *it = dynamic_cast<YTableItem*> (line->origItem() );

   //get actual repo tag from 0th column of the table
   YTableCell *tcell = it->cell(0);
   NCPkgRepoTag *tag = static_cast<NCPkgRepoTag *>( tcell );

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
    createLayout(  NCPkgNames::RepoLabel() );

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

void NCPkgPopupRepo::createLayout( const string & label) 
{

    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this,  YD_VERT );

    //the headline
    new NCLabel( split, label, true, false );

    // the repositories table
    YTableHeader *tableHeader = new YTableHeader();
    repolist = new NCPkgRepoTable( split, tableHeader );
    repolist->fillHeader();
    
    new NCSpacing( split, YD_VERT, false, 0.4 );

    //the cute button
    okButton = new NCPushButton( split, NCPkgNames::OKLabel() );
    okButton->setFunctionKey(10);

    new NCSpacing( split, YD_VERT, false, 0.4 );

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
        // but not now :) bug #296782	
	//ZyppProduct product = findProductForRepo( (*it) );
        //if ( product ) 
	//{
	//  name = product->summary();
        //}        
        string name = (*it).info().name();

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

    zypp::ResPool::byKind_iterator it = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product );
    zypp::ResPool::byKind_iterator end = zypp::ResPool::instance().byKindEnd( zypp::ResKind::product );

    while( it != end && !product )
    {
	//Single product - most common case
        if ( it->resolvable()->repoInfo().alias() == repo.info().alias() )
            product = zypp::asKind<zypp::Product>( it->resolvable() );
        it++;
    }

    while ( it != end )
    {
        if ( it->resolvable()->repoInfo().alias() == repo.info().alias() )
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

int NCPkgPopupRepo::preferredWidth()
{
    return NCurses::cols()*2/3;
}

int NCPkgPopupRepo::preferredHeight()
{
    if ( NCurses::lines() > 20 )
	return 20;
    else
	return NCurses::lines()-4;
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

    if ( postevent.widget == okButton )
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
