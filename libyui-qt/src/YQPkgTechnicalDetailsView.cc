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

  File:	      YQPkgTechnicalDetailsView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPkgTechnicalDetailsView.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


YQPkgTechnicalDetailsView::YQPkgTechnicalDetailsView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgTechnicalDetailsView::~YQPkgTechnicalDetailsView()
{
    // NOP
}


void
YQPkgTechnicalDetailsView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( selectable );

    ZyppPkg candidate = tryCastToZyppPkg( selectable->candidateObj() );
    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( candidate && installed && candidate != installed )
    {
	html_text += complexTable( selectable, installed, candidate );
    }
    else
    {
	if ( candidate )
	    html_text += simpleTable( selectable, candidate );

	if ( installed )
	    html_text += simpleTable( selectable, installed );
    }

    setTextFormat( Qt::RichText );
    setText( html_text );
}


QString
YQPkgTechnicalDetailsView::authorsListCell( ZyppPkg pkg ) const
{
    QString html = "<td align=top>";
    QString line;
    list<string> authors = pkg->authors();
    list<string>::const_iterator it = authors.begin();

    while ( it != authors.end() )
    {
	line = fromUTF8( *it );
	line = htmlEscape( line );
	html += line + "<br>";
	++it;
    }

    html += "</td>";

    return html;
}


QString
YQPkgTechnicalDetailsView::formatRpmGroup( ZyppPkg pkg ) const
{
    QStringList groups = QStringList::split( '/',	// delimiter
					     fromUTF8( pkg->group() ),
					     false );	// allow empties

    // Translate group path components

    QStringList translated;

    for ( QValueList<QString>::const_iterator it = groups.begin();
	  it != groups.end();
	  ++it )
    {
	translated.append( QString::fromUtf8( dgettext( "rpm-groups", (*it).utf8() ) ) );
    }

    return translated.join( "/" );
}


QString
YQPkgTechnicalDetailsView::simpleTable( ZyppSel selectable,
					ZyppPkg		pkg )
{
    QString html = "<br>" +
	table(
	       row( hcell( _( "Version:"	) ) + cell( pkg->edition().asString()		) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( pkg->buildtime()			) ) +
	       ( pkg == selectable->installedObj() ?
		 row( hcell( _( "Install Time:" ) ) + cell( pkg->installtime()			) )
		 : "" ) +
	       row( hcell( _( "Package Group:"	) ) + cell( formatRpmGroup( pkg )		) ) +
	       row( hcell( _( "License:"	) ) + cell( pkg->license()			) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( pkg->size().asString()		) ) +
	       row( hcell( _( "Archive Size:"   ) ) + cell( pkg->archivesize().asString()	) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( pkg->distribution()			) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( pkg->vendor()			) ) +
	       row( hcell( _( "Packager:"	) ) + cell( pkg->packager()			) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( pkg->arch().asString()		) ) +
	       row( hcell( _( "OS:"		) ) + cell( pkg->os()				) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( pkg->buildhost()			) ) +
	       row( hcell( _( "URL:"		) ) + cell( pkg->url()				) ) +
#ifdef FIXME_missing_in_zypp
	       row( hcell( _( "Source RPM:"	) ) + cell( pkg->sourceloc()			) ) +
#endif
	       row( hcell( _( "Media No.:"	) ) + cell( pkg->sourceMediaNr()		) ) +
	       row( hcell( _( "Authors:"	) ) + authorsListCell( pkg			) )
	       );

    return html;
}


QString
YQPkgTechnicalDetailsView::complexTable( ZyppSel	selectable,
					 ZyppPkg	installed,
					 ZyppPkg	candidate )
{
    ZyppPkg p1 = candidate;
    ZyppPkg p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html = "<br>" +
	table(
	       row( hcell( QString( "" ) )	    + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>"     ) ) +

	       row( hcell( _( "Version:"	) ) + cell( p1->edition().asString()		) + cell( p2->edition().asString()	) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( p1->buildtime()			) + cell( p2->buildtime()		) ) +
	       row( hcell( _( "Install Time:"	) ) + cell( p1->installtime()			) + cell( p2->installtime()		) ) +
	       row( hcell( _( "Package Group:"	) ) + cell( formatRpmGroup( p1 )		) + cell( formatRpmGroup( p2 )		) ) +
	       row( hcell( _( "License:"	) ) + cell( p1->license()			) + cell( p2->license()			) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( p1->size().asString()		) + cell( p2->size().asString()		) ) +
	       row( hcell( _( "Archive Size:"   ) ) + cell( p1->archivesize().asString()	) + cell( p2->archivesize().asString()	) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( p1->distribution()			) + cell( p2->distribution()		) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( p1->vendor()			) + cell( p2->vendor()			) ) +
	       row( hcell( _( "Packager:"	) ) + cell( p1->packager()			) + cell( p2->packager()		) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( p1->arch().asString()		) + cell( p2->arch().asString()		) ) +
	       row( hcell( _( "OS:"		) ) + cell( p1->os()				) + cell( p2->os()			) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( p1->buildhost()			) + cell( p2->buildhost()		) ) +
	       row( hcell( _( "URL:"		) ) + cell( p1->url()				) + cell( p2->url()			) ) +
#ifdef FIXME_missing_in_zypp
	       row( hcell( _( "Source RPM:"	) ) + cell( p1->sourceloc()			) + cell( p2->sourceloc()		) ) +
#endif
	       row( hcell( _( "Media No.:"	) ) + cell( p1->sourceMediaNr()			) + cell( p2->sourceMediaNr()		) ) +
	       row( hcell( _( "Authors:"	) ) + authorsListCell( p1			) + authorsListCell( p2			) )
	       );

    return html;
}



#include "YQPkgTechnicalDetailsView.moc"
