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

  File:	      YQPkgDependenciesView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qdatetime.h>
#include "YQPkgDependenciesView.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgDependenciesView::YQPkgDependenciesView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgDependenciesView::~YQPkgDependenciesView()
{
    // NOP
}


void
YQPkgDependenciesView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;
    
    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( selectable );

    y2debug( "Showing technical details for zypp::ResObject %s", selectable->theObj()->name.c_str() );

    ZyppPkg candidate = selectable->candidateObj();
    ZyppPkg installed = selectable->installedObj();

    if ( candidate && installed && candidate != installed )
    {
	html_text += complexTable( installed, candidate );
    }
    else
    {
	if ( candidate )
	    html_text += simpleTable( candidate );

	if ( installed )
	    html_text += simpleTable( installed );
    }

    setTextFormat( Qt::RichText );
    setText( html_text );
}


QString
YQPkgDependenciesView::simpleTable( ZyppPkg pkg )
{
    /************************

    Resolvable::deps[Dep::REQUIRES]
    Resolvable::deps[Dep::PROVIDES]

    see libzypp/zypp/target/store/serialize.cc

    zypp/Dependencies.c :

    str << "Dependencies: [" << endl;
    if ( ! obj[Dep::PROVIDES].empty() )
      str << "PROVIDES:" << endl << obj[Dep::PROVIDES];
    if ( ! obj[Dep::PREREQUIRES].empty() )
      str << "PREREQUIRES:" << endl << obj[Dep::PREREQUIRES];
    if ( ! obj[Dep::REQUIRES].empty() )
      str << "REQUIRES:" << endl << obj[Dep::REQUIRES];
    if ( ! obj[Dep::CONFLICTS].empty() )
      str << "CONFLICTS:" << endl << obj[Dep::CONFLICTS];
    if ( ! obj[Dep::OBSOLETES].empty() )
      str << "OBSOLETES:" << endl << obj[Dep::OBSOLETES];
    if ( ! obj[Dep::RECOMMENDS].empty() )
      str << "RECOMMENDS:" << endl << obj[Dep::RECOMMENDS];
    if ( ! obj[Dep::SUGGESTS].empty() )
      str << "SUGGESTS:" << endl << obj[Dep::SUGGESTS];
    if ( ! obj[Dep::FRESHENS].empty() )
      str << "FRESHENS:" << endl << obj[Dep::FRESHENS];
    if ( ! obj[Dep::ENHANCES].empty() )
      str << "ENHANCES:" << endl << obj[Dep::ENHANCES];
    return str << "]";


    **************************/
    QString html = "<br>" +
	table(
	       row( hcell( _( "Version:" ) )	+ cell( pkg->version() + "-" + pkg->release() ) ) +
	       row( hcell( _( "Provides:" ) )	+ cell( pkg->provides()		) ) +
	       row( hcell( _( "Requires:<br>(at run time)" ) )
						+ cell( pkg->requires()		) ) +
	       row( hcell( _( "Prerequires:<br>(at installation time)" ) )
							+ cell( pkg->prerequires()	) ) +
	       row( hcell( _( "Obsoletes:" ) )	+ cell( pkg->obsoletes()	) ) +
	       row( hcell( _( "Conflicts:" ) )	+ cell( pkg->conflicts()	) )
	       );

    return html;
}


QString
YQPkgDependenciesView::complexTable( ZyppPkg installed, ZyppPkg candidate )
{
    ZyppPkg p1 = candidate;
    ZyppPkg p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html = "<br>" +
	table(
	      row( hcell( QString( "" ) )	    + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>"     ) ) +

	      row( hcell( _( "Version:" ) ) + cell( p1->version() + "-" + p1->release() ) + cell( p2->version() + "-" + p2->release() ) ) +
	      row( hcell( _( "Provides:" ) ) + cell( p1->provides()			) + cell( p2->provides()		      ) ) +
	      row( hcell( _( "Requires:" ) ) + cell( p1->requires()			) + cell( p2->requires()		      ) ) +
	      row( hcell( _( "Prerequires:" ) ) + cell( p1->prerequires()		) + cell( p2->prerequires()		      ) ) +
	      row( hcell( _( "Obsoletes:" ) ) + cell( p1->obsoletes()			) + cell( p2->obsoletes()		      ) ) +
	      row( hcell( _( "Conflicts:" ) ) + cell( p1->conflicts()			) + cell( p2->conflicts()		      ) )
	      );

    return html;
}


QString
YQPkgDependenciesView::cell( const PMSolvable::PkgRelList_type & list )
{
    QString html;

    PMSolvable::PkgRelList_const_iterator it = list.begin();

    while ( it != list.end() )
    {
	if ( ! html.isEmpty() )
	    html += "<br>";

	html += htmlEscape( ( *it).asString().c_str() );
	++it;
    }

    return "<td>" + html + "</td>";
}


#include "YQPkgDependenciesView.moc"
