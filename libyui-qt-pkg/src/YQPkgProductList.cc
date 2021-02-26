/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgProductList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgProductList.h"

using std::list;
using std::set;


YQPkgProductList::YQPkgProductList( QWidget * parent )
    : YQPkgObjList( parent )
    , _vendorCol( -42 )
{
    yuiDebug() << "Creating product list" << endl;

    QStringList headers;
    int numCol = 0;
    headers << ( "" );			_statusCol	= numCol++;
    headers << _( "Product"	);	_nameCol	= numCol++;
    headers <<  _( "Summary"	);	_summaryCol	= numCol++;
    headers <<  _( "Version" 	);	_versionCol	= numCol++;
    headers <<  _( "Vendor" 	);	_vendorCol	= numCol++;

    setColumnCount( numCol );
    setHeaderLabels(headers);

    setAllColumnsShowFocus( true );
    //setColumnAlignment( sizeCol(), Qt::AlignRight );

    setSortingEnabled( true );
    sortByColumn( nameCol(), Qt::AscendingOrder );



    fillList();
    selectSomething();

    yuiDebug() << "Creating product list done" << endl;
}


YQPkgProductList::~YQPkgProductList()
{
    // NOP
}


void
YQPkgProductList::fillList()
{
    clear();
    yuiDebug() << "Filling product list" << endl;

    for ( ZyppPoolIterator it = zyppProductsBegin();
	  it != zyppProductsEnd();
	  ++it )
    {
	ZyppProduct zyppProduct = tryCastToZyppProduct( (*it)->theObj() );

	if ( zyppProduct )
	{
	    addProductItem( *it, zyppProduct );
	}
	else
	{
	    yuiError() << "Found non-product selectable" << endl;
	}
    }

    yuiDebug() << "product list filled" << endl;
    resizeColumnToContents(_statusCol);
}


void
YQPkgProductList::addProductItem( ZyppSel	selectable,
				  ZyppProduct	zyppProduct )
{
    if ( ! selectable )
    {
	yuiError() << "NULL ZyppSel!" << endl;
	return;
    }

    new YQPkgProductListItem( this, selectable, zyppProduct );
}






YQPkgProductListItem::YQPkgProductListItem( YQPkgProductList * 	productList,
					    ZyppSel		selectable,
					    ZyppProduct 	zyppProduct )
    : YQPkgObjListItem( productList, selectable, zyppProduct )
    , _productList( productList )
    , _zyppProduct( zyppProduct )
{
    if ( ! _zyppProduct )
	_zyppProduct = tryCastToZyppProduct( selectable->theObj() );

    if ( ! _zyppProduct )
	return;

    setStatusIcon();

    if ( vendorCol() > -1 )
	setText( vendorCol(), zyppProduct->vendor() );

}


YQPkgProductListItem::~YQPkgProductListItem()
{
    // NOP
}




void
YQPkgProductListItem::applyChanges()
{
    solveResolvableCollections();
}



#include "YQPkgProductList.moc"
