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

NCPkgRepoTag::NCPkgRepoTag ( ZyppRepo repoPtr)
    : NCTableCol (NCstring(" "), SEPARATOR)
      , repo (repoPtr)
{

}

NCPkgRepoTable::NCPkgRepoTable( NCWidget *parent, const YWidgetOpt & opt)
    :NCTable( parent, opt, vector<string> (), false )
{
   fillHeader();
}

void NCPkgRepoTable::fillHeader() 
{
    vector <string> header;

    header.reserve(3);
    header.push_back( "L" );
    header.push_back( "L" + NCPkgNames::PkgName() );
    header.push_back( "L" + NCPkgNames::RepoURL() );

    setHeader( header);
}

void NCPkgRepoTable::addLine ( ZyppRepo r, const vector <string> & cols )
{
    vector <NCTableCol*> items ( cols.size() + 1, 0);

    items[0] = new NCPkgRepoTag ( r );


    for ( unsigned i = 1; i < cols.size() + 1; ++i ) {
        items[i] = new NCTableCol( YCPString( cols[i - 1] ) );
    }

    myPad()->Append( items);
}

NCPkgRepoTag* NCPkgRepoTable::getTag (const int & index )
{
   NCTableLine *line = myPad()->ModifyLine( index );

   if ( !line )
   {
	return 0;
   }

   NCPkgRepoTag *tag = static_cast<NCPkgRepoTag *>( line->GetCol( 0 ) );

   return tag;
}

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

NCPkgPopupRepo::NCPkgPopupRepo (const wpos at, NCPackageSelector *pkg) 
    : NCPopup (at, false)
    , repolist( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    createLayout( YCPString( NCPkgNames::RepoLabel()) );

    fillRepoList( );
}

NCPkgPopupRepo::~NCPkgPopupRepo()
{
  //NOP
}

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

    repolist = new NCPkgRepoTable( split, opt );
    split->addChild( repolist);
    
    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );

    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, opt, YCPString(NCPkgNames::OKLabel()) );
    okButton->setId( NCPkgNames::OkButton () );

    split->addChild( okButton );

    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );
}

bool NCPkgPopupRepo::fillRepoList()
{
    NCMIL << "Filling repository list" << endl;

    vector <string> oneLine;
    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd(); 
          ++it)
    {
	oneLine.clear();
	
	ZyppProduct product = findProductForRepo( (*it) );
        string name = "";

        if ( product ) 
	{
	    name = product->summary();
        }        
      	oneLine.push_back( name ); 

        zypp::Url srcUrl;
        if ( ! (*it).info().baseUrlsEmpty() )
           srcUrl = *(*it).info().baseUrlsBegin();

	oneLine.push_back( srcUrl.asString() );
        repolist->addLine( (*it), oneLine);    
    }
    
    return true;
}

ZyppProduct NCPkgPopupRepo::findProductForRepo( ZyppRepo repo)
{

    ZyppProduct product;

    zypp::ResStore::iterator it = repo.resolvables().begin();

    while( it != repo.resolvables().end() && !product )
    {
        product = zypp::dynamic_pointer_cast<zypp::Product>( *it );
        it++;
    }

    while ( it != repo.resolvables().end() )
    {
        if ( zypp::dynamic_pointer_cast<zypp::Product>( *it ) )
        {
            NCMIL << "Multiple products in repository " <<
                     repo.info().alias().c_str() << endl;
            ZyppProduct null;
            return null;
        }

        ++it;
    }

   if ( !product )
   {
	NCMIL << "No product in repository " <<
                 repo.info().alias().c_str() << endl;
   } 

   return product;
}

long NCPkgPopupRepo::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 20 )
        vdim = 20;
    else
        vdim = NCurses::lines()-4;

    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
}

NCursesEvent NCPkgPopupRepo::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
        return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

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

	int index = repolist->getCurrentItem();
	ZyppRepo repo = repolist->getRepo( index );

        NCMIL << "Selected repository " << repo.info().alias().c_str << endl;

        packager->fillRepoFilterList( repo );

    }
    return postevent;
}
