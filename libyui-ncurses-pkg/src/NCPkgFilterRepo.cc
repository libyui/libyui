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

   File:       NCPkgFilterRepo.cc

   Author:     Bubli <kmachalkova@suse.cz> 

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterRepo.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPackageSelector.h"

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

NCPkgRepoTable::NCPkgRepoTable( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg )
    :NCTable( parent, tableHeader )
    ,packager(pkg)
{
   fillHeader();
   fillRepoList();
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
    header.push_back( "L" + NCPkgStrings::PkgName() );
    header.push_back( "L" + NCPkgStrings::RepoURL() );

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

/////////////////////////////////////////////////////////////////////
////
////
////	METHOD NAME : NCPkgFilterRepo::fillRepoList
////	METHOD TYPE : bool 
////
////	DESCRIPTION : Add items to the repository list (assoc.
////		      product name, if any, and URL)
////
//
bool NCPkgRepoTable::fillRepoList()
{
    yuiMilestone() << "Filling repository list" << endl;

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
        addLine( (*it), oneLine);    
    }
    
    return true;
}

bool NCPkgRepoTable::showRepoPackages( )
{
    int index = getCurrentItem();
    ZyppRepo repo = getRepo( index );

    yuiMilestone() << "Selected repository " << repo.info().alias().c_str() << endl;
    yuiMilestone() << "Collecting packages in selected repository" << endl;

    NCPkgTable *pkgList = packager->PackageList();
    //clean the pkg table first
    pkgList->itemsCleared ();

    //sets to store matching packages
    set <ZyppSel> exactMatch;
    set <ZyppSel> nearMatch;

    //iterate through the package pool
    for ( ZyppPoolIterator it = zyppPkgBegin();
          it != zyppPkgEnd();
          ++it )
    {
	//we have candidate object in this repository
        if ( (*it)->candidateObj() &&
             (*it)->candidateObj()->repository() == repo )
        {
            exactMatch.insert( *it );
        }
	//something else (?)
        else
        {
            zypp::ui::Selectable::available_iterator pkg_it = (*it)->availableBegin();

            while ( pkg_it != (*it)->availableEnd() )
            {
                if ( (*pkg_it)->repository() == repo )
                    nearMatch.insert( *it );

                ++pkg_it;
            }
        }

      }

    //finally create pkg table list entries
    set<ZyppSel>::const_iterator e_it = exactMatch.begin();
    while ( e_it != exactMatch.end() )
    {
        ZyppPkg pkg = tryCastToZyppPkg( (*e_it)->theObj() );
        pkgList->createListEntry ( pkg, *e_it);
	e_it++;
    }

    set<ZyppSel>::const_iterator n_it = nearMatch.begin();
    while ( n_it != nearMatch.end() )
    {
        ZyppPkg pkg = tryCastToZyppPkg( (*n_it)->theObj() );
        pkgList->createListEntry ( pkg, *n_it);
	n_it++;
    }

    //and show the whole stuff to the user
    pkgList->drawList();

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRepo::findProductForRepo 
//	METHOD TYPE : ZyppProduct 
//
//	DESCRIPTION : Find single zypp::Product for this repository
//		      (null product if multiple products found)
//

ZyppProduct NCPkgRepoTable::findProductForRepo( ZyppRepo repo)
{

    ZyppProduct product;

    zypp::ResPool::byKind_iterator beg = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product); 
    zypp::ResPool::byKind_iterator end = zypp::ResPool::instance().byKindEnd( zypp::ResKind::product); 

    while( beg != end && !product )
    {
	//Single product - most common case
	if ( beg->resolvable()->repoInfo().alias() == repo.info().alias() )
            product = zypp::asKind<zypp::Product>( beg->resolvable() );
        beg++;
    }

    while ( beg != end )
    {
	if ( beg->resolvable()->repoInfo().alias() == repo.info().alias() )
        {
	    //Aw, multiple product found, we don't want those
            yuiWarning() << "Multiple products in repository " <<
                     repo.info().alias().c_str() << endl;
            ZyppProduct null;
            return null;
        }

        beg++;
    }

   if ( !product )
   {
	//bad luck, nothing found
	yuiMilestone() << "No product in repository " <<
                 repo.info().alias().c_str() << endl;
   } 

   return product;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRepo::wHandleInput 
//	METHOD TYPE : NCursesEvent 
//
//	DESCRIPTION : default boring handle-input
//

NCursesEvent NCPkgRepoTable::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;
    handleInput( ch );

    switch ( ch )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME: {
	    ret = NCursesEvent::handled;
            showRepoPackages();
	    break;
	}

	default:
	   ret = NCTable::wHandleInput( ch ) ;
     }

    return ret;
}

