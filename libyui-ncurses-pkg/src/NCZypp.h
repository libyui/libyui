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

  File:		NCZypp.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef NCZypp_h
#define NCZypp_h

#include <set>
#include <zypp/ui/Status.h>
#include <zypp/ui/Selectable.h>
#include <zypp/ResObject.h>
#include <zypp/Package.h>
#include <zypp/Pattern.h>
#include <zypp/Product.h>
//#include <zypp/Language.h>
#include <zypp/Patch.h>
#include <zypp/ZYppFactory.h>
#include <zypp/ResPoolProxy.h>


using zypp::ui::S_Protected;           
using zypp::ui::S_Taboo;               
using zypp::ui::S_Del;                 
using zypp::ui::S_Update;              
using zypp::ui::S_Install;             
using zypp::ui::S_AutoDel;             
using zypp::ui::S_AutoUpdate;          
using zypp::ui::S_AutoInstall;         
using zypp::ui::S_KeepInstalled;       
using zypp::ui::S_NoInst;              


//
// Typedefs to make those nested namespaces human-readable
//

typedef zypp::ui::Status			ZyppStatus;
typedef zypp::ui::Selectable::Ptr		ZyppSel;
typedef zypp::ResObject::constPtr		ZyppObj;
typedef zypp::Package::constPtr			ZyppPkg;
typedef zypp::Pattern::constPtr			ZyppPattern;
//typedef zypp::Language::constPtr		ZyppLang;
typedef zypp::Patch::constPtr			ZyppPatch;
typedef zypp::Product::constPtr			ZyppProduct;
typedef zypp::Repository			ZyppRepo;

typedef zypp::ResPoolProxy			ZyppPool;
typedef zypp::ResPoolProxy::const_iterator	ZyppPoolIterator;
typedef zypp::ResPoolProxy::repository_iterator	ZyppRepositoryIterator;


inline ZyppPool		zyppPool()		{ return zypp::getZYpp()->poolProxy();	}

template<class T> ZyppPoolIterator zyppBegin()	{ return zyppPool().byKindBegin<T>();	}
template<class T> ZyppPoolIterator zyppEnd()	{ return zyppPool().byKindEnd<T>();	}

inline ZyppPoolIterator zyppPkgBegin()		{ return zyppBegin<zypp::Package>();	}
inline ZyppPoolIterator zyppPkgEnd()		{ return zyppEnd<zypp::Package>();	}

inline ZyppPoolIterator zyppPatternsBegin()	{ return zyppBegin<zypp::Pattern>();	}
inline ZyppPoolIterator zyppPatternsEnd()	{ return zyppEnd<zypp::Pattern>();	}

//inline ZyppPoolIterator zyppLangBegin()		{ return zyppBegin<zypp::Language>();	}
//inline ZyppPoolIterator zyppLangEnd()		{ return zyppEnd<zypp::Language>();	}

inline ZyppPoolIterator zyppPatchesBegin()	{ return zyppBegin<zypp::Patch>();	}
inline ZyppPoolIterator zyppPatchesEnd()	{ return zyppEnd<zypp::Patch>();	}

inline ZyppPoolIterator zyppProductsBegin()	{ return zyppBegin<zypp::Product>();	}
inline ZyppPoolIterator zyppProductsEnd()	{ return zyppEnd<zypp::Product>();	}

inline ZyppRepositoryIterator ZyppRepositoriesBegin() { return zyppPool().knownRepositoriesBegin(); }
inline ZyppRepositoryIterator ZyppRepositoriesEnd()   { return zyppPool().knownRepositoriesEnd(); }


inline ZyppPkg		tryCastToZyppPkg( ZyppObj zyppObj )
{
    return zypp::dynamic_pointer_cast<const zypp::Package>( zyppObj );
}

inline ZyppPattern 	tryCastToZyppPattern( ZyppObj zyppObj )
{
    return zypp::dynamic_pointer_cast<const zypp::Pattern>( zyppObj );
}

#if 0
inline ZyppLang		tryCastToZyppLang( ZyppObj zyppObj )
{
    return zypp::dynamic_pointer_cast<const zypp::Language>( zyppObj );
}
#endif
inline ZyppPatch	tryCastToZyppPatch( ZyppObj zyppObj )
{
    return zypp::dynamic_pointer_cast<const zypp::Patch>( zyppObj );
}

inline ZyppProduct	tryCastToZyppProduct( ZyppObj zyppObj )
{
    return zypp::dynamic_pointer_cast<const zypp::Product>( zyppObj );
}

template<typename T> bool inContainer( const std::set<T> & container, T search )
{
    return container.find( search ) != container.end();
}


template<typename T> bool bsearch( const std::vector<T> & sorted_vector, T search )
{
    return binary_search( sorted_vector.begin(), sorted_vector.end(), search);
}

inline bool sortByName( ZyppSel ptr1, ZyppSel ptr2 )
{
    return( ptr1->name() < ptr2->name() );
}
#endif // NCZypp_h
