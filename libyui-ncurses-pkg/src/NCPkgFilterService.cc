/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| Copyright (c) [2018] SUSE LLC
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact SUSE.
|
| To contact SUSE about this file by physical or electronic mail,
| you may find current contact information at www.suse.com
|
|***************************************************************************/

#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include <zypp/ServiceInfo.h>

#include "NCPkgFilterService.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPackageSelector.h"

#include "NCZypp.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTag::NCPkgServiceTag
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgServiceTag::NCPkgServiceTag ( ZyppService servicePtr)
    : YTableCell(std::string(" "))
    , service (servicePtr)
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTable::NCPkgServiceTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgServiceTable::NCPkgServiceTable( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg )
    :NCTable( parent, tableHeader )
    ,packager(pkg)
    ,repo_manager(new zypp::RepoManager())
{
   fillHeader();
   fillServiceList();
}

bool NCPkgServiceTable::any_service()
{
    bool ret = std::any_of(ZyppRepositoriesBegin(), ZyppRepositoriesEnd(), [](const zypp::Repository& repo) {
        // if the repository does not belong to any service then the service name is empty
        return !repo.info().service().empty();
    });

    yuiMilestone() << "Found a libzypp service: " << ret << std::endl;
    return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTable::fillHeader
//	METHOD TYPE : void
//
//	DESCRIPTION : Fill header of servicesitories table (name + URL)
//

void NCPkgServiceTable::fillHeader()
{
    std::vector <std::string> header;

    header.reserve(2);
    header.push_back( "L" );
    header.push_back( "L" + NCPkgStrings::PkgName() );

    setHeader( header);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTable::addLine
//	METHOD TYPE : void
//
//	DESCRIPTION : Add one line (with tag) to the service table
//

void NCPkgServiceTable::addLine ( ZyppService svc, const std::vector <std::string> & cols )
{
    //use default ctor, add cell in the next step
    YTableItem *tabItem = new YTableItem();

    //place tag (with service reference) to the 0th column
    tabItem->addCell( new NCPkgServiceTag (svc) );

    // and append the rest (name, URL and stuff)
    for(const std::string& s: cols) {
	tabItem->addCell(s);
    };

    // this is NCTable::addItem( tabItem );
    //it actually appends the line to the table
    addItem( tabItem );


}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTable::getTag
//	METHOD TYPE : NCPkgServiceTag *
//
//	DESCRIPTION : Get tag of service table line on current index,
//		      (contains service reference)
//

NCPkgServiceTag* NCPkgServiceTable::getTag (int index)
{
   NCTableLine *line = myPad()->ModifyLine( index );
   if ( !line )
   {
	return nullptr;
   }

   YTableItem *it = line->origItem();

   //get actual service tag from 0th column of the table
   YTableCell *tcell = it->cell(0);
   NCPkgServiceTag *tag = static_cast<NCPkgServiceTag *>( tcell );

   return tag;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgServiceTable::getService
//	METHOD TYPE : ZyppService
//
//	DESCRIPTION : Get service reference from selected line's tag
//

ZyppService NCPkgServiceTable::getService( int index )
{
    NCPkgServiceTag *t = getTag( index );

    return t ? t->getService() : ZyppService();
}

std::string NCPkgServiceTable::showDescription( ZyppService r)
{
    zypp::ServiceInfo si = repo_manager->getService(r);

    std::string label = _( "<b>Service URL:</b>" );
    std::string ret = label + si.url().asString();
    return ret;
}

/////////////////////////////////////////////////////////////////////
////
////
////	METHOD NAME : NCPkgFilterService::fillServiceList
////	METHOD TYPE : bool
////
////	DESCRIPTION : Add items to the service list (assoc.
////		      product name, if any, and URL)
////
//
bool NCPkgServiceTable::fillServiceList()
{
    yuiMilestone() << "Filling service list" << endl;

    std::set<std::string> seen_services;

    //iterate through all repositories
    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd();
          ++it)
    {
        const std::string &service_name(it->info().service());
        if (!service_name.empty())
        {
            if (seen_services.find(service_name) == seen_services.end())
            {
              seen_services.insert(service_name);

              std::vector <std::string> oneLine;
              oneLine.push_back( service_name );
              addLine( service_name, oneLine);
            }
        }
    }

    return true;
}

bool NCPkgServiceTable::showServicePackages( )
{
    int index = getCurrentItem();
    ZyppService service = getService( index );

    yuiMilestone() << "Selected service " << service << endl;
    yuiMilestone() << "Collecting packages in selected service" << endl;

    NCPkgTable *pkgList = packager->PackageList();
    //clean the pkg table first
    pkgList->itemsCleared ();

    zypp::PoolQuery q;
    q.addKind( zypp::ResKind::package );

    std::for_each(ZyppRepositoriesBegin(), ZyppRepositoriesEnd(), [&](const zypp::Repository& repo) {
        if (service == repo.info().service())
        {
          yuiMilestone() << "Adding repo filter: " << repo.info().alias() << std::endl;
          q.addRepo( repo.info().alias() );
        }
    });

    for( zypp::PoolQuery::Selectable_iterator it = q.selectableBegin();
	it != q.selectableEnd(); it++)
    {
        ZyppPkg pkg = tryCastToZyppPkg( (*it)->theObj() );
        pkgList->createListEntry ( pkg, *it);
    }

    packager->FilterDescription()->setText( showDescription( service ) );

    pkgList->setCurrentItem( 0 );
    pkgList->drawList();
    pkgList->showInformation();

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterService::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION : default boring handle-input
//

NCursesEvent NCPkgServiceTable::wHandleInput( wint_t ch )
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
            showServicePackages();
	    break;
	}

	default:
	   ret = NCTable::wHandleInput( ch ) ;
     }

    return ret;
}

