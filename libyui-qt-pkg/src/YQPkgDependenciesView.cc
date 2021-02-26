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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

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

    QString html_text = htmlStart();
    html_text += htmlHeading( selectable );

    ZyppObj candidate = selectable->candidateObj();
    ZyppObj installed = selectable->installedObj();

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

    html_text += htmlEnd();

    setHtml( html_text );
}


QString
YQPkgDependenciesView::simpleTable( ZyppObj pkg )
{
    QString html = "<br>" +
	table(
	      row( hcell( _( "Version:" ) ) + cell( pkg->edition().asString()	) ) +

	      // Intentionally NOT translating the RPM depencency tags:
	      // They make only sense to users who have some basic knowledge what they are anyway.

	      row( "Provides:",		pkg->dep( zypp::Dep::PROVIDES		) ) +
	      row( "Prerequires:",	pkg->dep( zypp::Dep::PREREQUIRES	) ) +
	      row( "Requires:",		pkg->dep( zypp::Dep::REQUIRES		) ) +
	      row( "Conflicts:",	pkg->dep( zypp::Dep::CONFLICTS		) ) +
	      row( "Obsoletes:",	pkg->dep( zypp::Dep::OBSOLETES		) ) +
	      row( "Recommends:",	pkg->dep( zypp::Dep::RECOMMENDS		) ) +
	      row( "Suggests:",		pkg->dep( zypp::Dep::SUGGESTS		) ) +
	      row( "Enances:",		pkg->dep( zypp::Dep::ENHANCES		) ) +
	      row( "Supplements:",	pkg->dep( zypp::Dep::SUPPLEMENTS	) )
	      );

    return html;
}


QString
YQPkgDependenciesView::complexTable( ZyppObj installed, ZyppObj candidate )
{
    ZyppObj p1 = candidate;
    ZyppObj p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html = "<br>" +
	table(
	      row( hcell( QString( "" ) ) + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>" ) ) +

	      row( hcell( _( "Version:" ) ) + cell( p1->edition().asString()	) + cell( p2->edition().asString()	) ) +

	      row( "Provides:",		p1->dep( zypp::Dep::PROVIDES	), p2->dep( zypp::Dep::PROVIDES		) ) +
	      row( "Prerequires:",	p1->dep( zypp::Dep::PREREQUIRES	), p2->dep( zypp::Dep::PREREQUIRES	) ) +
	      row( "Requires:",		p1->dep( zypp::Dep::REQUIRES	), p2->dep( zypp::Dep::REQUIRES		) ) +
	      row( "Conflicts:",	p1->dep( zypp::Dep::CONFLICTS	), p2->dep( zypp::Dep::CONFLICTS	) ) +
	      row( "Obsoletes:",	p1->dep( zypp::Dep::OBSOLETES	), p2->dep( zypp::Dep::OBSOLETES	) ) +
	      row( "Recommends:",	p1->dep( zypp::Dep::RECOMMENDS	), p2->dep( zypp::Dep::RECOMMENDS	) ) +
	      row( "Suggests:",		p1->dep( zypp::Dep::SUGGESTS	), p2->dep( zypp::Dep::SUGGESTS		) ) +
	      row( "Enances:",		p1->dep( zypp::Dep::ENHANCES	), p2->dep( zypp::Dep::ENHANCES		) ) +
	      row( "Supplements:",	p1->dep( zypp::Dep::SUPPLEMENTS	), p2->dep( zypp::Dep::SUPPLEMENTS	) )
	      );

    return html;
}


QString
YQPkgDependenciesView::row( const QString &		heading,
			    const zypp::Capabilities & 	capSet )
{
    QString content = htmlLines( capSet );

    if ( content.isEmpty() )
	return "";

    return QString( "<tr>" ) +
	hcell( heading ) +
	"<td>" + content + "</td>"
	+ "</tr>";
}


QString
YQPkgDependenciesView::row( const QString & 		heading,
			    const zypp::Capabilities & 	capSet1,
			    const zypp::Capabilities & 	capSet2 )
{
    QString content1 = htmlLines( capSet1 );
    QString content2 = htmlLines( capSet2 );

    if ( content1.isEmpty() && content2.isEmpty() )
	return "";

    return QString( "<tr>" ) +
	hcell( heading ) +
	"<td>" + content1 + "</td>" +
	"<td>" + content2 + "</td>" +
	"</tr>";
}


QString
YQPkgDependenciesView::htmlLines( const zypp::Capabilities & capSet )
{
    QString html;

    for ( zypp::Capabilities::const_iterator it = capSet.begin();
	  it != capSet.end();
	  ++it )
    {
	if ( ! html.isEmpty() )
	    html += "<br>";

	html += htmlEscape( ( *it).asString().c_str() );
    }

    return html;
}


#include "YQPkgDependenciesView.moc"
