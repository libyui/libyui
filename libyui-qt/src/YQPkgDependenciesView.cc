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

#include <Y2PM.h>
#include <y2pm/PMManager.h>

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
YQPkgDependenciesView::showDetails( PMObjectPtr pmObj )
{
    _pmObj = pmObj;

    if ( ! pmObj )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( pmObj );

    string name = pmObj->name();
    y2debug( "Showing technical details for PMObject %s", name.c_str() );

    PMPackagePtr candidate = pmObj->getCandidateObj();
    PMPackagePtr installed = pmObj->getInstalledObj();

#if 0
    html_text += complexTable( installed, installed );
#else
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
#endif

    setTextFormat( Qt::RichText );
    setText( html_text );
}


QString
YQPkgDependenciesView::simpleTable( PMPackagePtr pkg )
{
    QString html = "<br>" +
	table(
	       row( hcell( _( "Version:"	) )	+ cell( pkg->version() + "-" + pkg->release() ) ) +
	       row( hcell( _( "Provides:"	) )	+ cell( pkg->provides()		) ) +
	       row( hcell( _( "Requires:<br>(at run time)" ) )
							+ cell( pkg->requires()		) ) +
	       row( hcell( _( "Prerequires:<br>(at installation time)"	) )
							+ cell( pkg->prerequires()	) ) +
	       row( hcell( _( "Obsoletes:"	) )	+ cell( pkg->obsoletes()	) ) +
	       row( hcell( _( "Conflicts:"	) )	+ cell( pkg->conflicts()	) )
	       );

    return html;
}


QString
YQPkgDependenciesView::complexTable( PMPackagePtr installed, PMPackagePtr candidate )
{
    PMPackagePtr p1 = candidate;
    PMPackagePtr p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html = "<br>" +
	table(
	       row( hcell( QString("") )	    + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>"     ) ) +

	       row( hcell( _( "Version:"	) ) + cell( p1->version() + "-" + p1->release() ) + cell( p2->version() + "-" + p2->release() ) ) +
	       row( hcell( _( "Provides:"	) ) + cell( p1->provides()			) + cell( p2->provides()		      ) ) +
	       row( hcell( _( "Requires:"	) ) + cell( p1->requires()			) + cell( p2->requires()		      ) ) +
	       row( hcell( _( "Prerequires:"	) ) + cell( p1->prerequires()			) + cell( p2->prerequires()		      ) ) +
	       row( hcell( _( "Obsoletes:"	) ) + cell( p1->obsoletes()			) + cell( p2->obsoletes()		      ) ) +
	       row( hcell( _( "Conflicts:"	) ) + cell( p1->conflicts()			) + cell( p2->conflicts()		      ) )
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

	html += htmlEscape( (*it).asString().c_str() );
	++it;
    }

    return "<td>" + html + "</td>";
}


#include "YQPkgDependenciesView.moc.cc"
