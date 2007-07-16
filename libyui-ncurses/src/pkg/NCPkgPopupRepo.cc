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

NCPkgPopupRepo::NCPkgPopupRepo (const wpos at, NCPackageSelector *pkg) 
    : NCPopup (at, false)
    , repolist( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    createLayout( YCPString( NCPkgNames::RepoLabel()) );

//    fillRepoList( );
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

    repolist = new NCTable( split, opt, vector <string> (), false);
    fillHeader();
    split->addChild( repolist);
    
    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );

    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, opt, YCPString(NCPkgNames::OKLabel()) );
    okButton->setId( NCPkgNames::OkButton () );

    split->addChild( okButton );

    split->addChild ( new NCSpacing( split, opt, 0.4, false, true) );
}

void NCPkgPopupRepo::fillHeader() 
{
    vector <string> header;

    header.reserve(3);
    header.push_back( "L" + NCPkgNames::PkgName() );
    header.push_back( "L" + NCPkgNames::RepoURL() );

    repolist->setHeader( header);
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

    return postevent;
}
