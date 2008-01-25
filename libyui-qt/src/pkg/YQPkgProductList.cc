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

  Textdomain "packages-qt"

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

#if FIXME
    int numCol = 0;
    addColumn( "" );			_statusCol	= numCol++;
    addColumn( _( "Product"	) );	_nameCol	= numCol++;
    addColumn( _( "Summary"	) );	_summaryCol	= numCol++;
    addColumn( _( "Version" 	) );	_versionCol	= numCol++;
    addColumn( _( "Vendor" 	) );	_vendorCol	= numCol++;
    setAllColumnsShowFocus( true );
    setColumnAlignment( sizeCol(), Qt::AlignRight );

    setSorting( nameCol() );
    fillList();
    selectSomething();
#endif

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
